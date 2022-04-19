#include "include/main.h"

int main(void)
{
/*
	pthread_t hilo_menu;
	pthread_create( &hilo_menu, NULL,menu_principal,NULL);
	pthread_join(hilo_menu,NULL);
	exit(0);
*/
}
/*
void* menu_principal(){
		t_log* logger;
		char* modulo = "Consola";
		logger = iniciar_logger(modulo);
		mostrar_menu();
		leer_consola(logger);
		return NULL ;
}

void mostrar_menu(){

	printf("1-Opción enviar un id.\n");
	printf("2-Opción enviar un string sin respuesta.\n");
	printf("3-Opción enviar un string con respuesta.\n");

}

void leer_consola(t_log* logger)
{

	int leido;
	char *leido_string;
	char *mensaje = malloc(sizeof(char));
	leido_string = readline(">");
	leido = atoi(leido_string);

		while(leido != 0)
		{
				switch(leido)
				{
						case 1:
							log_info_sh(logger, "Opción enviar un id inresado por pantalla");
							int valor;
							printf("Ingrese el ID a enviar:");
							scanf("%d", &valor);
							enviar_un_id(logger,valor);
							mostrar_menu();
							break;
						case 2:
							log_info_sh(logger, "Opción enviar un string sin respuesta.");
							printf("Ingrese el valor a enviar:");
							scanf("%[^\n]%*c", mensaje);
							enviar_un_string(logger,mensaje);
							mostrar_menu();
							break;
						case 3:
							log_info_sh(logger, "Opción enviar un string con respuesta.");
							int server_fd = iniciar_servidor_del_modulo("consola");
							printf("Ingrese el valor a enviar:");
							scanf("%[^\n]%*c", mensaje);
							enviar_un_string_y_recibir_respuesta(logger,mensaje, server_fd);
							close(server_fd);
							mostrar_menu();
							break;

						default:
							log_warning_sh(logger, "Opción no codeada. Volver a intentar más tarde.");
							break;

				}

			leido_string = readline(">");
			leido = atoi(leido_string);

		}


	free(leido_string);
	free(mensaje);


}

void enviar_un_id(t_log* logger,uint32_t id)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t); // id_triuplante
	void* stream = malloc(buffer->size);

	memcpy(stream,&id, sizeof(uint32_t));

	buffer->stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));
	int conexion = crear_conexion_al_modulo("kernel");


	paquete->codigo_operacion= ENVIAR_ID;
	paquete->socket = conexion;
	paquete->modulo = CONSOLA;
	paquete->buffer=buffer;


	void* a_enviar = malloc(buffer->size + sizeof(int)*3 + sizeof(uint32_t));
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


	// Por último enviamos//No estoy segundo del tamaño del send()
	send(conexion, a_enviar, buffer->size + sizeof(int)*3 + sizeof(uint32_t), 0);

	// No nos olvidamos de liberar la memoria que ya no usaremos
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void enviar_un_string(t_log* logger,char* palabra)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) + strlen(palabra) + 1;
	void* stream = malloc(buffer->size);

	int offset=0;
	uint32_t tamanio_palabra =  strlen(palabra);
	memcpy(stream, &tamanio_palabra, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, palabra, strlen(palabra) + 1);

	buffer->stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	int conexion = crear_conexion_al_modulo("kernel");


	paquete->codigo_operacion= STR_SIN_RESPUESTA;
	paquete->socket = conexion;
	paquete->modulo = CONSOLA;
	paquete->buffer=buffer;


	void* a_enviar = malloc(buffer->size + sizeof(int)*3 + sizeof(uint32_t));
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

	// Por último enviamos
	send(conexion, a_enviar, buffer->size + sizeof(int)*3 + sizeof(uint32_t), 0);

	// No nos olvidamos de liberar la memoria que ya no usaremos
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	close(conexion);

}

void enviar_un_string_y_recibir_respuesta(t_log* logger,char* palabra, int server_fd)
{
	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(uint32_t) + strlen(palabra) + 1;
	void* stream = malloc(buffer->size);

	int offset=0;
	uint32_t tamanio_palabra =  strlen(palabra);
	memcpy(stream, &tamanio_palabra, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(stream + offset, palabra, strlen(palabra) + 1);

	buffer->stream = stream;

	t_paquete* paquete = malloc(sizeof(t_paquete));

	int conexion = crear_conexion_al_modulo("kernel");
	paquete->codigo_operacion= STR_CON_RESPUESTA;
	paquete->socket = conexion;
	paquete->modulo = CONSOLA;
	paquete->buffer=buffer;


	void* a_enviar = malloc(buffer->size + sizeof(int)*3 + sizeof(uint32_t));
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

	// Por último enviamos
	send(conexion, a_enviar, buffer->size + sizeof(int)*3 + sizeof(uint32_t), 0);

	// No nos olvidamos de liberar la memoria que ya no usaremos
	free(a_enviar);
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
	close(conexion);

	///////////// Deserializar /////////////////

	// Levanto server para escuchar respuesta
	//int server_fd = iniciar_servidor_consola();
	// Espero cliente
	int cliente_fd = esperar_cliente(server_fd);

	t_paquete* paquete2 = malloc(sizeof(t_paquete));
	paquete2->buffer = malloc(sizeof(t_buffer));

	// Primero recibimos el codigo de operacion
	recv(cliente_fd, &(paquete2->codigo_operacion), sizeof(int), 0);
	printf("Operation: %d\n", paquete2->codigo_operacion);

	// Recibo socket
	recv(cliente_fd, &(paquete2->socket), sizeof(int), 0);
	printf("Socket: %d\n", paquete2->socket);

	// Recibo modulo
	recv(cliente_fd, &(paquete2->modulo), sizeof(int), 0);
	printf("Module: %d\n", paquete2->modulo);

	// Después ya podemos recibir el buffer. Primero su tamaño seguido del contenido
	recv(cliente_fd, &(paquete2->buffer->size), sizeof(uint32_t), 0);
	paquete2->buffer->stream = malloc(paquete2->buffer->size);
	recv(cliente_fd, paquete2->buffer->stream, paquete2->buffer->size, 0);

	void* stream2 = paquete2->buffer->stream;
	uint32_t tamanio;
	memcpy(&tamanio, stream2, sizeof(uint32_t));
	stream2 += sizeof(uint32_t);
	char *palabra2 = malloc(tamanio+1);
	memcpy(palabra2, stream2, tamanio);
	printf("%s\n", palabra2);
	close(cliente_fd);

}*/


