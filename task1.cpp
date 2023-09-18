/* 3. Два вектора A и B размерности N распределены между процессами: значения векторов процессов
   с четными номерами составляют вектор A, значения векторов процессов с нечетными номерами составляют вектор B.
   Вычислить скалярное произведение (A,B). Два процесса, содержащие соответствующие компоненты векторов,
   обмениваются сообщениями, один из них выполняет сложение и отправляет результат нулевому процессу, который вычисляет
   и выводит результат.*/

#include "mpi.h"
#include <vector>
#include <random>
#include <iostream>

using namespace std;

vector<int> getRandomVector(int size) {
    // границы, в пределах которых генерируются рандомные числа
    const int min = 0;
    const int max = 100;

    vector<int> v;

    random_device r;
    default_random_engine e(r());
    uniform_int_distribution<int> dist(min, max);

    for (int i = 0; i < size; i++) {
        int num = dist(e); // генерируем рандомное число

        if (find(v.begin(), v.end(), num) == v.end()) // если числа нет в векторе
        {
            v.push_back(num); // добавляем число в вектор
        } else {
            i--; // иначе повторяем итерацию
        }
    }
    return v;
}

int multiplyVectors(vector<int> &v1, vector<int> &v2, int size) {
    int result = 0;
    for (int i = 0; i < size; i++) {
        result += v1[i] * v2[i];
    }
    return result;
}

void printVectorToConsole(vector<int> &v) {
    for (int i: v) {
        cout << i << ' ';
    }
    cout << endl;
}

int task1(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int m = 7; // размерность вектора

    vector<int> vec1;

    if (rank % 2 == 0 && rank < size - 1) {
        vec1 = getRandomVector(m);
        fprintf(stdout, "Process %i generated vector:\n", rank);
        printVectorToConsole(vec1);

        int dest = rank + 1;
        int tag = rank;

        MPI_Send(vec1.data(), (int) vec1.size(), MPI_INT, dest, tag, MPI_COMM_WORLD);
    } else {
        if (rank % 2 != 0) {
            vector<int> vec2 = getRandomVector(m);
            fprintf(stdout, "Process %i generated vector:\n", rank);
            printVectorToConsole(vec2);

            int source = rank - 1;
            int tag = rank - 1;

            vec1.resize(m);
            MPI_Recv(vec1.data(), (int) vec1.size(), MPI_INT, source, tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            int productPartSend = multiplyVectors(vec1, vec2, m);
            MPI_Send(&productPartSend, 1, MPI_INT, 0, rank, MPI_COMM_WORLD);
        }
    }

    if (rank == 0) {
        int product = 0;
        int productPartRecv;
        for (int i = 1; i < size; i += 2) {
            MPI_Recv(&productPartRecv, 1, MPI_INT, i, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            product += productPartRecv;
        }
        fprintf(stdout, "Product of vectors = %i\n", product);
    }

    MPI_Finalize();
    return 0;
}
/*
getRandomVector(int size): Генерирует случайный вектор размером size, содержащий случайные целочисленные значения от 0 до 100.
Гарантируется, что в векторе нет повторяющихся чисел.

multiplyVectors(vector<int> &v1, vector<int> &v2, int size): Вычисляет скалярное произведение двух векторов v1 и v2 размером size.

printVectorToConsole(vector<int> &v): Выводит вектор v в консоль.

task1(int argc, char *argv[]): Главная функция программы, в которой выполняются параллельные вычисления с использованием MPI.

Если ранг процесса четный и меньше общего числа процессов минус один, то процесс генерирует случайный вектор vec1,
выводит его в консоль и отправляет его следующему процессу (с четным номером).
Если ранг процесса нечетный, то процесс генерирует случайный вектор vec2,
выводит его в консоль. Затем процесс получает вектор vec1 от предыдущего процесса (с четным номером),
вычисляет частичное скалярное произведение productPartSend между vec1 и vec2, и отправляет его процессу с рангом 0.
Процесс с рангом 0 получает частичные результаты от нечетных процессов и суммирует их,
чтобы получить итоговое скалярное произведение.
Затем он выводит итоговый результат в консоль.
*/