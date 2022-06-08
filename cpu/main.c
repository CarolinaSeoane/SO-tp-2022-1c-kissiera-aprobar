#include "include/main.h"

int main() {

	inicializar_logger();
	inicializar_config();
	inicializar_semaforos();
	inicializar_flags();
	inicializar_conexiones();
	inicializar_servidores();

	// ------ RECIBE INTERRUPCIONES DE KERNEL
	pthread_t hilo_interrupt;
	cliente_interrupt = esperar_cliente(interrupt, logger);
	pthread_create(&hilo_interrupt, NULL, atender_interrupt, NULL);
	
	// ------ RECIBE PROCESOS DE KERNEL
	pthread_t hilo_dispatch;
	cliente_dispatch = esperar_cliente(dispatch, logger);
	pthread_create(&hilo_dispatch, NULL, atender_dispatch, NULL);

	// ------ ESPERA TERMINACION DE HILOS
	pthread_join(hilo_dispatch, NULL);
	pthread_join(hilo_interrupt, NULL);

	// ------ FINALIZACION
	destroy_recursos();

}