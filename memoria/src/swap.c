#include "../include/swap.h"

void* atender_pedidos_swap() {

    // Crea directorio si no existe
    if (access(config.PATH_SWAP, F_OK) == 0) {
        log_info(logger, "El directorio %s ya existe", config.PATH_SWAP); //lo podemos borrar
    } else {
        mkdir(config.PATH_SWAP, 0777);
    }

    while(1) {
        sem_wait(&swap_esta_libre);
        sem_wait(&realizar_op_de_swap);

        log_info(logger, "CONECTANDO CON SWAP...");
        usleep(config.RETARDO_SWAP * 1000); // Retardo swap
        
        pedido_swap *pedido = list_remove(cola_pedidos_a_swap, 0); //Obtengo pedido

        switch(pedido->co_op) {
            case INIT_PROCESO:
                log_info(logger, "SWAP RECIBE INIT PARA PID %d", pedido->pid);

                char* path = get_file_name(pedido->pid);
                FILE* f = fopen(path,"w");

                if (f == NULL) {
			        log_error(logger, "Error al crear archivo para PID %d en %s", pedido->pid, path);
			        exit(EXIT_FAILURE);
		        }
                
                ftruncate(fileno(f), pedido->tamanio_proceso);             

                fclose(f);
                free(path);

                break;

            case SWAP_IN_PAGINA:
                log_info(logger, "SWAP recibe SWAP IN para proceso %d, pag %d y la va a cargar en el frame %d", pedido->pid, pedido->numero_pagina, pedido->frame_libre);   

                char* file_name = get_file_name(pedido->pid);
                FILE *fp = fopen(file_name, "r");

                if(fp == NULL) {
                    log_error(logger, "Error al abrir archivo %s, cerrando modulo swap", file_name);
                    exit(1);
                }
      
                fseek(fp, pedido->numero_pagina * config.TAM_PAGINA, SEEK_SET);
                log_info(logger, "El puntero esta en la pos: %ld y va a leer %d bytes", ftell(fp), config.TAM_PAGINA);

                fread(memoria_principal + (pedido->frame_libre * config.TAM_PAGINA), 1, config.TAM_PAGINA, fp);

                // seguir
  
                
                //log_info(logger, "El archivo %s tiene un tamaño de %d bytes y se va a traer a memoria desde el byte %d hasta el %d", file_name, (int) file_st.st_size, offset, offset + config.TAM_PAGINA);

                // Mapeo
                //char *addr;
                //addr = mmap(memoria_principal + (pedido->frame_libre * config.TAM_PAGINA), config.TAM_PAGINA, PROT_READ, MAP_SHARED, file, pedido->numero_pagina * config.TAM_PAGINA);
                //
                //if(addr == MAP_FAILED) {
                //    log_error(logger, "Error al mapear archivo %s, cerrando modulo swap", file_name);
                //    exit(1);
                //}
                //
                //close(file);
                free(file_name);
                fclose(fp);
                //free(addr);
                break;

            case ELIMINAR_ARCHIVO_SWAP: ;
                    char* archivo_a_eliminar = get_file_name(pedido->pid);

                    if (remove(archivo_a_eliminar) == 0) {
                        log_info(logger, "El archivo %s ha sido eliminado", archivo_a_eliminar);
                    } else {
                        log_error(logger, "Error al eliminar el archivo %s", archivo_a_eliminar);
                    }

                    free(archivo_a_eliminar);
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