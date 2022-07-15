#ifndef MMU_H_
#define MMU_H_

#include <stdio.h>
#include <stdlib.h>
#include "tlb.h"
#include "utils.h"
#include <math.h>
#include "conexiones_memoria.h"

int traducir_direccion(int, int tlb[][3], int, int);
void calcular_dir_fisica(int, int, int* , int*);

#endif /* MMU_H_ */