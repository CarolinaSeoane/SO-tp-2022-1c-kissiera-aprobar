#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
/*
#include "../../shared/include/shared.h"
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <commons/collections/list.h>
#include <commons/collections/queue.h>

pthread_t planificador_trip
;
sem_t sem_planificador_trip;


pthread_t hilo_io;
pthread_t hilo_sabotaje;

pthread_mutex_t mx_ready;
pthread_mutex_t mx_io;
pthread_mutex_t mx_sabotaje;
pthread_mutex_t mx_exit;
pthread_mutex_t mx_ciclo;



t_list *cola_new;
t_list *cola_ready;
t_list *cola_exit;
t_list *cola_sabotaje;
t_list *cola_io;



int cantTripulantes; // grado de multiprogramacion
char* algoritmo_planificador;
int quantum;
int ciclo_de_cpu;

typedef enum{
    NEW, LISTO, TRABAJANDO,IO, EMERGENCIA, EXIT
} t_estado;


typedef struct {
    char* nombre_tarea_y_parametro; // nombre y parametro se separan con un espacio
    char** posicion_X;
    char** cosicion_Y;
    int duracion;
} t_tarea;


typedef struct {
    int id_patota;
    int ciclo_actual;
    t_estado estado;
    int prioridad; // 0 NO USO TODO EL Q,  1 EXEC-> READY, 1 BLOQ->READY,, 3 NEW->READY (default)
    int rafaga;
    int pc; //program counter
    int espera_tripulante;
    int cpu_llegada;
    t_tarea* tarea;

} t_tcb;


typedef struct {
    int id;
    int cantidad_tripulantes;
    t_list* tripulantes;
    sem_t sem_tripulante_finalizado;
    pthread_t hilo_patota;

} t_patota;


typedef struct {
    int id;
    char path_tarea;
    char** posicionX;
    char** posicionY;
    t_tcb tcb;

} t_tripulante;


void mandar_tripulante_a_ready(t_tripulante);
void mandar_tripulante_a_io(t_tripulante tripulante);
void mandar_tripulante_a_exit(t_tripulante tripulante);
void ejecutar_tarea_fifo(t_tripulante);

*/
#endif /* PLANIFICADOR_H_ */
