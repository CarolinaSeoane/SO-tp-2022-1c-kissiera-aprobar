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

instruccion* armar_y_devolver_instruccion (char*);
int devolver_cantidad_de_instrucciones(char*);
int es_una_instruccion_valida(char*);
int traer_identificador(char* codigo);

#endif /* CONSOLA_H_ */
