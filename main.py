"""Командная строка для учебного сравнения Хаффмана и LZW.

Примеры:
  python main.py huffman input.txt
  python main.py lzw input.txt
"""
from __future__ import annotations

import argparse
from pathlib import Path
from compressors import huffman, lzw


def run(algorithm: str, path: Path) -> None:
    data = path.read_bytes()
    module = huffman if algorithm == "huffman" else lzw
    packed = module.encode(data)
    restored = module.decode(packed)
    if restored != data:
        raise RuntimeError("Декодирование не совпало с исходным файлом")

    original_bits = len(data) * 8
    compressed_bits = module.compressed_size_bits(packed)
    ratio = compressed_bits / original_bits if original_bits else 0
    saved = 100 * (1 - ratio) if original_bits else 0

    print(f"Алгоритм: {algorithm}")
    print(f"Файл: {path}")
    print(f"Исходный размер: {len(data)} байт")
    print(f"Оценка сжатого размера: {compressed_bits} бит")
    print(f"Коэффициент compressed/original: {ratio:.3f}")
    print(f"Экономия: {saved:.1f}%")


def main() -> None:
    parser = argparse.ArgumentParser(description="Учебное сжатие без потерь")
    parser.add_argument("algorithm", choices=["huffman", "lzw"])
    parser.add_argument("input", type=Path)
    args = parser.parse_args()
    run(args.algorithm, args.input)


if __name__ == "__main__":
    main()
