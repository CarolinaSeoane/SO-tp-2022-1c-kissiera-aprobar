#include "../include/swap.h"

void* atender_pedidos_swap() {

    // Crea directorio si no existe
    if (access(config.PATH_SWAP, F_OK) == 0) {
        log_info(logger, "El directorio %s ya existe. Borrando directorio...", config.PATH_SWAP);
        if(rmdir(config.PATH_SWAP) == -1) {
            log_error(logger, "El directorio %s no pudo borrarse porque no está vacío", config.PATH_SWAP);
        }
    }
    mkdir(config.PATH_SWAP, 0777);

    while(1) {
        sem_wait(&swap_esta_libre);
        sem_wait(&realizar_op_de_swap);

        log_info(logger, "CONECTANDO CON SWAP...");
                
        pedido_swap *pedido = list_remove(cola_pedidos_a_swap, 0); //Obtengo pedido

        switch(pedido->co_op) {
            case INIT_PROCESO:
                log_info(logger, "Swap recibe INIT_PROCESO PARA PID %d\n", pedido->pid);

                char* path = get_file_name(pedido->pid);
                FILE* f = fopen(path,"w+");

                if (f == NULL) {
			        log_error(logger, "Error al crear archivo para PID %d en %s", pedido->pid, path);
			        exit(EXIT_FAILURE);
		        }
                
                ftruncate(fileno(f), pedido->tamanio_proceso);             

                fclose(f);
                free(path);

                break;

            case SWAP_IN_PAGINA:
                usleep(config.RETARDO_SWAP * 1000);
                log_info(logger, "SWAP recibe SWAP IN PAGINA para proceso %d, pag %d y la va a cargar en el frame %d", pedido->pid, pedido->numero_pagina, pedido->frame_libre);   

                char* file_name = get_file_name(pedido->pid);
                FILE *fp = fopen(file_name, "r");

                if(fp == NULL) {
                    log_error(logger, "Error al abrir archivo %s, cerrando modulo swap", file_name);
                    exit(1);
                }
      
                fseek(fp, pedido->numero_pagina * config.TAM_PAGINA, SEEK_SET);
                log_info(logger, "El puntero esta en la pos: %ld y va a leer %d bytes", ftell(fp), config.TAM_PAGINA);

                void* buffer = malloc(config.TAM_PAGINA);
                fread(buffer, config.TAM_PAGINA, 1, fp);

                pthread_mutex_lock(&mutex_memoria);
                memcpy(memoria_principal + (pedido->frame_libre * config.TAM_PAGINA), buffer, config.TAM_PAGINA);
                pthread_mutex_unlock(&mutex_memoria);

                //log_info(logger, "Despues de leer el puntero esta en la pos: %ld", ftell(fp));
                log_info(logger, "Se copio la pagina %d a memoria en el frame %d", pedido->numero_pagina, pedido->frame_libre);

                free(file_name);
                free(buffer);
                fclose(fp);
                
                break;

            case SWAP_OUT_PAGINA:
                usleep(config.RETARDO_SWAP * 1000);
                log_info(logger, "SWAP recibe SWAP OUT para proceso %d", pedido->pid);

                char* file_name_swap_out = get_file_name(pedido->pid);
                FILE *fp_swap_out = fopen(file_name_swap_out, "w+");

                if(fp_swap_out == NULL) {
                    log_error(logger, "Error al abrir archivo %s, cerrando modulo swap", file_name_swap_out);
                    exit(1);
                }
      
                t_list_iterator* iterator = list_iterator_create(pedido->paginas_a_escribir);
                pagina_a_escribir* elem_iterado;

                while(list_iterator_has_next(iterator)) {
                    elem_iterado = list_iterator_next(iterator);
                    fseek(fp_swap_out, elem_iterado->pagina * config.TAM_PAGINA, SEEK_SET);
                    log_info(logger, "El puntero esta en la pos: %ld y va a escribir %d bytes", ftell(fp_swap_out), config.TAM_PAGINA);

                    void* buffer_swap_out = malloc(config.TAM_PAGINA);

                    pthread_mutex_lock(&mutex_memoria);
                    memcpy(buffer_swap_out, memoria_principal + (elem_iterado->marco * config.TAM_PAGINA), config.TAM_PAGINA);
                    pthread_mutex_unlock(&mutex_memoria);

                    fwrite(buffer_swap_out, config.TAM_PAGINA, 1, fp_swap_out);

                    log_info(logger, "Se escribio el frame %d en la pagina %d del archivo swap", elem_iterado->marco, elem_iterado->pagina);
                    free(buffer_swap_out);
                
                }

                list_iterator_destroy(iterator);
                list_destroy(pedido->paginas_a_escribir);
                free(file_name_swap_out);
                fclose(fp_swap_out);

                break;

            case ELIMINAR_ARCHIVO_SWAP: ;
                char* archivo_a_eliminar = get_file_name(pedido->pid);
                log_info(logger, "SWAP recibe ELIMINAR ARCHIVO para proceso %d", pedido->pid);
                if (remove(archivo_a_eliminar) == 0) {
                    log_info(logger, "El archivo %s ha sido eliminado", archivo_a_eliminar);
                } else {
                    log_error(logger, "Error al eliminar el archivo %s", archivo_a_eliminar);
                }

                free(archivo_a_eliminar);
                log_info(logger, "Archivo del proceso %d eliminado con exito\n", pedido->pid);

                break;

            default:
                log_warning_sh(logger, "Operacion desconocida de Swap");
                break;
        }
        sem_post(&pedido->pedido_finalizado);
        sem_post(&swap_esta_libre);
        
        sem_destroy(&pedido->pedido_finalizado);
        free(pedido);
    }
}