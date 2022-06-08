#include "../include/conexiones_memoria.h"

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
