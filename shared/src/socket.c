#include "../include/socket.h"

int iniciar_servidor(char* ip, char* puerto, int cant_conexiones) {
    int socket_servidor;
    struct addrinfo hints, *servinfo;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, puerto, &hints, &servinfo);

    bool conecto = false;

    for (struct addrinfo *p = servinfo; p != NULL; p = p->ai_next) {
        socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (socket_servidor == -1)
            continue;

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        conecto = true;
        break;
    }

    if(!conecto) {
        free(servinfo);
        return 0;
    }

    listen(socket_servidor, cant_conexiones); // Escuchando cant_conexiones conexiones simultaneas

    freeaddrinfo(servinfo);

    return socket_servidor;
}

int esperar_cliente(int socket_servidor, t_log *logger)
{
	struct sockaddr_in dir_cliente; //no se que es esto
	unsigned int tam_direccion = sizeof(struct sockaddr_in);

    /* La llamada al sistema accept() es bloqueante. Es decir, el proceso servidor se quedará bloqueado en
     * accept hasta que le llegue un cliente.
     */

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);	
	log_info(logger, "Se conecto un cliente");
	return socket_cliente;
}

int crear_conexion(char *ip, char* puerto, t_log* logger) {

	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	// Error conectando
		if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1) {
			log_error(logger, "Error al conectarse con IP: %s y PUERTO: %s\n", ip, puerto);
			freeaddrinfo(server_info);
			return 0;
		} else
			log_info(logger, "Cliente conectado en IP: %s y PUERTO: %s\n", ip, puerto);

	freeaddrinfo(server_info);

	return socket_cliente;
}

void terminar_programa(int conexion, t_log* logger, t_config* config) {
	log_info(logger,"Conexión Terminada");
	free(logger);
	free(config);
	close(conexion);
}


void log_info_sh(t_log* logger, char* editor) {
	// log_info(logger,editor);
}


void log_warning_sh(t_log* logger, char* editor) {
	// log_warning(logger,editor);
}


int recibir_operacion(int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	if(recv(socket_cliente, &(paquete->codigo_operacion), sizeof(uint8_t), 0) != 0)
		return paquete->codigo_operacion;
	else
	{
		close(socket_cliente);
		return -1;
	}

}

