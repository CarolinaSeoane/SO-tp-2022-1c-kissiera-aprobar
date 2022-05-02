#include "../include/conexiones_memoria.h"

void send_handshake(int conexion) {
	void* a_enviar = malloc(sizeof(int));
	int* codigo = malloc(sizeof(int));
	*codigo = HANDSHAKE_MEMORIA;
	memcpy(a_enviar, &(*codigo), sizeof(int));
	send(conexion, a_enviar, sizeof(int), 0);
	free(a_enviar);
	free(codigo);
}

int recv_handshake(int conexion_memoria) {

	int codigo_handshake;
	if(recv(conexion_memoria, &codigo_handshake, sizeof(int), 0) == -1){
		log_info(logger, "Fallo el handshake con memoria");
		return EXIT_FAILURE;
	}
	log_info(logger, "Handshake realizado con éxito");

	recv(conexion_memoria, &tamanio_pagina, sizeof(tamanio_pagina), 0);
	printf("El tamanio de pagina enviado por Memoria es: %d\n", tamanio_pagina);

	recv(conexion_memoria, &cant_entradas_tabla, sizeof(cant_entradas_tabla), 0);
	printf("La cantidad de entradas por tabla enviada por Memoria es: %d\n", cant_entradas_tabla);

	return 1;
}

