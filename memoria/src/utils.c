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
        log_info(logger, "No se encontró el archivo de configuración");
        exit(0);
    }
}
