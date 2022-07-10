#include "../include/mmu.h"

void inicializar_tlb(int tlb[][3], int tamanio) {

    log_info(logger, "Inicializando TLB");
    for (int i = 0; i < tamanio; i++) {
        for (int j = 0; j < 3; j++) {
            tlb[i][j] = -1;     //inicializo los valores en -1
        }
    }
}

void agregar_direccion(int pagina, int marco, int tlb[][3], int tamanio) {

    if(esta_llena(tlb, tamanio)) {
        log_info(logger, "TLB llena. Se procede a eliminar entradas anteriores");

        if(!strcmp(config.REEMPLAZO_TLB, "FIFO")) {
            reemplazo_fifo(pagina, marco, tlb, tamanio);
        } else {
            reemplazo_lru(pagina, marco, tlb, tamanio);
        }

        log_info(logger, "Entrada %d %d insertada correctamente", pagina, marco);

    } else {
        log_info(logger, "Insertando nueva entrada en TLB: %d %d", pagina, marco);

        for (int i = 0; i < tamanio; i++) {
            if(tlb[i][0] == -1) {
                tlb[i][0] = pagina;
                tlb[i][1] = marco;
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

    log_info(logger, "\tL | F | T ");
    for (int i = 0; i < tamanio; i++) {
        if (tlb[i][0] == -1) {
            log_info(logger, "\t- | - | -");
        } else {
            log_info(logger, "\t%d | %d | %d", tlb[i][0], tlb[i][1], tlb[i][2]);
        }
    }
}

int buscar_entrada_en_tlb(int pagina, int tlb[][3], int tamanio) {
    int indice = esta_en_tlb(pagina, tlb, tamanio);
    if(indice != -1) {
        log_info(logger, "Entrada encontrada");
        return obtener_marco_asociado(pagina, indice, tlb);
    } else {
        log_info(logger, "Entrada no encontrada");
        return -1;
    }
}

int esta_en_tlb(int pagina, int tlb[][3], int tamanio) {
    log_info(logger, "Buscando pagina %d en TLB...", pagina);
    for (int i = 0; i < tamanio; i++) {
        if(tlb[i][0] == pagina) {
            return i;
        }
    } return -1;
}

int obtener_marco_asociado(int pagina, int indice, int tlb[][3]) {
    log_info(logger, "La pagina %d esta en el marco %d", pagina, tlb[indice][1]);
    tlb[indice][2] = time(NULL); //le actualizo el tiempo porque la esta usando ahora
    return tlb[indice][1];
}

void reemplazo_fifo(int pagina, int marco, int tlb[][3], int tamanio) {
    for (int i = 0; i < tamanio - 1; i++) {
        tlb[i][0] = tlb[i+1][0];
        tlb[i][1] = tlb[i+1][1];
    }

    tlb[tamanio-1][0] = pagina;
    tlb[tamanio-1][1] = marco;
    tlb[tamanio-1][2] = time(NULL); //no hace falta en fifo pero para que no quede mal el printeo. despues cambiar printeo

    printear(tlb, tamanio);
}

void reemplazo_lru(int pagina, int marco, int tlb[][3], int tamanio) {
    int menor = tlb[0][2];

    int indice = 0;
    for (int i = 0; i < tamanio; i++) {
        if(tlb[i][2] < menor) {
            menor = tlb[i][2];
            indice = i;
        }
    }
    tlb[indice][0] = pagina;
    tlb[indice][1] = marco;
    tlb[indice][2] = time(NULL);

    printear(tlb, tamanio);
}