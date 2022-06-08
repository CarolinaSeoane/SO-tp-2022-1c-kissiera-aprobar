#include "../include/ciclo_instruccion.h"

void ejecutar_ciclo_instruccion(Proceso_CPU* proceso) {

/*	inicializar_tlb();
	printear();
	//esto es todo de prueba
	agregar_direccion(4,5);
	printear();
	
	obtener_dir_tlb(4);
	obtener_dir_tlb(5);
	obtener_dir_tlb(3);
	agregar_direccion(3,8);
	printear();
	obtener_dir_tlb(3);

	agregar_direccion(9,6);
	printear();
	agregar_direccion(2,7);
	printear();

	agregar_direccion(5,9);
	printear();
	
	eliminar_tlb();
*/
    instruccion inst;
    int valor_copy;
	log_info(logger, "Ejecutando ciclos de instruccion del proceso %d", (*proceso).pid);
    while(!check_interrupt() && !check_syscall()) {
        fetch(proceso, &inst);
        //log_info(logger, "Fetch encontro la instruccion %d con PARAM1 = %d, PARAM2 = %d", inst.id_operacion, inst.operando1, inst.operando2);
        bool es_copy = decode(inst.id_operacion);
        
	    if (es_copy) { 
            valor_copy = fetch_operands(proceso, inst); // Para que saque el segundo operando y se comunique con memoria
        }
        
        (*proceso).program_counter++;
        execute(proceso, inst, valor_copy);
    }
	if(!flag_syscall) {
		log_info(logger, "El proceso %d es desalojado", (*proceso).pid);
		send_proceso_desalojado(proceso);
	}
	pthread_mutex_lock(&mutex_flag_interrupcion);
	flag_interrupcion = 0;
	pthread_mutex_unlock(&mutex_flag_interrupcion);
	log_info(logger, "Cambie el flag a %d",flag_interrupcion);
	flag_syscall = 0;
}

/* Fetch: buscar la próxima instrucción a ejecutar con el PC */
void fetch(Proceso_CPU* proceso, instruccion* inst) {
	int offset = (*proceso).program_counter * sizeof(instruccion);
	memcpy(inst, proceso->stream + offset, sizeof(instruccion));
}

/* Decode: interpretar que instrucción es la que se va a ejecutar para ver si se necesita fetch operands.*/ 
bool decode(int co_op) {
    return co_op == COPY;
}

int fetch_operands(Proceso_CPU* proceso, instruccion inst) {
	//send_pedido_lectura(proceso, inst, conexion_memoria);
	return 1; //recv_pedido_lectura(conexion_memoria);
}

void execute(Proceso_CPU* proceso, instruccion inst, int valor_copy) {
	
	switch(inst.id_operacion) {

		case NO_OP:
			log_info(logger, "Proceso %d ejecuta NO_OP", (*proceso).pid);
			usleep(config.RETARDO_NOOP * 1000);
			break;
		case IO:;
			log_info(logger, "Proceso %d ejecuta IO", (*proceso).pid);
			flag_syscall = 1;
			send_proceso_bloqueado(proceso, inst.operando1);
			/* aca solo hace falta devolverle el pid, pc y el bloqueo que lei recien. 
			dsp en kernel hay que buscar el proceso con ese pid, actualizarle el pc 
			y mandarlo a bloqueo */
			break;
		case READ:;
			log_info(logger, "Proceso %d ejecuta READ", (*proceso).pid);
			//send_pedido_lectura(proceso, inst, conexion_memoria);
			
			//int leido = recv_pedido_lectura(conexion_memoria);
			
			//log_info(logger, "El valor leido es %d", leido);
			break;
		case WRITE:
			log_info(logger, "Proceso %d ejecuta WRITE", (*proceso).pid);
			//send_pedido_escritura(inst.operando1, inst.operando2, conexion_memoria);
			break;
		case COPY:
			log_info(logger, "Proceso %d ejecuta COPY", (*proceso).pid);
			// send_pedido_escritura(inst.operando1, valor_copy, conexion_memoria);
			break;
		case EXIT:
			log_info(logger, "Proceso %d ejecuta EXIT", (*proceso).pid);
			flag_syscall = 1;
			send_proceso_finalizado(proceso);
			break;
		default:
			log_error(logger, "Operacion desconocida");
	}
}

int check_interrupt() {
	pthread_mutex_lock(&mutex_flag_interrupcion);
	int flag = flag_interrupcion;
	pthread_mutex_unlock(&mutex_flag_interrupcion);
	return flag;
}

int check_syscall() {
	return flag_syscall;
}