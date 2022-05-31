#include "../include/serializacion.h"

void recv_proceso_init(int *pid, int *tamanio_proceso, int cliente) {
	recv(cliente, pid, sizeof(int), 0);
	recv(cliente, tamanio_proceso, sizeof(int), 0);
}

void send_cpu_handshake(void* void_args) {

	args_thread_memoria* args = (args_thread_memoria*) void_args;

	int offset = 0;
	void* a_enviar = malloc(sizeof(int)*3);

	// Se envia este codigo pero CPU no lo usa realmente por ahora
	int* codigo = malloc(sizeof(int));
	*codigo = HANDSHAKE_MEMORIA;
	memcpy(a_enviar, &(*codigo), sizeof(int));
	offset += sizeof(int);

	memcpy(a_enviar+offset, &config.TAM_PAGINA, sizeof(int));
	offset += sizeof(int);

	memcpy(a_enviar+offset, &config.ENTRADAS_POR_TABLA, sizeof(int));

	send(args->cliente_fd, a_enviar, sizeof(int)*3, 0);
	free(a_enviar);
	free(codigo);
}

void send_tabla_primer_nivel(int conexion, uint32_t tabla_primer_nivel) {
	void* paquete = malloc(sizeof(uint32_t));
	memcpy(paquete, &tabla_primer_nivel, sizeof(uint32_t));
	send(conexion, paquete, sizeof(uint32_t), 0);
	free(paquete);
}

void asignar_memoria_y_estructuras(int pid, int tamanio_proceso, uint32_t* direc_tabla_primer_nivel) {

    // Despues delegar a funciones para hacer más prolijo
    uint32_t cant_marcos = (uint32_t) ceil((double)tamanio_proceso / config.TAM_PAGINA);
    log_info(logger, "El proceso %d necesita %d marcos", pid, cant_marcos);
    uint32_t cant_tablas_segundo_nivel = (uint32_t) ceil((double)cant_marcos / config.ENTRADAS_POR_TABLA);
    log_info(logger, "El proceso %d necesita %d tablas de segundo nivel", pid, cant_tablas_segundo_nivel);
    uint32_t cant_tablas_primer_nivel = (uint32_t) ceil((double) cant_tablas_segundo_nivel / config.ENTRADAS_POR_TABLA);
    log_info(logger, "El proceso %d necesita %d tablas de primer nivel", pid, cant_tablas_primer_nivel);

    for(uint32_t i=0; i<cant_tablas_primer_nivel; i++) {

        void* entrada_tabla_primer_nivel = malloc(sizeof(uint32_t) + sizeof(uint32_t)*config.ENTRADAS_POR_TABLA);

		// Acá se debería empezar a conectar las entradas con las tablas de segundo nivel
        memset(entrada_tabla_primer_nivel, 0, sizeof(uint32_t) + sizeof(uint32_t)*config.ENTRADAS_POR_TABLA);

        pthread_mutex_lock(&mutex_lista_primer_nivel);
        if(i==0) {
            *direc_tabla_primer_nivel = list_add(lista_tablas_primer_nivel, &entrada_tabla_primer_nivel);
        } else {
            list_add(lista_tablas_primer_nivel, &entrada_tabla_primer_nivel);
        }
        pthread_mutex_unlock(&mutex_lista_primer_nivel);

		free(entrada_tabla_primer_nivel);
    }

    for(uint32_t i=0; i<cant_tablas_segundo_nivel; i++) {
        
		uint32_t entrada_tabla_segundo_nivel[config.ENTRADAS_POR_TABLA][4];

        for(uint32_t j=0; j<config.ENTRADAS_POR_TABLA; j++) {
			entrada_tabla_segundo_nivel[j][0] = 0; // Marco. No deberia ser 0
			entrada_tabla_segundo_nivel[j][1] = 0; // Bit de presencia
			entrada_tabla_segundo_nivel[j][2] = 0; // Bit de uso
			entrada_tabla_segundo_nivel[j][3] = 0; // Bit de modificado
		}

        pthread_mutex_lock(&mutex_lista_segundo_nivel);
        list_add(lista_tablas_segundo_nivel, &entrada_tabla_segundo_nivel);
        pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    }
    
    log_info(logger, "Se asignaron %d tablas de primer nivel y %d tablas de segundo nivel al proceso %d", cant_tablas_primer_nivel, cant_tablas_segundo_nivel, pid); 

}