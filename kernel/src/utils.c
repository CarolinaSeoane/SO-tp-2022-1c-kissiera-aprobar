#include "../include/utils.h"

void cargarConfig(char* path, Config* config) {
    t_config* t_config = config_create(path);

    if (t_config != NULL) {
        (*config).IP_MEMORIA                   = strdup(config_get_string_value(t_config, "IP_MEMORIA"));
        (*config).PUERTO_MEMORIA               = strdup(config_get_string_value(t_config, "PUERTO_MEMORIA"));
        (*config).IP_CPU                       = strdup(config_get_string_value(t_config, "IP_CPU"));
        (*config).PUERTO_CPU_DISPATCH          = strdup(config_get_string_value(t_config, "PUERTO_CPU_DISPATCH"));
        (*config).PUERTO_CPU_INTERRUPT         = strdup(config_get_string_value(t_config, "PUERTO_CPU_INTERRUPT"));
        (*config).PUERTO_ESCUCHA               = strdup(config_get_string_value(t_config, "PUERTO_ESCUCHA"));
        (*config).ALGORITMO_PLANIFICACION      = strdup(config_get_string_value(t_config, "ALGORITMO_PLANIFICACION"));
        (*config).ESTIMACION_INICIAL           = config_get_int_value(t_config, "ESTIMACION_INICIAL");
        (*config).ALFA                         = config_get_double_value(t_config, "ALFA");
        (*config).GRADO_MULTIPROGRAMACION      = config_get_int_value(t_config, "GRADO_MULTIPROGRAMACION");

        log_info(logger, "Configuración cargada correctamente");
        config_destroy(t_config);

    } else {
        log_error(logger, "No se encontró el archivo de configuración");
        exit(0);
    }
}

void inicializar_colas() {
	cola_new = list_create();
	cola_ready = list_create();
	cola_exec = list_create();
	cola_blck = list_create();
	cola_finish = list_create();
}

void inicializar_semaforos() {
	pthread_mutex_init(&mutexBlockSuspended, NULL);
	pthread_mutex_init(&mutexReadySuspended, NULL);
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexBlock, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	pthread_mutex_init(&mutex_mover_de_new_a_ready, NULL);
}

void inicializar_logger() {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
}

Config inicializar_config() {
	cargarConfig("kernel.config", &config);
}

void inicializar_servidor() {
    kernel_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, SOMAXCONN);
	if(!kernel_server) {
		log_error(logger, "Error al iniciar el servidor Kernel\nCerrando el programa");
	}
}

void inicializar_conexiones() {
    conexion_dispatch = crear_conexion(config.IP_CPU, config.PUERTO_CPU_DISPATCH, logger);
    conexion_interrupt = crear_conexion(config.IP_CPU, config.PUERTO_CPU_INTERRUPT, logger);
    conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);
}