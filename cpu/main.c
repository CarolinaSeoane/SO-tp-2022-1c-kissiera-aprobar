#include "include/main.h"

int main() {
	logger = log_create("cpu.log", "Cpu", 1, LOG_LEVEL_DEBUG);
	Config config;
	cargarConfig("cpu.config", &config);

	int dispatch = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_DISPATCH, 1);
	int interrupt = iniciar_servidor("127.0.0.1", config.PUERTO_ESCUCHA_INTERRUPT, 1);

	if(!dispatch || !interrupt) {
		log_info(logger, "Error al iniciar la conexión dispatch o interrupt\nCerrando el programa");
		return 1;
	}

	log_info(logger, "CPU esperando conexión de Kernel");

	/*	Hilos para conexiones dispatch e interrupt	*/

	pthread_t hilo_atender_conexiones;

	int cliente_dispatch = esperar_cliente(dispatch, logger);

	args_dispatch *args_d = malloc(sizeof(args_dispatch));
    args_d->cliente_fd = cliente_dispatch;

	pthread_create( &hilo_atender_conexiones, NULL, atender_dispatch, (void*) args_d);
	pthread_join(hilo_atender_conexiones, NULL);

	int cliente_interrupt = esperar_cliente(interrupt, logger);
	pthread_create(&hilo_atender_conexiones, NULL, atender_interrupt, NULL);
	pthread_join(hilo_atender_conexiones, NULL);

	/*	--------------------------------------		*/

	int conexion_memoria = crear_conexion(config.IP_MEMORIA, config.PUERTO_MEMORIA, logger);
	
	// return 0;

}
/*	recv(args->cliente_fd, &pcb, sizeof(pcb), 0);
	printf("Recibo PCB: %d\n", len_instrucciones);
	recv(args->cliente_fd, &len_instrucciones, sizeof(int), 0);
	printf("Recibo Cantidad Instrucciones: %d\n", len_instrucciones);
	recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);
	printf("Recibo Tamaño del proceso: %d\n", tamanio_proceso);
	recv(args->cliente_fd, &tamanio_proceso, sizeof(int), 0);
	printf("Recibo Tamaño del proceso: %d\n", );
				void* stream = malloc(len_instrucciones*sizeof(instruccion));
				printf("Cantidad reservada para stream %d\n", len_instrucciones*sizeof(instruccion));
				recv(args->cliente_fd, stream, len_instrucciones*sizeof(instruccion), 0);
*/



/*
void inicializar_colas()
{
	cola_exit = list_create();
	cola_io = list_create();
	cola_sabotaje = list_create();

	pthread_mutex_init(&mx_ready, NULL);
	pthread_mutex_init(&mx_io, NULL);
	pthread_mutex_init(&mx_exit, NULL);
	pthread_mutex_init(&mx_ciclo, NULL);

	sem_init(&sem_planificador_trip,0,0);


}


void ejecutar_tarea_fifo(t_tripulante tripulante)
{

	sem_wait(&sem_planificador_trip);

	for (int i = 0; i < (tripulante.tcb.rafaga); i++)
	{
		pthread_mutex_lock(&mx_ciclo);

		tripulante.tcb.ciclo_actual++;

		pthread_mutex_unlock(&mx_ciclo);

		//sleep(retardo_cpu); //data race?

	}


}


void mandar_tripulante_a_ready(t_tripulante tripulante)
{
	pthread_mutex_lock(&mx_ready);
	list_add(cola_ready,&tripulante);
	pthread_mutex_unlock(&mx_ready);
	sem_post(&sem_planificador_trip);
}


void mandar_tripulante_a_io(t_tripulante tripulante)
{
	pthread_mutex_lock(&mx_io);
	list_add(cola_ready,&tripulante);
	pthread_mutex_unlock(&mx_io);
}


void mandar_tripulante_a_exit(t_tripulante tripulante)
{
	pthread_mutex_lock(&mx_exit);
	list_add(cola_ready,&tripulante);
	pthread_mutex_unlock(&mx_exit);

}*/
