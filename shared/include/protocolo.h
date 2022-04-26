#ifndef PROTOCOLO_H
#define PROTOCOLO_H

typedef enum {
	ENVIAR_INSTRUCCIONES,
	READ_M,
	WRITE_M, //les agrego _M (de memoria) para que no interfieran con los codigos de instruccion
	INIT_PROCESO,
	SUSP_PROCESO
}accion;

#endif