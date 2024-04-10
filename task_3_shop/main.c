#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "shop.h"

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    initialize_shops(); // Инициализация магазинов
    output_initial_stock(); // Вывод начального запаса товаров

    pthread_t customers[NUM_CUSTOMERS]; // Потоки покупателей
    pthread_t loader_thread; // Поток погрузчика

    int customer_ids[NUM_CUSTOMERS];
    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        customer_ids[i] = i + 1;
        pthread_create(&customers[i], NULL, customer, &customer_ids[i]); // Создание потоков для покупателей
    }

    pthread_create(&loader_thread, NULL, loader, NULL); // Создание потока для погрузчика

    for (int i = 0; i < NUM_CUSTOMERS; ++i) {
        pthread_join(customers[i], NULL); // Ожидание завершения потоков покупателей
    }

    pthread_cancel(loader_thread); // Остановка потока погрузчика
    output_program_end(); // Вывод сообщения о завершении программы

    return 0;
}

