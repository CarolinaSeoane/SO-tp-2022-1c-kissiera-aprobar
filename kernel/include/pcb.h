#ifndef PCB_H_
#define PCB_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"

int ULT_PID;

int generar_pid();
void crear_pcb(PCB*, int, void*, int, int, int);

#endif 