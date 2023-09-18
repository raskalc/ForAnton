#include <mpi.h>
#include <iostream>
#include <random>
#include <iomanip>
#include <limits>

/* Даны две матрицы размером n*n (n – количество запущенных процессов).
   Матрицы распределены между процессами. Каждый процесс генерирует строку матрицы A
   и строку матрицы B. Память отводится только для строки A и строки B.
   Для вычисления по формуле процесс использует свою строку матрицы A, а строки
   матрицы B передаются последовательно каждому процессу, при этом используется виртуальная
   топология «кольцо». Результат – каждый процесс вычисляет одно значение и выводит его. */

using namespace std;

void getRandomRow(int *row, int size) {
    // границы, в пределах которых генерируются рандомные числа
    const int min = 1, max = 20;

    random_device r;
    default_random_engine e(r());
    uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < size; i++) {
        row[i] = dist(e);
    }
}

void printRowToConsole(int *row, int size, char let, int rank) {
    cout << "Row " << rank << " of matrix " << let << endl;
    for (int i = 0; i < size; i++) {
        cout << setw(4) << row[i];
    }
    cout << endl;
}

int findMaxElemOfRow(int* row, int size) {
    int max = row[0];
    for (int i = 1; i < size; i++) {
        if (row[i] > max) {
            max = row[i];
        }
    }
    return max;
}

int task5(int argc, char **argv) {
    const int NUM_DIMS = 1;
    const int TAG = 5;

    int rank, size, source, destination;
    int dims[NUM_DIMS]{}, periods[NUM_DIMS]{}, coords[NUM_DIMS];
    int reorder = 0;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    for (int i = 0; i < NUM_DIMS; i++) {
        dims[i] = 0;
        periods[i] = 1;
    }

    MPI_Dims_create(size, NUM_DIMS, dims);
    MPI_Cart_create(MPI_COMM_WORLD, NUM_DIMS, dims, periods, reorder, &comm);
    MPI_Cart_coords(comm, rank, NUM_DIMS, coords);
    MPI_Cart_shift(comm, 0, -1, &source, &destination);

    int *rowA = new int[size];
    int *rowB = new int[size];

    int *mini = new int[size];
    for (int i = 0; i < size; i++) {
        mini[i] = INT_MAX;
    }

    getRandomRow(rowA, size);
    printRowToConsole(rowA, size, 'A', rank);
    getRandomRow(rowB, size);
    printRowToConsole(rowB, size, 'B', rank);

    for (int k = 0; k < size; k++) {
        int n = (rank + k) % size;

        for (int j = 0; j < size; j++) {
            int temp = rowA[n] * rowB[k];
            if (temp < mini[k]) {
                mini[k] = temp;
            }
            MPI_Sendrecv_replace(rowB, size, MPI_INT, destination, TAG, source, TAG, comm, &status);
        }
    }

    int res = findMaxElemOfRow(mini, size);

    std::cout << "res " << rank << " = " << res << endl;

    delete[] rowA;
    delete[] rowB;
    delete[] mini;
    MPI_Comm_free(&comm);
    MPI_Finalize();
    return 0;
}


/*
getRandomRow генерирует случайныеисла и сохраняет их в массиве row заданного размера.
printRowToConsole выводит содержимое массива row в консоль с указанием номера строки и буквы матрицы.
findMaxElemOfRow находит максимальное значение в массиве row.

Создание виртуальной топологии "кольцо" с использованием MPI_Cart_create. Каждый процесс связан с двумя соседними процессами.
Выделение памяти для массивов rowA, rowB и mini, которые будут хранить строки матриц и результаты вычислений.
Генерация случайных значений для rowA и rowB, а затем вывод их содержимого в консоль.
Внутренний цикл считает значения для массива mini в соответствии с формулой.
 Значения матрицы rowB сдвигаются между процессами виртуальной топологии "кольцо" с использованием MPI_Sendrecv_replace.
Нахождение максимального элемента в массиве mini.
Вывод результата в консоль.
*/