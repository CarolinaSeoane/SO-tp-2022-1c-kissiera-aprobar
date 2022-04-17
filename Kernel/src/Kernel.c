#include "Kernel.h"
#include <shared.h>

int main(void)
{
	escuchar_conexiones();

}

int atender_pedido(int cliente_fd)
{

	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->buffer = malloc(sizeof(t_buffer));

	// Primero recibimos el codigo de operacion
	recv(cliente_fd, &(paquete->codigo_operacion), sizeof(int), 0);
	printf("Operation: %d\n", paquete->codigo_operacion);

	// Recibo socket
	recv(cliente_fd, &(paquete->socket), sizeof(int), 0);
	printf("Socket: %d\n", paquete->socket);

	// Recibo modulo
	recv(cliente_fd, &(paquete->modulo), sizeof(int), 0);
	printf("Module: %d\n", paquete->modulo);

	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(cliente_fd, &(paquete->buffer->size), sizeof(uint32_t), 0);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	recv(cliente_fd, paquete->buffer->stream, paquete->buffer->size, 0);


	switch(paquete->codigo_operacion)
	{
		case ENVIAR_ID:
			printf("Me llegó una opción de expulsar tripulante\n");
			imprimir_valor(paquete->buffer);
			return EXIT_SUCCESS;
			break;
		case STR_SIN_RESPUESTA:
			imprimir_STRING(paquete->buffer);
			return EXIT_SUCCESS;
			break;
		case STR_CON_RESPUESTA:
			imprimir_STRING_y_responder(paquete->buffer);
			return EXIT_SUCCESS;
			break;
		default:
			log_warning_sh(logger, "Operacion desconocida.");
			break;
	}

	close(cliente_fd);
	return EXIT_SUCCESS;
}

int escuchar_conexiones()
{
	char* modulo = "Kernel.log";
	iniciar_logger(modulo);

	int server_fd = iniciar_servidor_del_modulo("kernel");

	while(1)
	{
		int cliente_fd = esperar_cliente(server_fd);
		pthread_t hilo_atender_pedido;
		pthread_create( &hilo_atender_pedido, NULL,atender_pedido,cliente_fd);
		pthread_join(hilo_atender_pedido,NULL);

	}
}


void imprimir_valor(t_buffer* buffer)
{

	void* stream = buffer->stream;
	int variable_rara;
	memcpy(&variable_rara, stream, sizeof(uint32_t));
	printf("el dato enviado y desempaquetado es: %d\n", variable_rara);
	
}


void imprimir_STRING(t_buffer* buffer)
{

	void* stream = buffer->stream;
	uint32_t tamanio;
	memcpy(&tamanio, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	char *palabra = malloc(tamanio+1);
	memcpy(palabra, stream, tamanio);
	printf("el dato enviado y desempaquetado es: %s\n", palabra);

}

void imprimir_STRING_y_responder(t_buffer* buffer)
{

	void* stream = buffer->stream;
	uint32_t tamanio;
	memcpy(&tamanio, stream, sizeof(uint32_t));
	stream += sizeof(uint32_t);
	char *palabra = malloc(tamanio+1);
	memcpy(palabra, stream, tamanio);
	printf("el dato enviado y desempaquetado es: %s\n", palabra);

	// Preparar para responder
	char *palabra2 = malloc(sizeof(char));
	printf("Inserte una respuesta para recibir en Consola: \n");
	scanf("%[^\n]%*c", palabra2);

	t_buffer* buffer2 = malloc(sizeof(t_buffer));
	buffer2->size = sizeof(uint32_t) + strlen(palabra2) + 1;
	void* stream2 = malloc(buffer2->size);

	int offset=0;
	uint32_t tamanio_palabra =  strlen(palabra2);
	memcpy(stream2, &tamanio_palabra, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream2 + offset, palabra2, strlen(palabra2) + 1);

	buffer2->stream = stream2;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	int conexion = crear_conexion_al_modulo("consola");


	paquete->codigo_operacion= STR_CON_RESPUESTA ;
	paquete->socket = conexion;
	paquete->modulo = KERNEL;
	paquete->buffer=buffer2;


	void* a_enviar = malloc(buffer2->size + sizeof(int)*3 + sizeof(uint32_t));
	int offset2 = 0;


	memcpy(a_enviar + offset2, &(paquete->codigo_operacion), sizeof(int));
	offset2 += sizeof(int);
	memcpy(a_enviar + offset2, &(paquete->socket), sizeof(int));
	offset2 += sizeof(int);
	memcpy(a_enviar + offset2, &(paquete->modulo), sizeof(int));
	offset2 += sizeof(int);
	memcpy(a_enviar + offset2, &(paquete->buffer->size), sizeof(uint32_t));
	offset2 += sizeof(uint32_t);
	memcpy(a_enviar + offset2, paquete->buffer->stream, paquete->buffer->size);

	// Por último enviamos//
	send(conexion, a_enviar, buffer2->size + sizeof(int)*3 + sizeof(uint32_t), 0);
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);

}



