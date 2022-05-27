#include "../include/utils.h"

void cargarConfig(char* path, Config* config) {
    t_config* t_config = config_create(path);

    if (t_config != NULL) {
        (*config).ENTRADAS_TLB                     = config_get_int_value(t_config, "ENTRADAS_TLB");
        (*config).REEMPLAZO_TLB                    = strdup(config_get_string_value(t_config, "REEMPLAZO_TLB"));
        (*config).RETARDO_NOOP                     = config_get_int_value(t_config, "RETARDO_NOOP");
        (*config).IP_MEMORIA                       = strdup(config_get_string_value(t_config, "IP_MEMORIA"));
        (*config).PUERTO_MEMORIA                   = strdup(config_get_string_value(t_config, "PUERTO_MEMORIA"));
        (*config).PUERTO_ESCUCHA_DISPATCH          = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA_DISPATCH"));
        (*config).PUERTO_ESCUCHA_INTERRUPT         = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA_INTERRUPT"));

        log_info(logger, "Configuracion cargada correctamente");
        config_destroy(t_config);

    } else {
        log_info(logger, "No se encontró el archivo de configuración");
        exit(0);
    } 
}

void inicializar_semaforos() {
	sem_init(&mutex_flag_interrupcion, 0, 1);
}

void inicializar_flags() {
    sem_wait(&mutex_flag_interrupcion);
    flag_interrupcion = 0;
    sem_post(&mutex_flag_interrupcion);
    flag_syscall = 0;
}

void inicializar_logger() {
	logger = log_create("cpu.log", "Cpu", 1, LOG_LEVEL_DEBUG);
}

void inicializar_config() {
	cargarConfig("cpu.config", &config);
}

void send_handshake(int conexion) {
	void* a_enviar = malloc(sizeof(int));
	int* codigo = malloc(sizeof(int));
	*codigo = HANDSHAKE_MEMORIA;
	memcpy(a_enviar, &(*codigo), sizeof(int));
	send(conexion, a_enviar, sizeof(int), 0);
	free(a_enviar);
	free(codigo);
}

int recv_handshake(int conexion_memoria) {
	int codigo_handshake;
	if(recv(conexion_memoria, &codigo_handshake, sizeof(int), 0) == -1){
		log_error(logger, "Fallo el handshake con memoria");
		return EXIT_FAILURE;
	}
	log_info(logger, "Handshake realizado con éxito");

	recv(conexion_memoria, &tamanio_pagina, sizeof(tamanio_pagina), 0);
	log_info(logger, "El tamanio de pagina enviado por Memoria es: %d", tamanio_pagina);

	recv(conexion_memoria, &cant_entradas_tabla, sizeof(cant_entradas_tabla), 0);
	log_info(logger, "La cantidad de entradas por tabla enviada por Memoria es: %d", cant_entradas_tabla);

	return 1;
}

void inicializar_conexiones() {
	conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);
	send_handshake(conexion_memoria);
	recv_handshake(conexion_memoria);
}

void inicializar_servidores() {
    dispatch = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_DISPATCH, 1);
	interrupt = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_INTERRUPT, 1);
    if(!dispatch || !interrupt) {
		log_error(logger, "Error al iniciar la conexión dispatch o interrupt");		
	}
}

void destroy_recursos() {
    log_destroy(logger);
    close(conexion_memoria);
    sem_destroy(&mutex_flag_interrupcion);
}