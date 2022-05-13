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

    free(codigo);

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

    free(codigo);

}

void serializar_proceso_desalojado(Proceso_CPU* proceso, void* paquete_desalojado) {
    int offset = 0;
    
    int* codigo = malloc(sizeof(int));
	*codigo = DESALOJO_PROCESO;
	memcpy(paquete_desalojado, &(*codigo), sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_desalojado, &(*proceso).pid, sizeof(int));
    
    offset += sizeof(int);
    memcpy(paquete_desalojado, &(*proceso).program_counter, sizeof(int));

    free(codigo);
}

void send_proceso_bloqueado(Proceso_CPU* proceso, int tiempo_bloqueo, void* void_args) {
    args_dispatch* args = (args_dispatch*) void_args;

    void* paquete_bloqueo = malloc(sizeof(int)*4);
    serializar_proceso_bloqueado(proceso, tiempo_bloqueo, paquete_bloqueo);
    send(args->cliente_dispatch_fd, paquete_bloqueo, sizeof(int)*4, 0);
    free(paquete_bloqueo);
}

void send_proceso_finalizado(Proceso_CPU* proceso, void* void_args) {
    args_dispatch* args = (args_dispatch*) void_args;

    void* paquete_finalizado = malloc(sizeof(int)*3);
    serializar_proceso_finalizado(proceso, paquete_finalizado);
    send(args->cliente_dispatch_fd, paquete_finalizado, sizeof(int)*3, 0);
    free(paquete_finalizado);
}

void send_proceso_desalojado(Proceso_CPU* proceso, void* void_args) {
    args_dispatch* args = (args_dispatch*) void_args;

    void* paquete_desalojado = malloc(sizeof(int)*3);
    serializar_proceso_desalojado(proceso, paquete_desalojado);
    send(args->cliente_dispatch_fd, paquete_desalojado, sizeof(int)*3, 0);
    free(paquete_desalojado);
}

// Hay mucho código repetido, se podría mejorar