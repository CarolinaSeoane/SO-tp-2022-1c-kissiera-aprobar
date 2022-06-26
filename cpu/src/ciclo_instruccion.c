#include "../include/ciclo_instruccion.h"

void ejecutar_ciclo_instruccion(Proceso_CPU* proceso) {

	int tamanio = config.ENTRADAS_TLB;
	int tlb[tamanio][3];

	inicializar_tlb(tlb, tamanio);
/*
	printear(tlb, tamanio);
	//esto es todo de prueba
	usleep(5);
	agregar_direccion(4,5, tlb, tamanio);
	printear(tlb, tamanio);
	
	obtener_dir_tlb(4, tlb, tamanio);
	obtener_dir_tlb(5, tlb, tamanio);
	obtener_dir_tlb(3, tlb, tamanio);
	usleep(5000000);	
	agregar_direccion(3,8, tlb, tamanio);
	printear(tlb, tamanio);
	usleep(5000000);		
	obtener_dir_tlb(3, tlb, tamanio);

	usleep(5000000);	
	agregar_direccion(9,6, tlb, tamanio);
	printear(tlb, tamanio);
	usleep(5000000);
	agregar_direccion(2,7, tlb, tamanio);
	printear(tlb, tamanio);

	usleep(5000000);
	obtener_dir_tlb(4, tlb, tamanio);
	printear(tlb, tamanio);

	usleep(5000000);
	agregar_direccion(5,0, tlb, tamanio);
	usleep(5000000);		
	agregar_direccion(1,1, tlb, tamanio);
	usleep(5000000);		
	agregar_direccion(3,3, tlb, tamanio);
	usleep(5000000);		
	agregar_direccion(7,7, tlb, tamanio);
	usleep(5000000);		
	agregar_direccion(6,6, tlb, tamanio);
	usleep(5000000);		
	agregar_direccion(2,3, tlb, tamanio);
	*/ //agrego los sleep para las pruebas porque sino ejecuta todo en el mismo instante. probar mejor cuando memoria devuelva resultados

    instruccion inst;
    uint32_t valor_copy;
	log_info(logger, "Ejecutando ciclos de instruccion del proceso %d", (*proceso).pid);
    while(!check_interrupt() && !check_syscall()) {
        fetch(proceso, &inst);
        //log_info(logger, "Fetch encontro la instruccion %d con PARAM1 = %d, PARAM2 = %d", inst.id_operacion, inst.operando1, inst.operando2);
        bool es_copy = decode(inst.id_operacion);
        
	    if (es_copy) {
			log_info(logger, "Proceso %d va a ejecutar COPY", (*proceso).pid); 
            valor_copy = fetch_operands(proceso, inst, tlb, tamanio); 
			log_info(logger, "El valor que se va a copiar es %d", valor_copy);
		}
        
        (*proceso).program_counter++;
        execute(proceso, inst, valor_copy, tlb, tamanio);
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

uint32_t fetch_operands(Proceso_CPU* proceso, instruccion inst, int tlb[][3], int tamanio) {
	send_pedido_lectura(proceso, inst.operando2, tlb, tamanio, (*proceso).pid);
	return recv_pedido();
}

void execute(Proceso_CPU* proceso, instruccion inst, int valor_copy, int tlb[][3], int tamanio) {
	
	switch(inst.id_operacion) {

		case NO_OP:
			log_info(logger, "Proceso %d ejecuta NO_OP", (*proceso).pid);
			usleep(config.RETARDO_NOOP * 1000);
			break;

		case IO:
			log_info(logger, "Proceso %d ejecuta IO", (*proceso).pid);
			flag_syscall = 1;
			send_proceso_bloqueado(proceso, inst.operando1);
			break;

		case READ:
			log_info(logger, "Proceso %d ejecuta READ", (*proceso).pid);
			send_pedido_lectura(proceso, inst.operando1, tlb, tamanio, (*proceso).pid);
			
			uint32_t leido = recv_pedido();
			log_info(logger, "El valor leido es %d\n", leido);

			break;

		case WRITE:
			log_info(logger, "Proceso %d ejecuta WRITE", (*proceso).pid);
			send_pedido_escritura(inst.operando1, inst.operando2, tlb, tamanio, (*proceso).pid);

			int operacion_exitosa = recv_pedido();
			if(!operacion_exitosa) {
				log_error(logger, "Error al tratar de escribir en memoria. Cerrando programa");
				exit(0);
			}

			break;

		case COPY: 
			log_info(logger, "Proceso %d ejecuta COPY", (*proceso).pid);
			send_pedido_escritura(inst.operando1, valor_copy, tlb, tamanio, (*proceso).pid);
			
			int escritura_exitosa = recv_pedido();
			if(!escritura_exitosa) {
				log_error(logger, "Error al tratar de escribir en memoria. Cerrando programa");
				exit(0);
			}

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
