#ifndef KERNEL_H_
#define KERNEL_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <string.h>
#include <commons/log.h>
#include <pthread.h>
#include <shared.h>

t_log* logger;

int atender_pedido(int);
int escuchar_conexiones();
void imprimir_valor(t_buffer*);
void imprimir_STRING(t_buffer*);
void imprimir_STRING_y_responder(t_buffer*);

#endif /* KERNEL_H_ */
