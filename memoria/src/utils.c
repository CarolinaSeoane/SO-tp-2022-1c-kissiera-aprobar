#include "../include/utils.h"

void cargarConfig(char* path, Config* config) {

	t_config* t_config = config_create(path);

    if (t_config != NULL) {
        (*config).PUERTO_ESCUCHA            = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA"));
        (*config).TAM_MEMORIA               = config_get_int_value(t_config, "TAM_MEMORIA");
        (*config).TAM_PAGINA                = config_get_int_value(t_config, "TAM_PAGINA");
        (*config).ENTRADAS_POR_TABLA        = config_get_int_value(t_config, "ENTRADAS_POR_TABLA");
        (*config).RETARDO_MEMORIA           = config_get_int_value(t_config, "RETARDO_MEMORIA");
        (*config).ALGORITMO_REEMPLAZO       = strdup(config_get_string_value(t_config, "ALGORITMO_REEMPLAZO"));
        (*config).MARCOS_POR_PROCESO        = config_get_int_value(t_config, "MARCOS_POR_PROCESO");
        (*config).RETARDO_SWAP              = config_get_int_value(t_config, "RETARDO_SWAP");
        (*config).PATH_SWAP                 = strdup(config_get_string_value(t_config, "PATH_SWAP"));

        log_info(logger, "Configuración cargada correctamente");
        config_destroy(t_config);

    } else {
        log_error(logger, "No se encontró el archivo de configuración");
        exit(0);
    }
}

void inicializar_logger() {
	logger = log_create("memoria.log", "Memoria", 1, LOG_LEVEL_DEBUG);
    logger_bitmap = log_create("bitmap.log", "Bitmap", 1, LOG_LEVEL_DEBUG);
}

void inicializar_config() {
	cargarConfig("memoria.config", &config);
}

void inicializar_servidor() {
    memoria_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, 2);
	if(!memoria_server) {
		log_error(logger, "Error al iniciar el servidor Memoria");
	}
}

void inicializar_semaforos() {
    pthread_mutex_init(&mutex_memoria, NULL);
    pthread_mutex_init(&mutex_lista_primer_nivel, NULL);
    pthread_mutex_init(&mutex_lista_segundo_nivel, NULL);
    pthread_mutex_init(&mutexColaSwap, NULL);
    pthread_mutex_init(&mutex_bitarray, NULL);
    sem_init(&realizar_op_de_swap, 0, 0);
    sem_init(&swap_esta_libre, 0, 1);
}

void destroy_recursos() {
    log_destroy(logger);

    pthread_mutex_destroy(&mutex_memoria);
    pthread_mutex_destroy(&mutex_lista_primer_nivel);
    pthread_mutex_destroy(&mutex_lista_segundo_nivel);
    pthread_mutex_destroy(&mutexColaSwap);
    sem_destroy(&realizar_op_de_swap);
    sem_destroy(&swap_esta_libre);

    bitarray_destroy(marcos_libres);
    free(memoria_principal);
    
    if(rmdir(config.PATH_SWAP) == -1) {
            log_error(logger, "El directorio %s no pudo borrarse porque no está vacío", config.PATH_SWAP);
        }
}

void inicializar_memoria_principal() {
    memoria_principal = malloc(config.TAM_MEMORIA);
    int cantidad_marcos = config.TAM_MEMORIA / config.TAM_PAGINA;
    char* data = asignar_bytes(cantidad_marcos);
    marcos_libres = bitarray_create_with_mode(data, cantidad_marcos/8, MSB_FIRST);
    log_info(logger, "Memoria inicializada con %d frames", cantidad_marcos);
}

void inicializar_tablas_de_paginas() {
    lista_tablas_primer_nivel = list_create();
    lista_tablas_segundo_nivel = list_create();
    lista_paginas_cargadas = list_create();
}

void inicializar_swap() {
    pthread_create(&hilo_swap, NULL, atender_pedidos_swap, NULL);
    pthread_detach(hilo_swap);
    cola_pedidos_a_swap = list_create();
}

char* get_file_name(int pid) {
    char file_name[5];
    sprintf(file_name, "%d", pid);

    char* path = strdup(config.PATH_SWAP);
    string_append(&path, "/");
    string_append(&path, file_name);
    string_append(&path, ".swap");

    return path;
}

char* asignar_bytes(int cantidad_frames) {
    char* buffer;
    int bytes;
    if(cantidad_frames < 8)
        bytes = 1;
    else
    {
        double c = (double) cantidad_frames;
        bytes = (int) ceil(c/8.0);
    }
    buffer = malloc(bytes);
    memset(buffer,0,bytes);
    return buffer;
}

int paginas_con_marco_cargado_presente(int index_tabla_primer_nivel){

    pthread_mutex_lock(&mutex_lista_primer_nivel);
    Tabla_Primer_Nivel* t_primer_nivel = list_get(lista_tablas_primer_nivel, index_tabla_primer_nivel);
    pthread_mutex_unlock(&mutex_lista_primer_nivel);

    int paginas_ocupadas = 0;

    pthread_mutex_lock(&mutex_lista_segundo_nivel);
    for(int i=0; i<t_primer_nivel->entradas_tabla_primer_nivel->elements_count; i++){
        
        Entrada_Tabla_Primer_Nivel * entrada_primer_nivel = list_get(t_primer_nivel->entradas_tabla_primer_nivel, i);
        Tabla_Segundo_Nivel * tabla_segundo_nivel = list_get(lista_tablas_segundo_nivel, entrada_primer_nivel->index_tabla_segundo_nivel);

        for(int j=0; j<tabla_segundo_nivel->entradas_tabla_segundo_nivel->elements_count; j++){

            Entrada_Tabla_Segundo_Nivel * entrada_segundo_nivel = list_get(tabla_segundo_nivel->entradas_tabla_segundo_nivel, j);
            if(entrada_segundo_nivel->bit_presencia == 1) {
                paginas_ocupadas += 1;
                
			} 
        }
        
    }
    //log_info(logger, "Páginas cargadas en memoria del proceso: %d\n\n", paginas_ocupadas);
    pthread_mutex_unlock(&mutex_lista_segundo_nivel);
    
    return paginas_ocupadas;

}

int buscar_frame_libre() {

    pthread_mutex_lock(&mutex_bitarray);
    int cantidad_bits = bitarray_get_max_bit(marcos_libres);
    int i = 0; 
    while(i < cantidad_bits) {
        if(!bitarray_test_bit(marcos_libres, i)) {
            bitarray_set_bit(marcos_libres, i);
            pthread_mutex_unlock(&mutex_bitarray);
            return i;
            break;
        } else {
            i++;
            pthread_mutex_unlock(&mutex_bitarray);
        }
    }

}