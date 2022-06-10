#ifndef TLB_H_
#define TLB_H_

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int obtener_dir_tlb(int, int tlb[][2], int);
int esta_en_tlb(int, int tlb[][2], int);
int buscar_direccion_fisica(int, int, int tlb[][2]);
void inicializar_tlb(int tlb[][2], int);
void agregar_direccion(int, int, int tlb[][2], int);
void printear(int tlb[][2], int);
bool esta_llena(int tlb[][2], int);
void reemplazo_fifo(int, int, int tlb[][2], int);
void reemplazo_lru(int, int, int tlb[][2], int);

#endif /* TLB_H_ */