#include "../include/conexiones_memoria.h"

// Alcanza con mandar 4 int: CODIGO_OP + PID + PC + TIEMPO_BLOQUEO 
void serializar_proceso_bloqueado(Proceso_CPU* proceso, int tiempo_bloqueo, void* paquete_bloqueo) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = BLOCK_PROCESO;
	memcpy(paquete_bloqueo, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo, &(*proceso).pid, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo, &(*proceso).program_counter, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo, &tiempo_bloqueo, sizeof(int));

}

// Alcanza con mandar 3 int: CODIGO_OP + PID + PC
void serializar_proceso_finalizado(Proceso_CPU* proceso, void* paquete_bloqueo) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = EXIT_PROCESO;
	memcpy(paquete_bloqueo, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo, &(*proceso).pid, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_bloqueo, &(*proceso).program_counter, sizeof(int));

}

void send_proceso_bloqueado(Proceso_CPU* proceso, int tiempo_bloqueo, void* void_args) {
    args_dispatch* args = (args_dispatch*) void_args;

    void* paquete_bloqueo = malloc(sizeof(int)*4);
    serializar_proceso_bloqueado(proceso, tiempo_bloqueo, paquete_bloqueo);
    send(args->cliente_dispatch_fd, paquete_bloqueo, sizeof(int)*4, 0);
}

void send_proceso_finalizado(Proceso_CPU* proceso, void* void_args) {
    args_dispatch* args = (args_dispatch*) void_args;

    void* paquete_finalizado = malloc(sizeof(int)*3);
    serializar_proceso_finalizado(proceso, paquete_finalizado);
    send(args->cliente_dispatch_fd, paquete_finalizado, sizeof(int)*3, 0);
}