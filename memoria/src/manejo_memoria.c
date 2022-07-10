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
            entrada->marco = -1;
            entrada->bit_presencia = 0;
            entrada->bit_modificado = 0;
            entrada->bit_uso = 0;
            entrada->bit_puntero = 0;
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
    sem_init(&pedido->pedido_finalizado, 0, 0);

	pthread_mutex_lock(&mutexColaSwap);
	list_add(cola_pedidos_a_swap, pedido);
	pthread_mutex_unlock(&mutexColaSwap);
    sem_post(&realizar_op_de_swap);

    sem_wait(&pedido->pedido_finalizado);
    return pid;
}

void verificar_memoria() {

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    pthread_mutex_lock(&mutex_lista_segundo_nivel);

    //log_info(logger, "Hay %d tablas de primer nivel", list_size(lista_tablas_primer_nivel));
    //log_info(logger, "Hay %d tablas de segundo nivel\n\n", list_size(lista_tablas_segundo_nivel));

    mostrar_lista_tablas_primer_nivel();

    pthread_mutex_unlock(&mutex_lista_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);

}

void mostrar_bitmap() {
    int cant_frames = config.TAM_MEMORIA / config.TAM_PAGINA;

    log_info(logger_bitmap, "BITMAP");
    for(int i = 0; i < cant_frames; i++) {
        log_info(logger_bitmap, "|#F %-3d| %d|", i, bitarray_test_bit(marcos_libres, i));
    }
    
}

void mostrar_lista_tablas_primer_nivel() {

    log_info(logger, "-- ESTADO TABLAS DE PAGINAS --");

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
            printf("\n");
            log_info(logger, "\tIndex en lista 2do nivel: %d", elem_iterado_2->index_tabla_segundo_nivel);
            
            t_list_iterator* iterator3 = list_iterator_create(tabla_segundo_nivel->entradas_tabla_segundo_nivel);
            Entrada_Tabla_Segundo_Nivel* elem_iterado_3;
            
            log_info(logger, "\t_______________________");
            log_info(logger, "\t|__F__|_P_|_M_|_U_|_*_|");
            while(list_iterator_has_next(iterator3)) {
                elem_iterado_3 = list_iterator_next(iterator3);
                if(elem_iterado_3->marco == -1) {
                    log_info(logger, "\t|  -  | - | - | - | - |");
                } else {
                    log_info(logger, "\t| %-3d | %d | %d | %d | %d |", elem_iterado_3->marco, elem_iterado_3->bit_presencia, elem_iterado_3->bit_modificado, elem_iterado_3->bit_uso, elem_iterado_3->bit_puntero);                   
                }
            }

            list_iterator_destroy(iterator3);
            
        }
        
        list_iterator_destroy(iterator2);
        //printf("\n"); // Horrible despues sacar

    }

    list_iterator_destroy(iterator);

}

 
void finalizar_estructuras_del_proceso_y_avisar_a_kernel(int index_tabla_primer_nivel, int socket) {
    
    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, index_tabla_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    int marcos_a_liberar = paginas_con_marco_cargado_presente(index_tabla_primer_nivel);
    pthread_mutex_lock(&mutex_lista_segundo_nivel);

    log_info(logger,"Tengo que liberar %d páginas del proceso %d", marcos_a_liberar, index_tabla_primer_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            entrada_segundo_nivel -> bit_presencia;
            if(entrada_segundo_nivel->bit_presencia == 1) {

                // Actualizacion del bitmap
                bitarray_clean_bit(marcos_libres, entrada_segundo_nivel->marco);

                entrada_segundo_nivel->marco = -1;
                entrada_segundo_nivel->bit_presencia = 0;
                entrada_segundo_nivel->bit_modificado = 0;
                entrada_segundo_nivel->bit_uso = 0;
                entrada_segundo_nivel->bit_puntero = 0;

                log_info(logger, "Liberación de páginas del proceso  %d : Página %d de la tabla %d de segundo nivel liberada\n\n", index_tabla_primer_nivel, j, i);
			} 
        }
        
    }

    eliminar_archivo_swap(index_tabla_primer_nivel);
    
    //log_info(logger, "Ya reinicé estructuras, ahora mando confirmación a kernel");
    mostrar_bitmap();

    pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    int bytes_a_enviar = sizeof(int);
    void* a_enviar = malloc(bytes_a_enviar);
    int* codigo = malloc(sizeof(int));
    *codigo = 1;
    memcpy(a_enviar, &(*codigo), sizeof(int));
    send(socket, a_enviar, sizeof(int), 0);
    free(codigo);
    free(a_enviar);   

}

