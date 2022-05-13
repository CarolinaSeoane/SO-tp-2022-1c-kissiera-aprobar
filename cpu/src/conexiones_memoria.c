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
		log_error(logger, "Fallo el handshake con memoria");
		return EXIT_FAILURE;
	}
	log_info(logger, "Handshake realizado con éxito");

	recv(conexion_memoria, &tamanio_pagina, sizeof(tamanio_pagina), 0);
	log_info(logger, "El tamanio de pagina enviado por Memoria es: %d", tamanio_pagina);

	recv(conexion_memoria, &cant_entradas_tabla, sizeof(cant_entradas_tabla), 0);
	log_info(logger, "La cantidad de entradas por tabla enviada por Memoria es: %d", cant_entradas_tabla);

	return 1;
}

// Faltaria la traduccion de dirección lógica a física
void send_pedido_lectura(Proceso_CPU* proceso, instruccion inst, int conexion_memoria) {
	
	int leer = inst.operando1;

	// int direccion_fisica = traducir_direccion(direccion_logica);

	int* codigo = malloc(sizeof(int));
	*codigo = READ_M;

	void* paquete = malloc(sizeof(int)*2);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(leer), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*2, 0);

	free(codigo);
	free(paquete);

}

// Faltaria la traduccion de dirección lógica a física
void send_pedido_escritura(int direccion, int valor, int conexion_memoria) {

	// int direccion_fisica = traducir_direccion(direccion_logica);

	int* codigo = malloc(sizeof(int));
	*codigo = WRITE_M;

	void* paquete = malloc(sizeof(int)*3);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(direccion), sizeof(int)); // Sería la física
	offset += sizeof(int);
	memcpy(paquete + offset, &(valor), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*3, 0);

	free(codigo);
	free(paquete);

}

int recv_pedido_lectura(int conexion_memoria) {
	int valor;
	recv(conexion_memoria, &valor, sizeof(int), 0);
	return valor;
}
