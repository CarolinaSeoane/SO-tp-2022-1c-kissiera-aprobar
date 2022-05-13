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

void inicializar_semaforo() {
    
	sem_init(&mutex_flag_interrupcion, 0, 1);

    sem_wait(&mutex_flag_interrupcion);
    flag_interrupcion = 0;
    sem_post(&mutex_flag_interrupcion);

}