void eliminar_archivo_swap(int pid) {
    pedido_swap *pedido = malloc(sizeof(pedido_swap));
	pedido->co_op = ELIMINAR_ARCHIVO_SWAP;
    pedido->pid = pid;
    sem_init(&pedido->pedido_finalizado, 0, 0);

	pthread_mutex_lock(&mutexColaSwap);
	list_add(cola_pedidos_a_swap, pedido);
	pthread_mutex_unlock(&mutexColaSwap);
    sem_post(&realizar_op_de_swap);

    sem_wait(&pedido->pedido_finalizado);
}

int solicitar_pagina_a_swap(int pid, int numero_pagina) {

    int frame = buscar_frame_libre();

    pedido_swap *pedido = malloc(sizeof(pedido_swap));
	pedido->co_op = SWAP_IN_PAGINA;
    pedido->pid = pid;
    pedido->numero_pagina = numero_pagina;
    pedido->frame_libre = frame;
    sem_init(&pedido->pedido_finalizado, 0, 0);

	pthread_mutex_lock(&mutexColaSwap);
	list_add(cola_pedidos_a_swap, pedido);
	pthread_mutex_unlock(&mutexColaSwap);
    sem_post(&realizar_op_de_swap);

    sem_wait(&pedido->pedido_finalizado);

    return frame; 

}

void solicitar_swap_out_a_swap(int pid, int numero_pagina, int marco) {

    pedido_swap* pedido = malloc(sizeof(pedido_swap));
    pedido->co_op = SWAP_OUT_PAGINA;
    pedido->pid = pid;
    pedido->paginas_a_escribir = list_create();

    pagina_a_escribir* pagina_nueva = malloc(sizeof(pagina_a_escribir));
    pagina_nueva->pagina = numero_pagina;
    pagina_nueva->marco = marco;

    list_add(pedido->paginas_a_escribir, pagina_nueva);

    pthread_mutex_lock(&mutex_bitarray);
    bitarray_clean_bit(marcos_libres, marco);
    pthread_mutex_unlock(&mutex_bitarray);

    sem_init(&pedido->pedido_finalizado, 0, 0);

    pthread_mutex_lock(&mutexColaSwap);
	list_add(cola_pedidos_a_swap, pedido);
	pthread_mutex_unlock(&mutexColaSwap);
    sem_post(&realizar_op_de_swap);

    sem_wait(&pedido->pedido_finalizado);

}

void buscar_numero_de_pagina(int marco, int indice, pagina_victima* victima) {

    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, indice);

    for(int i = 0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++) {

        Entrada_Tabla_Primer_Nivel* entrada_p_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel* tabla_s_nivel = list_get(lista_tablas_segundo_nivel, entrada_p_nivel->index_tabla_segundo_nivel);

        for(int j = 0; j<tabla_s_nivel->entradas_tabla_segundo_nivel->elements_count; j++) {

            Entrada_Tabla_Segundo_Nivel* entrada_s_nivel = list_get(tabla_s_nivel->entradas_tabla_segundo_nivel, j);

            if(entrada_s_nivel->marco == marco) {
                victima->numero_pagina = j;
                if(entrada_s_nivel->bit_modificado) {
                    victima->fue_modificada = true;
                } else {
                    victima->fue_modificada = false;
                }
			} 
        }
        
    }

    log_info(logger, "En el marco %d esta la pagina %d", marco, victima->numero_pagina);
}

