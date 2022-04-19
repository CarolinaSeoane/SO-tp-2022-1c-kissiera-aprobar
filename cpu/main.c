#include "include/main.h"

int main()
{
	// return 0;

}
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
