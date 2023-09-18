#include "task1.cpp"
#include "task2.cpp"
#include "task3.cpp"
#include "task4.cpp"
#include "task5.cpp"


// Решения были разделены по файлам для удобства чтения кода
// Для запуска программ надо использовать mpiexec -n 4 projectMPI.exe
// После выбора каждой таски обязательно надо пересобрать проект!
int main(int argc, char *argv[]){
    task1(argc, argv);
//    task2(argc, argv);
//    task3(argc, argv);
//    task4(argc, argv);
//    task5(argc, argv);
    return 0;
}