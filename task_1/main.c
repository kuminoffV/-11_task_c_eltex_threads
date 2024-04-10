#include <stdio.h>
#include <pthread.h>
#include "threads.h"

#define NUM_THREADS 5

int main() {
    pthread_t threads[NUM_THREADS]; // Массив для хранения идентификаторов потоков
    int rc; // Переменная для хранения кода возврата функций
    long t; // Переменная для итерации по индексам потоков

    // Создание и запуск потоков
    for(t = 0; t < NUM_THREADS; t++) {
        printf("Создание потока %ld\n", t);
        // Вызов функции создания потока, передача функции print_index в качестве аргумента
        // и передача индекса потока в качестве аргумента функции print_index
        rc = pthread_create(&threads[t], NULL, print_index, (void *)t);
        if (rc) {
            // Если создание потока завершилось ошибкой, выводим сообщение об ошибке
            printf("Ошибка: Невозможно создать поток %ld\n", t);
            return -1;
        }
    }

    // Ожидание завершения всех созданных потоков
    for(t = 0; t < NUM_THREADS; t++) {
        pthread_join(threads[t], NULL);
    }

    pthread_exit(NULL); // Завершение главного потока
}

