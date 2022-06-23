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
void finalizar_estructuras_del_proceso_y_avisar_a_kernel(int,int);
void solicitar_pagina_a_swap(int, int);
int cargar_pagina_en_memoria(int, void*);
void actualizar_tabla_de_paginas(int, int, int);
void pedir_swap_in_a_swap(int);
bool comparator_orden_de_carga(void*, void*);
void ordenar_lista_con_paginas_cargadas_segun_orden_de_carga();
void generar_lista_de_paginas_cargadas_en_orden(int);


#endif