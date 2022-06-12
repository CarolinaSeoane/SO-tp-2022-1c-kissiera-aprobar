#ifndef MMU_H_
#define MMU_H_

#include <stdio.h>
#include <stdlib.h>
#include "tlb.h"

int traducir_direccion(int, int tlb[][3], int);
int calcular_dir_fisica(int);

#endif /* MMU_H_ */