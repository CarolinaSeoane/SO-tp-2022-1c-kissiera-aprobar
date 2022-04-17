#ifndef SOCKET_H_
#define SOCKET_H_

#include<stdio.h>
#include<stdlib.h>
#include<commons/log.h>
#include<commons/string.h>
#include<commons/config.h>
#include<readline/readline.h>
#include<string.h>
#include<commons/log.h>


t_log* iniciar_logger(char*);
t_config* leer_config(void);
void terminar_programa(int, t_log*, t_config*);
int recibir_operacion(int);
int crear_conexion(char *, char *);
int esperar_cliente(int);
void liberar_conexion();
void log_info_sh();
void log_warning_sh();
int iniciar_servidor(char*, char*);
int crear_conexion_al_modulo(char*);
int iniciar_servidor_del_modulo(char*);


#endif
