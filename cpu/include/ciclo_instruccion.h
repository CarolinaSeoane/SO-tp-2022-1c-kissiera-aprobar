#ifndef CICLO_INSTRUCCION_H
#define CICLO_INSTRUCCION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include "utils.h"
#include "conexiones_kernel.h"
#include "conexiones_memoria.h"
#include <unistd.h>
#include "mmu.h"

void ejecutar_ciclo_instruccion(Proceso_CPU*, void*);
void fetch(Proceso_CPU*, instruccion*);
bool decode(int);
int fetch_operands(Proceso_CPU*, instruccion);
void execute(Proceso_CPU*, instruccion, int, void*);
int check_interrupt();
int check_syscall();

#endif 