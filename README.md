# Otus homework 6: matrix

## Особенности моей реализации следующие:
1)	Все основные операции и требования к реализации 2-мерной разряженной бесконечной матрицы – выполнены.

2)	Но на самом деле реализована N-мерная матрица.<br>
    Размерность матрицы задается 3-м параметром. Например, 3-мерная матрица:
```cpp
Matrix<int,-1, 3> matrix;
```
3)	Реализована каноническая форма оператора =
    Следующее выражение, работает:
```cpp
 ((matrix[100][100] = 314) = 0) = 217
 ```
4)	Реализация основана на *unordered_map*, хеш вычисляется из координат, записываемого значения в марицу
5)	Код снабжен проверкой времени компиляции, если количество скобочек [] при доступе к элементу не соответствует размерности матрицы
6)  Примеры работы с N-мерной матрицей и оператором = можно посмотреть в тестах.

    
Документацию и дополнительное описание проекта можно найти здесь:
https://rra-roro.github.io/homework_6_matrix
