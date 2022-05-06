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