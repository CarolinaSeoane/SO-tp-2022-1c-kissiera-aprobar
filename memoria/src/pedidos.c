#include "../include/pedidos.h"

int atender_pedido(int conexion) {

    int co_op;


    switch(co_op) {
        case READ_M:
            
            break;

        case WRITE_M:
            
            break;

        case INIT_PROCESO:
            
            break;

        case SUSP_PROCESO:
            
            break;

        default:
            log_warning_sh(logger, "Operacion desconocida.");
			close(conexion);
			break;
    }

    return 0;

}