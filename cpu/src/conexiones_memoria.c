#include "../include/conexiones_memoria.h"

// Faltaria la traduccion de dirección lógica a física
void send_pedido_lectura(Proceso_CPU* proceso, instruccion inst, int tlb[][3], int tamanio, int pid) {
	
	int direccion_logica = inst.operando1;

	int direccion_fisica = traducir_direccion(direccion_logica, tlb, tamanio, pid);

	int* codigo = malloc(sizeof(int));
	*codigo = READ_M;

	void* paquete = malloc(sizeof(int)*2);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(direccion_fisica), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*2, 0);

	free(codigo);
	free(paquete);

}

// Faltaria la traduccion de dirección lógica a física
void send_pedido_escritura(int direccion_logica, int valor, int tlb[][3], int tamanio, int pid) {

	int direccion_fisica = traducir_direccion(direccion_logica, tlb, tamanio, pid);

	int* codigo = malloc(sizeof(int));
	*codigo = WRITE_M;

	void* paquete = malloc(sizeof(int)*3);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(direccion_fisica), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(valor), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*3, 0);

	free(codigo);
	free(paquete);

}

int recv_pedido() {
	int valor;
	recv(conexion_memoria, &valor, sizeof(int), 0);
	return valor;
} //usamos el mismo recv para distintos pedidos

void send_pedido_tabla_segundo_nivel(int entrada_tabla_primer_nivel, int pid) {

	int* codigo = malloc(sizeof(int));
	*codigo = ENVIAR_TABLA_PRIMER_NIVEL;

	void* paquete = malloc(sizeof(int)*3);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(pid), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(entrada_tabla_primer_nivel), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*3, 0);

	free(codigo);
	free(paquete);

}

void send_pedido_marco(int direc_tabla_segundo_nivel, int entrada_tabla_segundo_nivel) {

	int* codigo = malloc(sizeof(int));
	*codigo = ENVIAR_TABLA_SEGUNDO_NIVEL;

	void* paquete = malloc(sizeof(int)*3);

	int offset = 0;
	memcpy(paquete, &(*codigo), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(direc_tabla_segundo_nivel), sizeof(int));
	offset += sizeof(int);
	memcpy(paquete + offset, &(entrada_tabla_segundo_nivel), sizeof(int));

	send(conexion_memoria, paquete, sizeof(int)*3, 0);

	free(codigo);
	free(paquete);

}
