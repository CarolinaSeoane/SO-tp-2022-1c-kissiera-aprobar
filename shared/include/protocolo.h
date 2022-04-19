#ifndef PROTOCOLO_H
#define PROTOCOLO_H

typedef enum
{
	STR_SIN_RESPUESTA,
	ENVIAR_ID,
	STR_CON_RESPUESTA

}op_code;

typedef enum
{
	CONSOLA,
	KERNEL,
	CPU,
	MEMORIA_Y_SWAP
}id_modulo;

#endif