void actualizar_tabla_de_paginas(int index_tabla_segundo_nivel, int entrada_tabla_segundo_nivel, int marco) {

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
	Tabla_Segundo_Nivel* t_segundo_nivel = list_get(lista_tablas_segundo_nivel, index_tabla_segundo_nivel);

    Entrada_Tabla_Segundo_Nivel* pagina_a_actualizar = list_get(t_segundo_nivel->entradas_tabla_segundo_nivel, entrada_tabla_segundo_nivel);

    pagina_a_actualizar->marco = marco;
    pagina_a_actualizar->bit_presencia = 1;
    pagina_a_actualizar->bit_modificado = 0;
    pagina_a_actualizar->bit_uso = 1;
    pagina_a_actualizar->bit_puntero = 0;
	pthread_mutex_unlock(&mutex_lista_segundo_nivel);

    //list_iterator_destroy(iterator);
    log_info(logger, "Se actualizo la tabla de paginas\n\n");
    verificar_memoria();

}

/*bool comparator_orden_de_carga(void* elem1, void* elem2){
    Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel_1 = elem1;
    Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel_2 = elem2;
    return entrada_segundo_nivel_1->orden_de_carga < entrada_segundo_nivel_2->orden_de_carga;
    
}*/

/*void ordenar_lista_con_paginas_cargadas_segun_orden_de_carga(){
    list_sort(lista_paginas_cargadas_en_orden, comparator_orden_de_carga);
    Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel_iter;
    log_info(logger,"---\n\nInicio comprobación lista de carga de paginas ordenada----\n");
    for(int i=0; i<lista_paginas_cargadas_en_orden->elements_count;i++){
        entrada_segundo_nivel_iter = list_get(lista_paginas_cargadas_en_orden, i);
        log_info(logger,"El orden de carga de la entrada es: %d", entrada_segundo_nivel_iter->orden_de_carga);
    }
    log_info(logger,"---\n\nFin comprobación lista de carga de paginas ordenada----\n\n");
}*/

void generar_lista_de_paginas_cargadas(int index_tabla_primer_nivel){

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, index_tabla_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            if(entrada_segundo_nivel->bit_presencia == 1) {
                list_add(lista_paginas_cargadas,entrada_segundo_nivel);
			} 
        }
        
    }
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    log_info(logger, "Lista con páginas cargadas populada.\n\n");
}

void actualizar_bit_presencia(int pid) {

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, pid);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            // Se podria poner para que tmb el bit de presencia sea 1 pero deberia alcanzar
            if(entrada_segundo_nivel->bit_presencia == 1) {
                entrada_segundo_nivel->bit_presencia = 0;
			} 
        }    
    }
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    
}


void actualizar_bit_modificado(int pid, int marco) {

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, pid);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            // Se podria poner para que tmb el bit de presencia sea 1 pero deberia alcanzar
            if(entrada_segundo_nivel->marco == marco) {
                entrada_segundo_nivel->bit_modificado = 1;
			} 
        }    
    }
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    log_info(logger, "Se actualizo el bit de modificado del marco %d del proceso %d", marco, pid);
}

void actualizar_bit_puntero(int index){
    index++;

    if (index >= (lista_paginas_cargadas->elements_count)){
        index = 0;
    }

    Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(lista_paginas_cargadas, index);
    entrada_segundo_nivel-> bit_puntero = 1;
}

int buscar_index_puntero_para_aplicar_algoritmo(){

    int index = 0;
    for(int i=0; i<lista_paginas_cargadas->elements_count; i++){

        Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(lista_paginas_cargadas, i);
        if(entrada_segundo_nivel->bit_puntero == 1) {
            index = i;
        }
    }
    return index;
}

int aplicar_algoritmo_de_sustitucion_clock(){
    
    int marco = -1;
    Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel;
    int index_puntero = buscar_index_puntero_para_aplicar_algoritmo();
    bool primera_vez = true;

    while(marco == -1){
        entrada_segundo_nivel = list_get(lista_paginas_cargadas, index_puntero);

        //Desasigno el bit inicial de puntero
        if (primera_vez){
            entrada_segundo_nivel->bit_puntero = 0;
            primera_vez = false;
        }
        //Si el bit de uso es 0 es el marco a sustituir, sino, le doy una oportunidad y sigo
        if(entrada_segundo_nivel->bit_uso == 0) {
            marco = entrada_segundo_nivel->marco;
            break;
        } else{
            entrada_segundo_nivel->bit_uso = 0;
        }
        index_puntero++;
        if (index_puntero >= (lista_paginas_cargadas->elements_count)){
            index_puntero = 0;
        }
    }
    log_info(logger, "Algoritmo Clock Finalizado: el marco a descargar es %d", marco);
    actualizar_bit_puntero(index_puntero);
    return marco;
}

