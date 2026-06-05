# Курсовая работа: «Сжатие без потерь: от Хаффмана до LZW»

**ФИО:** [впишите ФИО]  
**Курс:** [впишите курс]  
**Группа:** [впишите группу]

Репозиторий содержит учебную реализацию двух алгоритмов сжатия без потерь:
кодирования Хаффмана и LZW. Код написан на Python 3, снабжён комментариями и
предназначен для сопровождения курсовой работы.

## Структура

```text
compressors/huffman.py  — построение дерева Хаффмана, кодирование и декодирование
compressors/lzw.py      — кодирование и декодирование LZW
main.py                 — запуск алгоритма из командной строки
experiments.py          — мини-эксперименты и CSV-таблица результатов
tests/test_roundtrip.py — проверка обратимости алгоритмов
```

## Запуск

```bash
python main.py huffman data/example.txt
python main.py lzw data/example.txt
python experiments.py
```

## Публикация на GitHub

1. Создайте новый репозиторий, например `lossless-compression-coursework`.
2. Скопируйте в него файлы из этой папки.
3. В README укажите свои ФИО, курс и группу.
4. Выполните:

```bash
git init
git add .
git commit -m "Add coursework source code"
git branch -M main
git remote add origin https://github.com/<username>/lossless-compression-coursework.git
git push -u origin main
```
