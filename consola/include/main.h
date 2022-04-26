#ifndef CONSOLA_H_
#define CONSOLA_H_
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <string.h>
#include <commons/log.h>
#include <pthread.h>
#include "utils.h"
/*

void  mostrar_menu();
int atender_conexion(int);
int recibir_operacion_new(int);
void leer_consola(t_log*);
void obtener_bitacora(int ID_TRIPULANTE);
void pausar_planificacion();
void iniciar_planificacion();
void leer_consola(t_log*);
void mostrar_menu();
void* menu_principal();
void enviar_un_id(t_log*,uint32_t);
void enviar_un_string(t_log*, char*);
void enviar_un_string_y_recibir_respuesta(t_log*, char*, int);*/

instruccion* armar_y_devolver_instruccion (char*);
int devolver_cantidad_de_instrucciones(char*);
int es_una_instruccion_valida(char*);
int traer_identificador(char* codigo);

#endif /* CONSOLA_H_ */
