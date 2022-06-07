#include "../include/manejo_memoria.h"

void asignar_memoria_y_estructuras(int pid, int tamanio_proceso, uint32_t* direc_tabla_primer_nivel) {

    // Despues delegar a funciones para hacer más prolijo
    uint32_t cant_marcos = (uint32_t) ceil((double)tamanio_proceso / config.TAM_PAGINA);
    uint32_t cant_tablas_segundo_nivel = (uint32_t) ceil((double)cant_marcos / config.ENTRADAS_POR_TABLA);
    uint32_t cant_tablas_primer_nivel = (uint32_t) ceil((double) cant_tablas_segundo_nivel / config.ENTRADAS_POR_TABLA);
    

    for(uint32_t i=0; i<cant_tablas_primer_nivel; i++) {

        void* tabla_primer_nivel = malloc(sizeof(uint32_t) + sizeof(uint32_t)*config.ENTRADAS_POR_TABLA);

		// Acá se debería empezar a conectar las entradas con las tablas de segundo nivel
		memset(tabla_primer_nivel, (uint32_t)pid, sizeof(uint32_t));
		uint32_t offset = sizeof(uint32_t);
        memset(tabla_primer_nivel + offset, 0, sizeof(uint32_t)*config.ENTRADAS_POR_TABLA);

        pthread_mutex_lock(&mutex_lista_primer_nivel);
        if(i==0) {
            *direc_tabla_primer_nivel = list_add(lista_tablas_primer_nivel, &tabla_primer_nivel);
        } else {
            list_add(lista_tablas_primer_nivel, &tabla_primer_nivel);
        }
        pthread_mutex_unlock(&mutex_lista_primer_nivel);

		free(tabla_primer_nivel);
    }

    for(uint32_t i=0; i<cant_tablas_segundo_nivel; i++) {
        
		uint32_t tabla_segundo_nivel[config.ENTRADAS_POR_TABLA][4];

        for(uint32_t j=0; j<config.ENTRADAS_POR_TABLA; j++) {
			tabla_segundo_nivel[j][0] = 0; // Marco. No deberia ser 0
			tabla_segundo_nivel[j][1] = 0; // Bit de presencia
			tabla_segundo_nivel[j][2] = 0; // Bit de uso
			tabla_segundo_nivel[j][3] = 0; // Bit de modificado
		}

        pthread_mutex_lock(&mutex_lista_segundo_nivel);
        list_add(lista_tablas_segundo_nivel, &tabla_segundo_nivel);
        pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    }
    
    log_info(logger, "Asigno %d marcos", cant_marcos);
    log_info(logger, "Asigno %d tablas de primer nivel", cant_tablas_primer_nivel);
    log_info(logger, "Asigno %d tablas de segundo nivel\n\n", cant_tablas_segundo_nivel);

}