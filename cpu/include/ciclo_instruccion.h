#ifndef CICLO_INSTRUCCION_H
#define CICLO_INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "utils.h"
#include <unistd.h>

void ejecutar_ciclo_instruccion(Proceso_CPU*, Config);
void fetch(Proceso_CPU*, instruccion*);
bool decode(int);
void fetch_operands(Proceso_CPU*);
void execute(Proceso_CPU*, instruccion, Config);

#endif 