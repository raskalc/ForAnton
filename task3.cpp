/*
Каждый процесс заполняет свой массив размером n случайными числами.
Для решения задачи использовать операции приведения с собственной функцией для решения задачи.
Результат – вектор размером n, каждый элемент которого получен по правилу определенной в задаче функции.
Найти произведение положительных.
*/

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <mpi.h>

int task3(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank; // Ранг текущего процесса
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int size; // Общее количество процессов
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const int n = 3; // Размер массива

    // Создаем массив случайных чисел в каждом процессе
    std::vector<int> local_array(n);
    std::srand(static_cast<unsigned int>(std::time(nullptr) + rank)); // Используем разные seed для разных процессов
    for (int i = 0; i < n; i++) {
        // Генерируем случайные числа от -5 до 4
        int num = std::rand() % 10 - 5;
        // Проверка на обнуление произведения
        if (num != 0) {
            local_array[i] = num;
        }
        else{
            local_array[i] = num + 1;
        }


    }

    // Выводим массив в каждом процессе
    std::cout << "Process " << rank << ": ";
    for (int i = 0; i < n; i++) {
        std::cout << local_array[i] << " ";
    }
    std::cout << std::endl;

    // Вычисляем произведение положительных чисел в локальных массивах
    int local_product = 1;
    for (int i = 0; i < n; i++) {
        if (local_array[i] > 0) {
            local_product *= local_array[i];
        }
    }

    // Собираем результаты со всех процессов
    int global_product;
    MPI_Reduce(&local_product, &global_product, 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

    // Выводим общее произведение только из процесса с рангом 0
    if (rank == 0) {
        std::cout << "Total product of positive numbers: " << global_product << std::endl;
    }

    MPI_Finalize();

    return 0;
}
/*
1. Заполняем локальный массив случайными числами.
2. Вычисляем произведение положительных чисел в локальных массивах.
3. С помощью функции MPI_Reduce собираем результаты вычислений со всех процессов в переменной global_product.
4. Процесс с рангом 0 выводит общее произведение положительных чисел.
*/