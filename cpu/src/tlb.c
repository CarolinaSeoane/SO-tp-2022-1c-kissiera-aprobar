#include "../include/mmu.h"

void inicializar_tlb(int tlb[][3], int tamanio) {

    log_info(logger, "Inicializando TLB");
    for (int i = 0; i < tamanio; i++) {
        for (int j = 0; j < 3; j++) {
            tlb[i][j] = -1;     //inicializo los valores en -1
        }
    }
}

void agregar_direccion(int dir_logica, int dir_fisica, int tlb[][3], int tamanio) {

    if(esta_llena(tlb, tamanio)) {
        log_info(logger, "TLB llena. Se procede a eliminar entradas anteriores");

        if(!strcmp(config.REEMPLAZO_TLB, "FIFO")) {
            reemplazo_fifo(dir_logica, dir_fisica, tlb, tamanio);
        } else {
            reemplazo_lru(dir_logica, dir_fisica, tlb, tamanio);
        }

        log_info(logger, "Entrada %d %d insertada correctamente", dir_logica, dir_fisica);

    } else {
        log_info(logger, "Insertando nueva entrada en TLB: %d %d", dir_logica, dir_fisica);

        for (int i = 0; i < tamanio; i++) {
            if(tlb[i][0] == -1) {
                tlb[i][0] = dir_logica;
                tlb[i][1] = dir_fisica;
                tlb[i][2] = time(NULL);
                i = 1000;
                break;
            }
        } 
    }
}

bool esta_llena(int tlb[][3], int tamanio) {
    for(int i = 0; i < tamanio; i++) {
        if(tlb[i][0] == -1) {
            return false;
        }
    }
    return true;
}

void printear(int tlb[][3], int tamanio) {
    log_info(logger, "Comenzando printeo");

    for (int i = 0; i < tamanio; i++) {
        for (int j = 0; j < 3; j++) {
            printf("%d ", tlb[i][j]);
        }
    printf("\n");
    }
}

int obtener_dir_tlb(int direccion_logica, int tlb[][3], int tamanio) {
    int indice = esta_en_tlb(direccion_logica, tlb, tamanio);
    if(indice != -1) {
        log_info(logger, "Entrada encontrada");
        return buscar_direccion_fisica(direccion_logica, indice, tlb);
    } else {
        log_info(logger, "Entrada no encontrada");
        return -1;
    }
}

int esta_en_tlb(int direccion_logica, int tlb[][3], int tamanio) {
    log_info(logger, "Buscando entrada %d en TLB...", direccion_logica);
    for (int i = 0; i < tamanio; i++) {
        if(tlb[i][0] == direccion_logica) {
            return i;
        }
    } return -1;
}

int buscar_direccion_fisica(int direccion_logica, int indice, int tlb[][3]) {
    log_info(logger, "La direccion logica %d corresponde a la direccion fisica %d", direccion_logica, tlb[indice][1]);
    tlb[indice][2] = time(NULL); //le actualizo el tiempo porque la esta usando ahora
    log_info(logger, "ACTUALICE EL TIEMPO");
    log_info(logger, "");
    return tlb[indice][1];
}

void reemplazo_fifo(int dir_logica, int dir_fisica, int tlb[][3], int tamanio) {
    for (int i = 0; i < tamanio - 1; i++) {
        tlb[i][0] = tlb[i+1][0];
        tlb[i][1] = tlb[i+1][1];
    }

    tlb[tamanio-1][0] = dir_logica;
    tlb[tamanio-1][1] = dir_fisica;
    tlb[tamanio-1][2] = time(NULL); //no hace falta en fifo pero para que no quede mal el printeo. despues cambiar printeo

    printear(tlb, tamanio);
}

void reemplazo_lru(int dir_logica, int dir_fisica, int tlb[][3], int tamanio) {
    int menor = tlb[0][2];

    int indice = 0;
    for (int i = 0; i < tamanio; i++) {
        if(tlb[i][2] < menor) {
            menor = tlb[i][2];
            indice = i;
        }
    }
    tlb[indice][0] = dir_logica;
    tlb[indice][1] = dir_fisica;
    tlb[indice][2] = time(NULL);

    printear(tlb, tamanio);
}