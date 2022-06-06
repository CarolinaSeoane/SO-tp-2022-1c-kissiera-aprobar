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
        (*config).ESTIMACION_INICIAL           = config_get_double_value(t_config, "ESTIMACION_INICIAL");
        (*config).ALFA                         = config_get_double_value(t_config, "ALFA");
        (*config).GRADO_MULTIPROGRAMACION      = config_get_int_value(t_config, "GRADO_MULTIPROGRAMACION");
        (*config).TIEMPO_MAXIMO_BLOQUEADO      = config_get_int_value(t_config, "TIEMPO_MAXIMO_BLOQUEADO");


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
	cola_blck = list_create();
    cola_suspended_ready = list_create();
	cola_suspended_blck = list_create();

    procesos_que_se_van_a_suspender = list_create();
}

void inicializar_semaforos() {
    pthread_mutex_init(&mutex_PID, NULL);
	pthread_mutex_init(&mutexSuspendedBlocked, NULL);
	pthread_mutex_init(&mutexSuspendedReady, NULL);
	pthread_mutex_init(&mutexNew, NULL);
	pthread_mutex_init(&mutexReady, NULL);
	pthread_mutex_init(&mutexBlock, NULL);
	pthread_mutex_init(&mutexExe, NULL);
	pthread_mutex_init(&mutexExit, NULL);
	pthread_mutex_init(&mutex_vg_ex, NULL);

    pthread_mutex_init(&mutexProcesosQueSeVanASuspender, NULL);

    sem_init(&sem_hilo_new_ready, 0, 0);
    sem_init(&sem_hilo_exec_exit, 0, 0);
    sem_init(&sem_hilo_ready_susp_ready, 0, 0);
    sem_init(&sem_planificar_FIFO, 0, 1);
    sem_init(&sem_planificar_SRT, 0, 1);
    sem_init(&sem_hay_procesos_en_ready, 0, 0);
    sem_init(&finalizar, 0, 0);
    sem_init(&sem_ejecutar_IO, 0, 0);
    sem_init(&IO_esta_disponible, 0, 1);
    sem_init(&sem_grado_multiprogramacion, 0, config.GRADO_MULTIPROGRAMACION);
    sem_init(&sem_hilo_blocked_a_blocked_susp, 0, 0);
    sem_init(&sem_desalojar, 0, 0);
}

void inicializar_logger() {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
}

void inicializar_config() {
	cargarConfig("kernel.config", &config);
}

void inicializar_servidor() {
    kernel_server = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, SOMAXCONN);
	if(!kernel_server) {
		log_error(logger, "Error al iniciar el servidor Kernel");
	}
}

void inicializar_conexiones() {
    conexion_dispatch = crear_conexion(config.IP_CPU, config.PUERTO_CPU_DISPATCH, logger);
    pthread_create(&hilo_atender_pedidos_dispatch, NULL, atender_pedidos_dispatch, NULL);
    pthread_detach(hilo_atender_pedidos_dispatch);

    conexion_interrupt = crear_conexion(config.IP_CPU, config.PUERTO_CPU_INTERRUPT, logger);

    conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);
}

void destroy_recursos() {
    log_destroy(logger);
    close(conexion_dispatch);
    close(conexion_interrupt);
    close(conexion_memoria);
}

void inicializar_planificacion() {
    pthread_mutex_lock(&mutex_vg_ex);
    hay_un_proceso_ejecutando = 0;
    pthread_mutex_unlock(&mutex_vg_ex);

    pthread_mutex_lock(&mutex_vg_io);
    IO_ocupado = 0;
    pthread_mutex_unlock(&mutex_vg_io);

    pthread_create(&hilo_new_ready, NULL, priorizar_procesos_suspendidos_ready_sobre_new, NULL);
    pthread_create(&hilo_bloqueado_a_bloqueado_susp, NULL, pasar_de_bloqueado_a_bloqueado_susp, NULL);

    if(!strcmp(config.ALGORITMO_PLANIFICACION, "FIFO")) {
		pthread_create(&hilo_ready_exec, NULL, pasar_de_ready_a_exec_FIFO, NULL);
	} else {
		pthread_create(&hilo_ready_exec, NULL, pasar_de_ready_a_exec_SRT, NULL);
	} 

    pthread_create(&hilo_IO, NULL, ejecutar_IO, NULL);
}

void print_colas() {
    log_info(logger, "NEW:");
    print_elementos_de_una_cola(cola_new);
    log_info(logger, "READY:");
    print_elementos_de_una_cola(cola_ready);
    log_info(logger, "BLOCKED:");
    print_elementos_de_una_cola(cola_blck);
    log_info(logger, "SUSP/READY:");
    print_elementos_de_una_cola(cola_suspended_ready);
    log_info(logger, "SUSP/BLOCKED:");
    print_elementos_de_una_cola(cola_suspended_blck);
    log_info(logger, "PROCESOS QUE SE VAN A SUSPENDER:");
    print_elementos_de_una_cola(procesos_que_se_van_a_suspender);
    log_info(logger, " ");
    log_info(logger, " ");
}

void print_elementos_de_una_cola(t_list *cola) {
    t_list_iterator* iterator = list_iterator_create(cola);
	PCB* elem_iterado;

	while(list_iterator_has_next(iterator)) {
		elem_iterado = list_iterator_next(iterator);        
		log_info(logger, "%d", elem_iterado->pid);
	}

	list_iterator_destroy(iterator);
}