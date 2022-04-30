#include "../include/utils.h"

void cargarConfig(char* path, Config* config) {
    t_config* t_config = config_create(path);

    if (t_config != NULL) {
        (*config).IP_KERNEL                   = strdup(config_get_string_value(t_config, "IP_KERNEL"));
        (*config).PUERTO_KERNEL               = strdup(config_get_string_value(t_config, "PUERTO_KERNEL"));

        log_info(logger, "Configuración cargada correctamente");
        config_destroy(t_config);

    } else {
        log_info(logger, "No se encontró el archivo de configuración");
        exit(0);
    }
}

