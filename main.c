#include <stdio.h>
#include <stdbool.h>
#include <malloc.h>
#include <pthread.h>
#include <stdlib.h>

typedef struct buffer_t {
    int index;
    int capacity;
    int *array;
    int pocetParnych;
    int pocetNeparnych;
} buffer_t;

void buffer_ini(buffer_t *buff, int capacity) {
    buff->array = malloc(sizeof(int) * capacity);
    buff->index = 0;
    buff->capacity = capacity;
    buff->pocetParnych = 0;
    buff->pocetNeparnych = 0;
}

void buffer_destroy(buffer_t *buff) {
    free(buff->array);
}

bool buffer_push(buffer_t *buf, int data) {
    if(buf->index < buf->capacity) {
        buf->array[buf->index++] = data;
        return true;
    }
    return false;
}

int buffer_pull(buffer_t *buf) {
    if(buf->capacity > 0) {
        return buf->array[--buf->index];
    }
    return -1;
}


typedef struct thread_data_t {
    pthread_mutex_t mutex;
    pthread_cond_t tinkyWinky;
    pthread_cond_t laLa;
    buffer_t buff;

} thread_data_t;

void thread_data_init(thread_data_t *data, int capacity) {
    pthread_mutex_init(&data->mutex, NULL);
    pthread_cond_init(&data->tinkyWinky, NULL);
    pthread_cond_init(&data->laLa, NULL);
    buffer_ini(&data->buff, capacity);
}

void thread_data_destroy(thread_data_t *data) {
    pthread_mutex_destroy(&data->mutex);
    pthread_cond_destroy(&data->tinkyWinky);
    pthread_cond_destroy(&data->laLa);
    buffer_destroy(&data->buff);
}

int generujCisla(int min, int max) {
    float cislo = rand() / (float) RAND_MAX;
    return min + cislo * (max - min);
}

void* tinkyWinky_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;

    for(int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&data_t->mutex);
        int cislo = generujCisla(1,1000);
        while(!buffer_push(&data_t->buff, cislo)) {
            pthread_cond_wait(&data_t->tinkyWinky, &data_t->mutex);
        }

        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->laLa);

    }

}

void vyhodnotCislo(buffer_t *buff,int cislo) {
    if(cislo % 2 == 0) {
        buff->pocetParnych++;
        printf("cislo je parne %d \n", cislo);
    } else {
        buff->pocetNeparnych++;
        printf("cislo je neparne %d \n", cislo);
    }
}

void* lala_fun(void* data) {
    thread_data_t* data_t = (thread_data_t*) data;
    for(int i = 0; i < 1000; i++) {
        pthread_mutex_lock(&data_t->mutex);

        while(data_t->buff.index == 0) {
            pthread_cond_wait(&data_t->laLa, &data_t->mutex);
        }

        int cislo = buffer_pull(&data_t->buff);
        vyhodnotCislo(&data_t->buff, cislo);
        pthread_mutex_unlock(&data_t->mutex);
        pthread_cond_signal(&data_t->tinkyWinky);

    }

}


int main() {
    thread_data_t data;
    thread_data_init(&data, 8);

    pthread_t tinkyWinky, laaLaa;
    pthread_create(&tinkyWinky, NULL, tinkyWinky_fun, &data);
    pthread_create(&laaLaa, NULL, lala_fun, &data);

    pthread_join(tinkyWinky, NULL);
    pthread_join(laaLaa, NULL);

    thread_data_destroy(&data);

    if (data.buff.pocetParnych >= 600) {
        printf("60 percent nahodne generovanych cisiel je parnych \n");
    } else {
        printf("60 percent nahodne generovanych cisiel nie je parnych \n");
    }
    return 0;
}