int aplicar_algoritmo_de_sustitucion_clock_modificado() {

    int marco = -1;
    bool no_encontre_marco = true;

    Entrada_Tabla_Segundo_Nivel* entrada_segundo_nivel;

    int index_puntero = buscar_index_puntero_para_aplicar_algoritmo();
    int index_inicial = index_puntero;

    while(marco == -1) {

        while(marco == -1) {
            entrada_segundo_nivel = list_get(lista_paginas_cargadas, index_puntero);

            if(entrada_segundo_nivel->bit_uso == 0 && entrada_segundo_nivel->bit_modificado == 0) {
                marco = entrada_segundo_nivel->marco;
                break;

            }

            index_puntero++; 
            if (index_puntero >= (lista_paginas_cargadas->elements_count)) {
                index_puntero = 0;
            }

            if(index_puntero == index_inicial) {
                break;
            } //ya me di la vuelta 

        }

        while(marco == -1) {
            
            entrada_segundo_nivel = list_get(lista_paginas_cargadas, index_puntero);

            if(entrada_segundo_nivel->bit_uso == 0 && entrada_segundo_nivel->bit_modificado == 1) {
                marco = entrada_segundo_nivel->marco;
                break;

            } else {
                entrada_segundo_nivel->bit_uso = 0;
                verificar_memoria();
            }
            
            index_puntero++;
            if (index_puntero >= (lista_paginas_cargadas->elements_count)) {
                index_puntero = 0;
            }

            if(index_puntero == index_inicial) {
                break;
            } //me di la vuelta de nuevo. Tengo que empezar de nuevo.

        }
          
    }

    log_info(logger, "Algoritmo CLOCK-M finalizado: el marco a descargar es %d", marco);
    actualizar_bit_puntero(index_puntero);

    return marco;

}

void escribir_paginas_modificadas(int pid) {

    pedido_swap* pedido = malloc(sizeof(pedido_swap));
    pedido->co_op = SWAP_OUT_PAGINA;
    pedido->pid = pid;
    pedido->paginas_a_escribir = list_create();

    bool hay_paginas_modificadas = false;

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, pid);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            
            if(entrada_segundo_nivel->bit_presencia == 1 && entrada_segundo_nivel->bit_modificado == 1) {
                
                hay_paginas_modificadas = true;
                
                pagina_a_escribir* pagina_nueva = malloc(sizeof(pagina_a_escribir));
                pagina_nueva->pagina = i*config.ENTRADAS_POR_TABLA + j;
                pagina_nueva->marco = entrada_segundo_nivel->marco;
                list_add(pedido->paginas_a_escribir, pagina_nueva);

                pthread_mutex_lock(&mutex_bitarray);
                bitarray_clean_bit(marcos_libres, entrada_segundo_nivel->marco);
                pthread_mutex_unlock(&mutex_bitarray);

			} 
        }    
    }
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);

    if(hay_paginas_modificadas) {

        sem_init(&pedido->pedido_finalizado, 0, 0);

        pthread_mutex_lock(&mutexColaSwap);
	    list_add(cola_pedidos_a_swap, pedido);
	    pthread_mutex_unlock(&mutexColaSwap);
        sem_post(&realizar_op_de_swap);

        sem_wait(&pedido->pedido_finalizado);
        log_info(logger, "Se escribieron en swap las paginas modificadas del proceso %d", pid);

    } else {
        list_destroy(pedido->paginas_a_escribir);
        free(pedido);
        log_info(logger, "El proceso %d no tiene paginas modificadas para escribir en swap", pid);
    }

}