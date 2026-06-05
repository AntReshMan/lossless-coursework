"""Запускает мини-эксперименты для раздела курсовой работы."""
from __future__ import annotations

import csv
from pathlib import Path
from compressors import huffman, lzw

DATASETS = {
    "Повторяющийся текст": ("ABRACADABRA " * 600).encode("utf-8"),
    "Русский фрагмент": ("сжатие без потерь алгоритм хаффмана lzw " * 350).encode("utf-8"),
    "Псевдослучайные байты": __import__("random").Random(42).randbytes(6000),
}


def main() -> None:
    out_path = Path("results/experiment_results.csv")
    out_path.parent.mkdir(exist_ok=True)
    with out_path.open("w", newline="", encoding="utf-8") as f:
        writer = csv.writer(f)
        writer.writerow(["dataset", "algorithm", "original_bits", "compressed_bits", "ratio", "saved_percent"])
        for name, data in DATASETS.items():
            for algorithm, module in [("Huffman", huffman), ("LZW", lzw)]:
                container = module.encode(data)
                assert module.decode(container) == data
                original_bits = len(data) * 8
                compressed_bits = module.compressed_size_bits(container)
                ratio = compressed_bits / original_bits
                writer.writerow([name, algorithm, original_bits, compressed_bits, f"{ratio:.4f}", f"{100*(1-ratio):.2f}"])
    print(f"Результаты записаны в {out_path}")


if __name__ == "__main__":
    main()
