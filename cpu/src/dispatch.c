#include "../include/dispatch.h"


int fetch(Proceso_CPU* proceso) {

	int offset = proceso->program_counter * 12;
	int co_op;
	memcpy(&co_op, proceso->stream + offset, sizeof(int));
	log_info(logger, "Fetch encontro la instruccion: %d", co_op);
	return co_op;


}

operacion decode(int co_op) {
	
	switch(co_op) {

		case 0:
			return NO_OP;
			break;
		case 1:
			return IO;
			break;
		case 2:
			return READ;
			break;
		case 3:
			return WRITE;
			break;
		case 4:
			return COPY;
			break;
		case 5:
			return EXIT;
			break;
		default:
			log_error(logger, "Operacion desconocida");
	}

}

void fetch_operands(Proceso_CPU* proceso) {
	// TO DO
}

void execute(Proceso_CPU* proceso, operacion op) {

	switch(op) {

		case NO_OP:
			
			break;
		case IO:
			
			break;
		case READ:
			
			break;
		case WRITE:
			
			break;
		case COPY:
			
			break;
		case EXIT:
			
			break;
		default:
			log_error(logger, "Operacion desconocida");
	}
}

void mostrar_instrucciones(void* stream, int len_instrucciones){

	int offset=0;
	operacion id_operacion;
	uint32_t operando1;
	uint32_t operando2;

	for(int i=0; i<len_instrucciones; i++) {
		memcpy(&id_operacion, stream+offset, sizeof(operacion));
		offset+=sizeof(operacion);
		memcpy(&operando1, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		memcpy(&operando2, stream+offset, sizeof(uint32_t));
		offset+=sizeof(uint32_t);
		log_info(logger, "id_operacion: %d - operando1: %d - operando2: %d", id_operacion, operando1, operando2);
	}
	log_info(logger, "------------------ DONE ---------------\n");
} //dsp borrar


void recv_proceso(Proceso_CPU* proceso, args_dispatch* args) {

	recv(args->cliente_fd, &(proceso->pid), sizeof(int), 0);

	recv(args->cliente_fd, &(proceso->program_counter), sizeof(int), 0);

	recv(args->cliente_fd, &(proceso->tabla_paginas), sizeof(int), 0);

	int tam_stream;
	recv(args->cliente_fd, &tam_stream, sizeof(int), 0);

	proceso->stream = malloc(tam_stream);
	recv(args->cliente_fd, proceso->stream, tam_stream, 0);
	mostrar_instrucciones(proceso->stream, tam_stream/sizeof(instruccion));

}

void* atender_dispatch(void* void_args) {
	args_dispatch* args = (args_dispatch*) void_args;
	
	while(args->cliente_fd != -1) {
		int co_op;
		recv(args->cliente_fd, &co_op, sizeof(co_op), 0);
		switch(co_op) {
			case EXEC_PROCESO:
				log_info(logger, "Voy a recibir un proceso para ejecutar");
                Proceso_CPU proceso;
				recv_proceso(&proceso, args);
                log_info(logger, "Proceso recibido: PDI es %d - PC: %d - Tabla de páginas: %d\n\n", proceso.pid, proceso.program_counter, proceso.tabla_paginas);
				int co_op = fetch(&proceso);
				operacion op = decode(co_op);
				if (op == COPY) { 
					fetch_operands(&proceso); // Para que saque el segundo operando y se comunique con memoria
				}
				proceso.program_counter = proceso.program_counter + 1; //Puede que no vaya aca
				execute(&proceso, op);
				//check_interrupt();
				//free(proceso.stream);
				break;
			case BLOCK_PROCESO:
				break;
			default:
				break;
		}	
	}
	free(args);
}
