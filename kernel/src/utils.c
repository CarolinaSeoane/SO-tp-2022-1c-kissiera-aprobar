#include "../include/utils.h"

void cargarConfig(char* path) {
    t_config* t_config = config_create(path);

    if (t_config != NULL) {
        config.IP_MEMORIA                   = config_get_string_value(t_config, "IP_MEMORIA");
        config.PUERTO_MEMORIA               = config_get_string_value(t_config, "PUERTO_MEMORIA");
        config.IP_CPU                       = config_get_string_value(t_config, "IP_CPU");
        config.PUERTO_CPU_DISPATCH          = config_get_string_value(t_config, "PUERTO_CPU_DISPATCH");
        config.PUERTO_CPU_INTERRUPT         = config_get_string_value(t_config, "PUERTO_CPU_INTERRUPT");
        config.PUERTO_ESCUCHA               = config_get_string_value(t_config, "PUERTO_ESCUCHA");
        config.ALGORITMO_PLANIFICACION      = config_get_string_value(t_config, "ALGORITMO_PLANIFICACION");
        config.ESTIMACION_INICIAL           = config_get_int_value(t_config, "ESTIMACION_INICIAL");
        config.ALFA                         = config_get_double_value(t_config, "ALFA");
        config.GRADO_MULTIPROGRAMACION      = config_get_int_value(t_config, "GRADO_MULTIPROGRAMACION");

        log_info(logger, "Configuracion cargada correctamente");

    } else {
        log_info(logger, "Error al cargar archivo de configuracion");
    }
    config_destroy(t_config);
}

int iniciar_modulo() {
	logger = log_create("kernel.log", "Kernel", 1, LOG_LEVEL_DEBUG);
	cargarConfig("kernel.config");
	return iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA, SOMAXCONN);
}
