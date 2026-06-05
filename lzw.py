"""Учебная реализация алгоритма LZW для байтовых строк.

Алгоритм начинает со словаря всех одиночных байтов (0..255), затем добавляет
новые последовательности по мере чтения входа. Декодер восстанавливает тот же
словарь синхронно, поэтому словарь не нужно хранить в сжатом файле.
"""
from __future__ import annotations

from typing import List


def encode(data: bytes, max_code: int = 4095) -> dict:
    """Кодирует байты в список кодов LZW.

    max_code=4095 соответствует 12-битному словарю, как в классических
    практических вариантах LZW. При заполнении словаря новые строки перестают
    добавляться, что упрощает учебную реализацию.
    """
    if not data:
        return {"algorithm": "lzw", "codes": [], "original_size": 0, "code_width": 12}

    dictionary = {bytes([i]): i for i in range(256)}
    next_code = 256
    w = bytes([data[0]])
    result: List[int] = []

    for byte in data[1:]:
        k = bytes([byte])
        wk = w + k
        if wk in dictionary:
            w = wk
        else:
            result.append(dictionary[w])
            if next_code <= max_code:
                dictionary[wk] = next_code
                next_code += 1
            w = k

    result.append(dictionary[w])
    return {
        "algorithm": "lzw",
        "codes": result,
        "original_size": len(data),
        "code_width": 12,
    }


def decode(container: dict, max_code: int = 4095) -> bytes:
    """Восстанавливает исходные байты из списка кодов LZW."""
    codes: List[int] = list(container["codes"])
    if not codes:
        return b""

    dictionary = {i: bytes([i]) for i in range(256)}
    next_code = 256

    w = dictionary[codes[0]]
    out = bytearray(w)

    for code in codes[1:]:
        if code in dictionary:
            entry = dictionary[code]
        elif code == next_code:
            # Особый случай LZW: код указывает на строку, которая создаётся
            # прямо сейчас. Она равна предыдущей строке плюс её первый байт.
            entry = w + w[:1]
        else:
            raise ValueError(f"Некорректный LZW-код: {code}")

        out.extend(entry)
        if next_code <= max_code:
            dictionary[next_code] = w + entry[:1]
            next_code += 1
        w = entry

    return bytes(out)


def compressed_size_bits(container: dict) -> int:
    """Оценивает размер LZW-потока при фиксированной ширине кода."""
    return len(container["codes"]) * int(container.get("code_width", 12))
