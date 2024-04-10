#include "shop.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t shop_cond = PTHREAD_COND_INITIALIZER;
Shop shops[NUM_SHOPS];

void print_customer_info(int id, int need, int shop_id, int purchase, int remaining_need) {
    printf("Покупатель %d: Потребность перед походом в магазин: %d, Магазин, в который он зашел: %d, Сумма покупки: %d, Оставшаяся потребность: %d\n",
           id, need, shop_id, purchase, remaining_need);
    printf("Покупатель %d: Засыпает на 2 секунды...\n", id);
    if (remaining_need <= 0) {
        printf("Покупатель %d: Насыщен! Больше не ходит по магазинам.\n", id);
    }
}

void print_loader_info(int shop_id, int current_stock, int products_added) {
    printf("------------------------------------\n");
    printf("Погрузчик: Магазин, в который он зашел: %d, Текущее количество товаров: %d, Количество товаров, которые он положил: %d\n",
           shop_id, current_stock, products_added);
    printf("------------------------------------\n");
}

int random_number(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void initialize_shops() {
    for (int i = 0; i < NUM_SHOPS; ++i) {
        shops[i].id = i;
        shops[i].stock = random_number(MIN_PRODUCTS, MAX_PRODUCTS);
    }
}

void print_initial_stock() {
    printf("Информация о заполненности магазинов в начале программы:\n");
    for (int i = 0; i < NUM_SHOPS; ++i) {
        printf("Магазин %d: Товаров на складе: %d\n", i, shops[i].stock);
    }
}

void print_program_end() {
    printf("Программа завершена.\n");
}

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
            print_customer_info(id, need + purchase, shop_id, purchase, need);
            sleep(2);
        } else {
            pthread_cond_wait(&shop_cond, &mutex);
            pthread_mutex_unlock(&mutex);
        }
    }
    return NULL;
}

void* loader(void* arg) {
    while (1) {
        for (int i = 0; i < NUM_SHOPS; ++i) {
            pthread_mutex_lock(&mutex);
            int products_added = random_number(0, MAX_STOCK);
            shops[i].stock += products_added;
            print_loader_info(i, shops[i].stock, products_added);
            pthread_cond_broadcast(&shop_cond);
            pthread_mutex_unlock(&mutex);
            sleep(1);
        }
    }
    return NULL;
}

