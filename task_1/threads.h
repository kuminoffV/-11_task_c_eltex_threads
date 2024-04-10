#ifndef THREADS_H
#define THREADS_H

#include <pthread.h>

// Объявление функции, которую будут выполнять потоки
void *print_index(void *thread_id);

#endif /* THREADS_H */

