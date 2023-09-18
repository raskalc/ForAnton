/*
Требуется вычислить значения суммы ряда в n точках заданного интервала [A,B] с точностью ε.
Процесс-мастер вводит с клавиатуры A, B, ε и n, вычисляет аргументы x1=A, x2, …,xn=B и рассылает каждому процессу
n/k (k – количество запущенных процессов) значений, используя функцию MPI_Scatter и значение ε с помощью функции MPI_Bcast.
Каждый процесс вычисляет значения функции в полученных точках и отправляет процессу- мастеру с помощью функции MPI_Gather.
Процесс - мастер выводит полученные результаты и точные значения функции в соответствующих точках в виде таблицы.
*/


#include "mpi.h"
#include <iostream>
#include <vector>
#include <limits>
#include <math.h>
#include <iomanip>

using namespace std;

template<typename T>
void readNumFromConsole(const string text, T &num) {
    while (true) {
        cout << text;
        cin >> num;
        if (cin) {
            return;
        }
        cin.clear();
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cout << "ERROR! retype please" << endl;
    }
}

double calculateSumOfSeriesAtSpecificPointWithEpsPrecision(double &point, double &eps) {
    double cur = point;
    double result = 0;
    int i = 1;
    double squaredPoint = pow(point, 2);
    int denominator = 1;

    while (abs(cur) > eps) {
        result += cur;
        cur = -cur * squaredPoint * i / (i + 1) * i / (i + 2);
        i += 2;
    }

    return result;
}

int task2(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    int n; // кол-во точек интервала [A, B]
    int count; // кол-во рассылаемых значений
    double eps; // точность
    double a, b; // интервал [A, B]
    vector<double> x; // аргументы, которые вычисляются корневым процессом (значения n точек между A и B)
    vector<double> subX; // аргументы, которые каждый процесс получает после применения MPI_Scatter (у каждого свои)
    vector<double> subResults; // результаты вычисления каждым процессом значений ряда в точках subX (у каждого свои)
    vector<double> results; // общие результаты, которые получает корневой процесс после применения MPI_Gather
    vector<double> exactValues; // точные значения в точках

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        do {
            do {
                readNumFromConsole("Enter A - begin interval: ", a);
                if (a < -1 || a > 1) {
                    cout << "Input error. Accepted from -1 to 1" << endl;
                }
            } while (a < -1 || a > 1);

            do {
                readNumFromConsole("Enter B - end interval: ", b);
                if (b < -1 || b > 1) {
                    cout << "Input error. Accepted from -1 to 1" << endl;
                }
            } while (b < -1 || b > 1);

            if (a >= b) {
                cout << "Input error. A should be < B" << endl;
            }
        } while (a >= b);

        do {
            readNumFromConsole("Enter eps: ", eps);
            if (eps <= 0 || eps >= 1) {
                cout << "Input error. Eps should be in (0, 1)" << endl;
            }
        } while (eps <= 0 || eps >= 1);

        do {
            readNumFromConsole("Enter N - number of points in interval: ", n);
            if (n % size != 0) {
                cout << "Input error. N must be divisible by the number of processes: " << size << endl;
            }
        } while (n % size != 0);

        x.push_back(a);
        double inc = (b - a) / n;
        for (int i = 1; i < n - 1; i++) {
            x.push_back(a + inc * i);
        }
        x.push_back(b);
    }

    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    count = n / size;

    subX.resize(count);
    MPI_Scatter(x.data(), count, MPI_DOUBLE, subX.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    for (double point: subX) {
        subResults.push_back(calculateSumOfSeriesAtSpecificPointWithEpsPrecision(point, eps));
    }

    results.resize(n);
    MPI_Gather(subResults.data(), count, MPI_DOUBLE, results.data(), count, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (double point: x) {
            exactValues.push_back(log(point + sqrt(point * point + 1)));
        }

        cout << setw(25) << "Point" << setw(25) << "Row sum" << setw(25) << "Exact value" << endl;
        for (int i = 0; i < n; i++) {
            cout << setw(25) << x[i] << setw(25) << results[i] << setw(25) << exactValues[i] << endl;
        }
    }

    MPI_Finalize();
    return 0;
}

/*
функция readNumFromConsole, которая используется для безопасного считывания чисел с консоли, обрабатывая ошибки ввода.
функция calculateSumOfSeriesAtSpecificPointWithEpsPrecision, которая вычисляет сумму ряда в заданной точке с заданной точностью (по эпсилону).
Если ранг процесса равен 0, то это считается корневым процессом, и пользователь вводит параметры интервала [A, B], точность (eps) и количество точек (n) на этом интервале.
Корневой процесс генерирует значения точек x равномерно распределенные в интервале [A, B] и передает их всем процессам с помощью MPI_Scatter.
Каждый процесс вычисляет сумму ряда в своих точках (subX) с заданной точностью (eps) с использованием функции calculateSumOfSeriesAtSpecificPointWithEpsPrecision.
Результаты вычислений каждого процесса сохраняются в массив subResults.
Затем результаты всех процессов собираются в корневой процесс с помощью MPI_Gather и сохраняются в массив results.
Если ранг процесса равен 0, то корневой процесс вычисляет точные значения функции в точках x и выводит таблицу сравнения точных значений, результатов ряда и соответствующих точек на экран.
*/
