#include <iostream>
#include <cmath>
#include <vector>
#include <mpi.h>

double ln_sqrt_series(double x, int n) {
    double result = 0.0;
    double term = x - 1;
    for (int i = 1; i <= n; ++i) {
        result += term / i;
        term *= -(x - 1);
    }
    return result;
}

int task2(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size <= 1) {
        std::cerr << "This program requires at least 2 processes." << std::endl;
        MPI_Finalize();
        return 1;
    }

    double A, B, epsilon;
    int n;
    if (rank == 0) {
        // Процесс-мастер вводит A, B, epsilon и n
        std::cout << "Enter A, B, epsilon and n: ";
        std::cin >> A >> B >> epsilon >> n;
    }

    // Рассылка A, B, epsilon и n всем процессам
    MPI_Bcast(&A, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&B, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&epsilon, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Вычисление значений аргументов xi для каждого процесса
    int local_n = n / size;
    std::vector<double> x_values(local_n);
    double dx = (B - A) / n;
    for (int i = 0; i < local_n; ++i) {
        x_values[i] = A + rank * local_n * dx + i * dx;
    }

    // Вычисление значений функции в точках x_values с большим количеством членов ряда
    std::vector<double> results(local_n);
    int num_terms = n; // Увеличьте количество членов ряда для лучшей точности
    for (int i = 0; i < local_n; ++i) {
        results[i] = ln_sqrt_series(x_values[i], num_terms);
    }

    // Сбор результатов со всех процессов в процесс-мастер
    std::vector<double> all_results(n);
    MPI_Gather(&results[0], local_n, MPI_DOUBLE, &all_results[0], local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Вывод результатов в виде таблицы
    if (rank == 0) {
        std::cout << "point (xi) | func result" << std::endl;
        std::cout << "---------------------------" << std::endl;
        for (int i = 0; i < n; ++i) {
            double xi = A + i * dx;
            std::cout << xi << " | " << all_results[i] << std::endl;
        }
    }

    MPI_Finalize();
    return 0;
}
