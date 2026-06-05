from __future__ import annotations

from collections import Counter
from dataclasses import dataclass
import heapq
import itertools
from typing import Dict, Optional, Tuple


@dataclass
class Node:
    """Узел дерева Хаффмана.

    Атрибут symbol заполнен только у листьев. Внутренний узел хранит сумму
    частот двух дочерних поддеревьев и используется при декодировании.
    """

    freq: int
    symbol: Optional[int] = None
    left: Optional["Node"] = None
    right: Optional["Node"] = None

    def is_leaf(self) -> bool:
        return self.symbol is not None


def build_tree(data: bytes) -> Optional[Node]:
    """Строит дерево Хаффмана для входной байтовой строки."""
    if not data:
        return None

    # heapq хранит минимальную кучу; второй элемент tuple нужен, чтобы узлы
    # с одинаковыми частотами сравнивались стабильно и без ошибки TypeError.
    counter = itertools.count()
    heap: list[Tuple[int, int, Node]] = []
    for symbol, freq in Counter(data).items():
        heapq.heappush(heap, (freq, next(counter), Node(freq=freq, symbol=symbol)))

    # Если во входе один уникальный байт, дерево состоит из одного листа.
    while len(heap) > 1:
        freq1, _, left = heapq.heappop(heap)
        freq2, _, right = heapq.heappop(heap)
        parent = Node(freq=freq1 + freq2, left=left, right=right)
        heapq.heappush(heap, (parent.freq, next(counter), parent))

    return heap[0][2]


def make_codes(root: Optional[Node]) -> Dict[int, str]:
    """Формирует словарь: байт -> битовая строка."""
    if root is None:
        return {}

    codes: Dict[int, str] = {}

    def walk(node: Node, prefix: str) -> None:
        if node.is_leaf():
            # Для единственного символа нужен хотя бы один бит кода.
            codes[node.symbol] = prefix or "0"
            return
        if node.left is not None:
            walk(node.left, prefix + "0")
        if node.right is not None:
            walk(node.right, prefix + "1")

    walk(root, "")
    return codes


def _pack_bits(bit_string: str) -> tuple[bytes, int]:
    """Упаковывает строку из '0'/'1' в байты и возвращает число полезных битов."""
    bit_length = len(bit_string)
    padding = (-bit_length) % 8
    bit_string += "0" * padding
    out = bytearray()
    for i in range(0, len(bit_string), 8):
        out.append(int(bit_string[i : i + 8], 2))
    return bytes(out), bit_length


def _unpack_bits(payload: bytes, bit_length: int) -> str:
    """Преобразует упакованные байты обратно в битовую строку."""
    return "".join(f"{byte:08b}" for byte in payload)[:bit_length]


def encode(data: bytes) -> dict:
    """Кодирует данные и возвращает учебный контейнер с метаданными."""
    root = build_tree(data)
    codes = make_codes(root)
    bit_string = "".join(codes[byte] for byte in data)
    payload, bit_length = _pack_bits(bit_string)
    return {
        "algorithm": "huffman",
        "codes": {str(k): v for k, v in codes.items()},
        "payload": payload,
        "bit_length": bit_length,
        "original_size": len(data),
    }


def decode(container: dict) -> bytes:
    """Декодирует контейнер, созданный функцией encode."""
    codes = {int(k): v for k, v in container["codes"].items()}
    reverse = {code: symbol for symbol, code in codes.items()}
    bits = _unpack_bits(container["payload"], container["bit_length"])

    current = ""
    out = bytearray()
    for bit in bits:
        current += bit
        if current in reverse:
            out.append(reverse[current])
            current = ""
    return bytes(out)


def compressed_size_bits(container: dict) -> int:
    """Оценивает размер сжатого потока с учётом полезных битов полезной нагрузки."""
    # В промышленном формате нужно компактно хранить дерево/длины кодов.
    # Здесь метаданные хранятся отдельно, поэтому для сравнения алгоритма
    # учитываем битовую длину кодированного сообщения.
    return int(container["bit_length"])
