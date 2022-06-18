#ifndef MANEJO_MEMORIA_H
#define MANEJO_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include "../../shared/include/shared.h"
#include "utils.h"
#include <math.h>

uint32_t asignar_memoria_y_estructuras(int, int);
void verificar_memoria();
void mostrar_lista_tablas_primer_nivel();
void solicitar_pagina_a_swap(int, int);
int cargar_pagina_en_memoria(int, void*);
void actualizar_tabla_de_paginas(int, int, int);

#endif