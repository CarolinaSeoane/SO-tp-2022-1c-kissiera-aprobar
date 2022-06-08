#ifndef SERIALIZACION_H_
#define SERIALIZACION_H_

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"
#include "utils.h"

void recv_proceso_init(int*, int*, int);
void send_cpu_handshake(void*);
void send_tabla_primer_nivel(int, int);

#endif