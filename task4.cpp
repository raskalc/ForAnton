/*
Создать описатель типа и использовать его при передаче данных в качестве шаблона для следующего преобразования:
*/

#include <mpi.h>
#include <iostream>
#include <random>
#include <iomanip>

using namespace std;

const int rows = 10;
const int cols = 10;

void printArrayToConsole(int array[][cols], int rows) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << setw(4) << array[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

int task4(int argc, char **argv) {

    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int array[rows][cols]{};

    if (rank == 0) {
        // границы, в пределах которых генерируются рандомные числа
        const int min = 10, max = 100;

        random_device r;
        default_random_engine e(r());
        uniform_int_distribution<int> dist(min, max);

        // заполнение матрицы
        for (auto & i : array) {
            for (int & j : i) {
                j = dist(e);
            }
        }

        printArrayToConsole(array, rows);
    }

    MPI_Aint lowerBound;
    MPI_Aint sizeOfInt;
    MPI_Type_get_extent(MPI_INT, &lowerBound, &sizeOfInt);

    int blocklengths[rows]{};
    for (int i = 0; i < rows; i++) {
        blocklengths[i] = i + 1; // Изменяем blocklengths
    }

    int displacements[rows]{};
    for (int i = 0; i < rows; i++) {
        displacements[i] = i * cols; // Изменяем displacements
    }

    MPI_Datatype MyDataType;
    MPI_Type_indexed(rows, blocklengths, displacements, MPI_INT, &MyDataType);
    MPI_Type_commit(&MyDataType);

    if (rank == 0) {
        MPI_Send(array, 1, MyDataType, 1, 0, MPI_COMM_WORLD);
    } else if (rank == 1) {
        MPI_Recv(array, 1, MyDataType, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printArrayToConsole(array, rows);
    }

    MPI_Type_free(&MyDataType);

    MPI_Finalize();
    return 0;
}


/*
Определим размеры матрицы rows (количество строк) и cols (количество столбцов),
которая будет заполнена случайными числами и передана между процессами.
printArrayToConsole - функция для вывода двумерного массива в консоль.
Она используется для вывода содержимого матрицы на экран.
Инициализируется среда выполнения MPI с помощью MPI_Init.
Получается номер процесса текущего исполнения с помощью MPI_Comm_rank.
Каждый процесс имеет свой уникальный номер, который определяет его роль в выполнении программы.
Создается двумерный массив array, который будет использоваться для хранения данных.
В данном случае, процесс с рангом 0 (главный процесс) генерирует случайные числа и заполняет этот массив.
Затем определяются параметры пользовательского типа данных MPI с именем MyDataType.
Этот тип данных используется для описания распределения данных в массиве между процессами.
blocklengths и displacements определяют, какие элементы из массива будут отправлены каждому процессу
и где они расположеныв исходном массиве.
Если процесс имеет ранг 0, то он отправляет данные, используя MPI_Send,
а если процесс имеет ранг 1, то он принимает данные с помощью MPI_Recv.
Это обмен данными между процессами с использованием определенного пользовательского типа данных.
После завершения обмена данными, тип данных MyDataType освобождается с помощью MPI_Type_free.
Завершается среда выполнения MPI с помощью MPI_Finalize.
*/