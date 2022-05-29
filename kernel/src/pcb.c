#include "../include/pcb.h"

int generar_pid() {
    ULT_PID++;
    return ULT_PID;
}

void crear_pcb(PCB* pcb, int tam, void* stream, int len_instrucciones, int est_inicial, int cliente) {
    (*pcb).pid = generar_pid();
    (*pcb).tamanio_proceso = tam;
    (*pcb).len_instrucciones = len_instrucciones;
    (*pcb).stream = malloc(len_instrucciones*sizeof(instruccion));
    (*pcb).stream = stream;
    (*pcb).program_counter = 0;
    (*pcb).tabla_paginas = 0; // hay que pedirlo a memoria
    (*pcb).estimacion_rafaga = est_inicial; // si es fifo el planificador no lo va a usar
    (*pcb).cliente_fd = cliente;
}