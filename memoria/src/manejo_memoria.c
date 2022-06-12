#include "../include/manejo_memoria.h"

uint32_t asignar_memoria_y_estructuras(int pid, int tamanio_proceso) {

    // Despues delegar a funciones para hacer más prolijo
    uint32_t cant_max_marcos = (uint32_t) ceil((double)config.MARCOS_POR_PROCESO); //A todos se les asigna esto independientemente de su tamaño
    uint32_t cant_marcos = (uint32_t) ceil((double)tamanio_proceso / config.TAM_PAGINA);
    uint32_t cant_tablas_segundo_nivel = (uint32_t) ceil((double)cant_marcos / config.ENTRADAS_POR_TABLA);
    uint32_t cant_entradas_tabla_primer_nivel = config.ENTRADAS_POR_TABLA;

    // Cada proceso tiene una tabla de primer nivel
    Tabla_Primer_Nivel* tabla_primer_nivel = malloc(sizeof(Tabla_Primer_Nivel));
    tabla_primer_nivel->pid = pid;
    tabla_primer_nivel->entradas_tabla_primer_nivel = list_create();

    for (int i = 0; i < cant_entradas_tabla_primer_nivel; i++) {

        Entrada_Tabla_Primer_Nivel* entrada_primer_nivel = malloc(sizeof(Entrada_Tabla_Primer_Nivel));
            
        // Por cada entrada, creo una tabla de segundo nivel que va a ser la asociada
        Tabla_Segundo_Nivel* tabla_segundo_nivel = malloc(sizeof(Tabla_Segundo_Nivel));
        tabla_segundo_nivel->entradas_tabla_segundo_nivel = list_create();

        // Cada tabla de segundo nivel va a tener la misma cantidad de entradas
        // aunque algunas no las use
        for(int k = 0; k < config.ENTRADAS_POR_TABLA; k++) {
                           
            // Esto despues se llenaria cuando se carga la pagina en el espacio de usuario
            Entrada_Tabla_Segundo_Nivel* entrada = malloc(sizeof(Entrada_Tabla_Segundo_Nivel));
            entrada->marco = 0;
            entrada->bit_presencia = 0;
            entrada->bit_modificado = 0;
            entrada->bit_uso = 0;
            list_add(tabla_segundo_nivel->entradas_tabla_segundo_nivel, entrada);
        } 
            
        // La tabla se 2do nivel esta completa, la agrego a la lista
		pthread_mutex_lock(&mutex_lista_segundo_nivel);
        entrada_primer_nivel->index_tabla_segundo_nivel = list_add(lista_tablas_segundo_nivel, tabla_segundo_nivel);
        pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    
        // Con la direccion que me dio, creo la entrada de 1er nivel y la agrego a la tabla
        list_add(tabla_primer_nivel->entradas_tabla_primer_nivel, entrada_primer_nivel);
    
    }
    
    // La tabla de primer nivel esta llena, la agrego a la lista
    pthread_mutex_lock(&mutex_lista_primer_nivel);
    list_add_in_index(lista_tablas_primer_nivel, pid, tabla_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    // Estos valores son los reales que necesita el proceso. 
    // Pero la tabla se llena con todas las entradas aunque no las use
    log_info(logger, "Asigno %d marcos", cant_marcos);
    log_info(logger, "Asigno %d entradas en la tabla de primer nivel", cant_entradas_tabla_primer_nivel);
    log_info(logger, "Asigno %d tablas de segundo nivel\n\n", cant_tablas_segundo_nivel);

    verificar_memoria();

    // Pedir INIT a swap
    pedido_swap *pedido = malloc(sizeof(pedido_swap));
	pedido->co_op = INIT_PROCESO;
    pedido->pid = pid;
    pedido->tamanio_proceso = tamanio_proceso;

	pthread_mutex_lock(&mutexColaSwap);
	list_add(cola_pedidos_a_swap, pedido);
	pthread_mutex_unlock(&mutexColaSwap);
    sem_post(&realizar_op_de_swap);

    return pid;

}

void verificar_memoria() {

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    pthread_mutex_lock(&mutex_lista_segundo_nivel);

    log_info(logger, "Hay %d tablas de primer nivel", list_size(lista_tablas_primer_nivel));
    log_info(logger, "Hay %d tablas de segundo nivel\n\n", list_size(lista_tablas_segundo_nivel));

    mostrar_lista_tablas_primer_nivel();

    pthread_mutex_unlock(&mutex_lista_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);

}


void mostrar_lista_tablas_primer_nivel() {

    log_info(logger, "LISTA DE TABLAS DE PRIMER NIVEL");

    t_list_iterator* iterator = list_iterator_create(lista_tablas_primer_nivel);
    Tabla_Primer_Nivel* elem_iterado;

    while(list_iterator_has_next(iterator)) {
        elem_iterado = list_iterator_next(iterator);
        log_info(logger, "PID %d", elem_iterado->pid);

        t_list_iterator* iterator2 = list_iterator_create(elem_iterado->entradas_tabla_primer_nivel);
        Entrada_Tabla_Primer_Nivel* elem_iterado_2;
        
        while(list_iterator_has_next(iterator2)) {
            elem_iterado_2 = list_iterator_next(iterator2);
            Tabla_Segundo_Nivel* tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, elem_iterado_2->index_tabla_segundo_nivel);
            log_info(logger, "\tIndex en lista 2do nivel: %d", elem_iterado_2->index_tabla_segundo_nivel);
            
            t_list_iterator* iterator3 = list_iterator_create(tabla_segundo_nivel->entradas_tabla_segundo_nivel);
            Entrada_Tabla_Segundo_Nivel* elem_iterado_3;
            
            while(list_iterator_has_next(iterator3)) {
                elem_iterado_3 = list_iterator_next(iterator3);
                log_info(logger, "\tM %d | P %d | M %d | U %d", elem_iterado_3->marco, elem_iterado_3->bit_presencia, elem_iterado_3->bit_modificado, elem_iterado_3->bit_uso);
            }
            
        }
        
        list_iterator_destroy(iterator2);
        printf("\n"); // Horrible despues sacar

    }

    list_iterator_destroy(iterator);

}