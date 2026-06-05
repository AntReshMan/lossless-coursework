# Курсовая работа: «Сжатие без потерь: от Хаффмана до LZW»

**Решетников Антон Владиславович**
**2 курс** 
**ИУСбд-03-24**

```text
compressors/huffman.py  — построение дерева Хаффмана, кодирование и декодирование
compressors/lzw.py      — кодирование и декодирование LZW
main.py                 — запуск алгоритма из командной строки
experiments.py          — мини-эксперименты и CSV-таблица результатов
tests/test_roundtrip.py — проверка обратимости алгоритмов
```

```bash
python main.py huffman data/example.txt
python main.py lzw data/example.txt
python experiments.py
```

```bash
git init
git add .
git commit -m "Add coursework source code"
git branch -M main
git remote add origin https://github.com/<username>/lossless-compression-coursework.git
git push -u origin main
```
