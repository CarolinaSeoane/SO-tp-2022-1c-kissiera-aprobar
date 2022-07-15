#include "../include/serializacion.h"

void send_proceso_a_cpu(PCB* pcb, int size_stream) {
    int bytes_a_enviar = (sizeof(int) * 5) + size_stream;
    void* a_enviar = serializar_proceso(pcb, bytes_a_enviar);
    send(conexion_dispatch, a_enviar, bytes_a_enviar, 0);
    free(a_enviar);
}

void send_desalojo_a_cpu(PCB* pcb, int size_stream) {
    int bytes_a_enviar = sizeof(int);
    int* codigo = malloc(sizeof(int));
    *codigo = INTERRUPCION;         
    send(conexion_interrupt, codigo, bytes_a_enviar, 0);
    free(codigo);
}

void* serializar_proceso(PCB* pcb, int bytes) {
    void* paquete = malloc(bytes);
    
    int* codigo = malloc(sizeof(int));
    *codigo = EXEC_PROCESO;

    int tam_stream = bytes - (sizeof(int) * 5);

    int offset = 0;
    memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->pid), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->program_counter), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pcb->tabla_paginas), sizeof(int));
	offset += sizeof(int);
    memcpy(paquete + offset, &(tam_stream), sizeof(int));
    offset += sizeof(int);
    memcpy(paquete + offset, pcb->stream, tam_stream);

    free(codigo);

    return paquete;
}

void send_proceso_finalizado_a_consola(int pid, int consola) {
    int bytes_a_enviar = sizeof(int) * 2;
	void* a_enviar = malloc(bytes_a_enviar);
    int codigo = 1;
    int id = pid;
	int offset = 0;

	memcpy(a_enviar, &codigo, sizeof(int));
    offset += sizeof(int);
	memcpy(a_enviar + offset, &id, sizeof(int));
	
	log_info(logger, "Enviando finalizacion a consola. Pid: %d y codigo: %d", id, codigo);
	send(consola, a_enviar, bytes_a_enviar, 0);
    free(a_enviar);	
}

int solicitar_tabla_de_paginas_a_memoria(PCB* proceso) {   
    int bytes_a_enviar = sizeof(int) * 3; //PID, OPERACION, TAM PROCESO
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = INIT_PROCESO;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(proceso->pid), sizeof(int));
	offset += sizeof(int);
    memcpy(a_enviar + offset, &(proceso->tamanio_proceso), sizeof(int));

    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);
    free(codigo);
    free(a_enviar);
    
    int tabla_primer_nivel;
    recv(conexion_memoria, &tabla_primer_nivel, sizeof(int), 0);
    return tabla_primer_nivel;
}

void solicitar_swap_out_a_memoria(PCB* proceso) {
    int bytes_a_enviar = sizeof(int) * 2; // OPERACION, PID
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = SWAP_OUT;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(proceso->pid), sizeof(int));

    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);
    free(codigo);
    free(a_enviar);
    
    int confirmacion;
    recv(conexion_memoria, &confirmacion, sizeof(int), 0);
    log_info(logger, "El proceso %d fue swapeado", proceso->pid);
    sem_post(&(proceso->termino_operacion_swap_out));
}

void solicitar_swap_in_a_memoria(PCB* proceso) {
    int bytes_a_enviar = sizeof(int) * 2; // OPERACION, PID
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = SWAP_IN;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(proceso->pid), sizeof(int));
	
    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);
    free(codigo);
    free(a_enviar);
}

void recv_proceso_bloqueado(int* pid, int* pc, int* tiempo_bloqueo) {
    recv(conexion_dispatch, pid, sizeof(int), 0);
    recv(conexion_dispatch, pc, sizeof(int), 0);
    recv(conexion_dispatch, tiempo_bloqueo, sizeof(int), 0);
}

void recv_proceso_cpu(int* pid_a_finalizar, int* program_counter) {
    recv(conexion_dispatch, pid_a_finalizar, sizeof(int), 0);
    recv(conexion_dispatch, program_counter, sizeof(int), 0);
}

void pedir_finalizar_estructuras_y_esperar_confirmacion(int pid){
    int bytes_a_enviar = sizeof(int) * 2; //OPERACION, PID
	void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = EXIT_PROCESO_M;
    int offset = 0;

    memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(a_enviar + offset, &(pid), sizeof(int));

    send(conexion_memoria, a_enviar, bytes_a_enviar, 0);

    int proceso_finalizado;
	recv(conexion_memoria, &proceso_finalizado, sizeof(int), MSG_WAITALL);
    log_info(logger,"Memoria me avisa que liberó las estructuras del proceso %d", pid);
    free(codigo);
    free(a_enviar);
}
