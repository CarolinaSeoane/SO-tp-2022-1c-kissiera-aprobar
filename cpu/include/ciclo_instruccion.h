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

void ejecutar_ciclo_instruccion(Proceso_CPU*);
void fetch(Proceso_CPU*, instruccion*);
bool decode(int);
uint32_t fetch_operands(Proceso_CPU*, instruccion, int tlb[][3], int);
void execute(Proceso_CPU*, instruccion, uint32_t, int tlb[][3], int);
int check_interrupt();
int check_syscall();

#endif 