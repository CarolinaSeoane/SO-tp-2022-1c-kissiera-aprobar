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
int solicitar_pagina_a_swap(int, int);
int cargar_pagina_en_memoria(int);
void actualizar_tabla_de_paginas(int, int, int);
void generar_lista_de_paginas_cargadas(int);
void actualizar_bit_modificado(int, int);
int buscar_index_puntero_para_aplicar_algoritmo();
int aplicar_algoritmo_de_sustitucion_clock();
int aplicar_algoritmo_de_sustitucion_clock_modificado();
void eliminar_archivo_swap(int);


#endif