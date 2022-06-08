#ifndef TLB_H_
#define TLB_H_

#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

int obtener_dir_tlb(int);
int esta_en_tlb(int);
int buscar_direccion_fisica(int, int);
void inicializar_tlb();
void agregar_direccion(int, int);
void printear();
void eliminar_tlb();
bool esta_llena();

int tlb[][2];
int* tamanio;

#endif /* TLB_H_ */