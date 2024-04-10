#include "shop.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t shop_cond = PTHREAD_COND_INITIALIZER;
Shop shops[NUM_SHOPS];

// Функция для вывода информации о покупателе
void output_customer_info(int id, int need, int shop_id, int purchase, int remaining_need) {
    printf("ПОКУПАТЕЛЬ %d: Потребность ДО: %d | Магазин: %d | Сумма покупки: %d | Потребность ПОСЛЕ: %d | Отдых 2 секунды\n\n",
           id, need, shop_id, purchase, remaining_need);
    //printf("Покупатель %d: Засыпает на 2 секунды...\n\n", id);
    if (remaining_need <= 0) {
        printf("Покупатель %d: Насыщен! Больше не ходит по магазинам.\n\n", id);
    }
}

// Функция для вывода информации о погрузчике
void output_loader_info(int shop_id, int current_stock, int products_added) {
    printf("------------------------------------\n");
    printf("ДОСТАВКА: Магазин: %d | Текущее количество товаров: %d | Количество товаров, которые доставлены: %d | Отдых 1 секунда\n", 
           shop_id, current_stock, products_added);
    printf("------------------------------------\n\n");
}

// Генерация случайного числа в заданном диапазоне
int random_number(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Инициализация магазинов
void initialize_shops() {
    for (int i = 0; i < NUM_SHOPS; ++i) {
        shops[i].id = i;
        shops[i].stock = random_number(MIN_PRODUCTS, MAX_PRODUCTS);
    }
}

// Вывод начального запаса товаров в магазинах
void output_initial_stock() {
    printf("\nЗаполненность магазинов в начале дня:\n");
    for (int i = 0; i < NUM_SHOPS; ++i) {
        printf("Магазин %d: Товаров на складе: %d\n\n", i, shops[i].stock);
    }
}

// Вывод сообщения о завершении программы
void output_program_end() {
    printf("КОНЕЦ\n");
}

// Функция, представляющая поведение покупателя
void* customer(void* arg) {
    int id = *((int*) arg);
    int need = MAX_NEED;
    while (need > 0) {
        int shop_id = -1;
        pthread_mutex_lock(&mutex);
        for (int i = 0; i < NUM_SHOPS; ++i) {
            if (shops[i].stock > 0) {
                shop_id = i;
                break;
            }
        }

        if (shop_id != -1) {
            int purchase = shops[shop_id].stock < need ? shops[shop_id].stock : need;
            shops[shop_id].stock -= purchase;
            need -= purchase;
            pthread_mutex_unlock(&mutex);
            output_customer_info(id, need + purchase, shop_id, purchase, need);
            sleep(2);
        } else {
            pthread_cond_wait(&shop_cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

// Функция, представляющая поведение погрузчика
void* loader(void* arg) {
    while (1) {
        for (int i = 0; i < NUM_SHOPS; ++i) {
            pthread_mutex_lock(&mutex);
            int products_added = random_number(0, MAX_STOCK);
            shops[i].stock += products_added;
            output_loader_info(i, shops[i].stock, products_added);
            pthread_cond_broadcast(&shop_cond);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    return NULL;
}

