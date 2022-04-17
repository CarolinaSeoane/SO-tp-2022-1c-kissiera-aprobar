#include "socket.h"
#include "serializador.h"
#include <string.h>
#include <readline/readline.h>



int iniciar_servidor_del_modulo(char* modulo)
{
	t_config* config_file = leer_config();
	char* ip = config_get_string_value(config_file, "IP");
	char* port_name = "PUERTO_CONSOLA";
	if (strcmp(modulo,"kernel") == 0)
	{
		port_name = "PUERTO_KERNEL";
	}
	if (strcmp(modulo,"cpu") == 0)
	{
		port_name = "PUERTO_CPU";
	}
	char * puerto = config_get_string_value(config_file, port_name);
	return iniciar_servidor(ip, puerto);
}

int crear_conexion_al_modulo(char* modulo)
{
	t_config* config_file = leer_config();
	char* ip = config_get_string_value(config_file, "IP");
	char* port_name = "PUERTO_CONSOLA";
	if (strcmp(modulo,"kernel") == 0)
	{
		port_name = "PUERTO_KERNEL";
	}
	if (strcmp(modulo,"cpu") == 0)
	{
		port_name = "PUERTO_CPU";
	}
	char * puerto = config_get_string_value(config_file, port_name);
	return crear_conexion(ip, puerto);
}


t_log* iniciar_logger(char* modulo)
{
	return log_create(modulo,"TP Integrador",1,0);
}


t_config* leer_config(void)
{
	return config_create("/home/utnso/workspace/tp-2022-1c-kissiera-aprobar/shared/src/conexiones.config");

}


int esperar_cliente(int socket_servidor)
{
	struct sockaddr_in dir_cliente;
	unsigned int tam_direccion = sizeof(struct sockaddr_in);

	int socket_cliente = accept(socket_servidor, (void*) &dir_cliente, &tam_direccion);

	printf("Se conecto un Cliente\n");

	return socket_cliente;
}

int crear_conexion(char *ip, char *port)
{
	struct addrinfo hints;
	struct addrinfo *server_info;


	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, port, &hints, &server_info);

	int socket_cliente = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);

	if(connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1)
		{
			freeaddrinfo(server_info);
			free(port);
			return -1;
		}


	freeaddrinfo(server_info);
	free(port);

	return socket_cliente;
}


int iniciar_servidor(char* ip, char* port) {
	int socket_servidor;
    struct addrinfo hints, *servinfo, *p;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(ip, port, &hints, &servinfo);

    for (p=servinfo; p != NULL; p = p->ai_next)
    {
        if ((socket_servidor = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;

        int activado = 1;
        setsockopt(socket_servidor,SOL_SOCKET,SO_REUSEADDR,&activado,sizeof(activado));

        if (bind(socket_servidor, p->ai_addr, p->ai_addrlen) == -1) {
            close(socket_servidor);
            continue;
        }
        break;
    }
	printf("Servidor corriendo correctamente\n");
	listen(socket_servidor, SOMAXCONN);

    freeaddrinfo(servinfo);

    //log_trace(logger, "Listo para escuchar a mi cliente");

    return socket_servidor;
}




void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}


void terminar_programa(int conexion, t_log* logger, t_config* config)
{

	log_info(logger,"Conexión Terminada");
	free(logger);
	free(config);
	liberar_conexion(conexion);

}


void log_info_sh(t_log* logger, char* editor)
{
	log_info(logger,editor);
}


void log_warning_sh(t_log* logger, char* editor)
{
	log_warning(logger,editor);
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

