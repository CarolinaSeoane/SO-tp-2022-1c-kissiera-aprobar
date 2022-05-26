#include "../include/mmu.h"

void inicializar_tlb() {

    tamanio = malloc(sizeof(int));
    *tamanio = config.ENTRADAS_TLB;
    log_info(logger, "Inicializando TLB");
    log_info(logger, "entradas tlb es: %d", config.ENTRADAS_TLB);
    for (int i = 0; i < *tamanio; i++) {
        for (int j = 0; j < 2; j++) {
            tlb[i][j] = -1;     //inicializo los valores en -1
        }
    }

}

void agregar_direccion(int dir_logica, int dir_fisica) {

    if(esta_llena()) {
        log_info(logger, "TLB llena. Se procede a eliminar entradas anteriores");
        //llamar algoritmo de reemplazo
    } else {
        log_info(logger, "Insertando nueva entrada en TLB: %d %d", dir_logica, dir_fisica);

        for (int i = 0; i < *tamanio; i++) {
            if(tlb[i][0] == -1) {
                tlb[i][0] = dir_logica;
                tlb[i][1] = dir_fisica;
                i = 1000;
                break;
            }
        } 
    }
}

bool esta_llena() {
    for(int i = 0; i < *tamanio; i++) {
        if(tlb[i][0] == -1) {
            return false;
        }
    }
    return true;
}

void printear() {
    log_info(logger, "Comenzando printeo");

    for (int i = 0; i < *tamanio; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%d ", tlb[i][j]);
        }
    printf("\n");
    }
}

int obtener_dir_tlb(int direccion_logica) {
    int indice = esta_en_tlb(direccion_logica);
    if(indice != -1) {
        log_info(logger, "Entrada encontrada");
        return buscar_direccion_fisica(direccion_logica, indice);
    } else {
        log_info(logger, "Entrada no encontrada");
        return -1;
    }
}

int esta_en_tlb(int direccion_logica) {
    log_info(logger, "Buscando entrada %d en TLB...", direccion_logica);
    for (int i = 0; i < *tamanio; i++) {
        if(tlb[i][0] == direccion_logica) {
            return i;
        }
    } return -1;
}

int buscar_direccion_fisica(int direccion_logica, int indice) {
    log_info(logger, "La direccion logica %d corresponde a la direccion fisica %d", direccion_logica, tlb[indice][1]);
    return tlb[indice][1];
}

void eliminar_tlb() {
    log_info(logger, "Eliminando TLB");
    free(tamanio);
}