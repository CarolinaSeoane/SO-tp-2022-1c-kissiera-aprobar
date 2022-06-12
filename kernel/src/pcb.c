#include "../include/pcb.h"

int ULT_PID=-1;

int generar_pid() {
    pthread_mutex_lock(&mutex_PID);
    ULT_PID++;
    int ult = ULT_PID;
    pthread_mutex_unlock(&mutex_PID);
    return ult;
}

void crear_pcb(PCB* pcb, int tam, void* stream, int len_instrucciones, int cliente) {
    (*pcb).pid = generar_pid();
    (*pcb).tamanio_proceso = tam;
    (*pcb).len_instrucciones = len_instrucciones;
    (*pcb).stream = malloc(len_instrucciones*sizeof(instruccion));
    (*pcb).stream = stream;
    (*pcb).program_counter = 0;
    (*pcb).tabla_paginas = 0; // hay que pedirlo a memoria
    (*pcb).estimacion_rafaga = config.ESTIMACION_INICIAL; // si es fifo el planificador no lo va a usar
    (*pcb).cliente_fd = cliente;
    (*pcb).ult_rafaga_real_CPU = 0;
    (*pcb).timestamp_blocked = 0;
    (*pcb).timestamp_exec = 0;
    sem_init(&(pcb->puedo_finalizar), 0, 0);
}