#include "../include/mmu.h"

int traducir_direccion(int direccion_logica, int tlb[][2], int tamanio) {
    int dir_fisica_tlb = obtener_dir_tlb(direccion_logica, tlb, tamanio);

    if(dir_fisica_tlb != -1) { //tlb devuelve -1 si no la tiene
        return dir_fisica_tlb;
    } else {
        return calcular_dir_fisica(direccion_logica);
    }
}

int calcular_dir_fisica(int direccion_logica) {
    return 5; //de prueba
}