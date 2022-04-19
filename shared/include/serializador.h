#ifndef SERIALIZADOR_H_
#define SERIALIZADOR_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<string.h>
#include<netdb.h>
#include"protocolo.h"

typedef struct {
    uint32_t size; // Tamaño del payload
    void* stream; // Payload
} t_buffer;

typedef struct
{
	int socket;
	id_modulo modulo;
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;


#endif
