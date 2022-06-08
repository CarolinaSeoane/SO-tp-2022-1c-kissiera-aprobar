#include "../include/funciones.h"

int devolver_cantidad_de_instrucciones(char* path){
	FILE* archivo = fopen(path, "r");
	char* contenido = malloc(sizeof(char*));
	int len = 0;
	int lines=0;
	int adic;
	while (getline(&contenido, &len, archivo) != -1)
	{

		adic=0;
		char** tokens_string = string_split(contenido, " ");
		if (!strcmp("NO_OP", tokens_string[0])){
			adic = atoi(tokens_string[1]);
			lines = lines + adic;

		}else{
			lines++;
		}
		string_array_destroy(tokens_string);

	}
	fclose(archivo);
	free(contenido);
	return lines;

}

int es_una_instruccion_valida(char* instruccion) {

	char** tokens_string = string_split(instruccion, " ");
	if(!strcmp("NO_OP", tokens_string[0]) || !strcmp("I/O", tokens_string[0]) || !strcmp("READ", tokens_string[0])) {

		for(int i=0; i<strlen(tokens_string[1]); i++) {
			if(!isdigit(tokens_string[1][i])){
				string_array_destroy(tokens_string);
				return 0;
			}
		}
		string_array_destroy(tokens_string);
		return 1;

	} else if(!strcmp("COPY", tokens_string[0]) || !strcmp("WRITE", tokens_string[0])) {

		if(tokens_string[1] == NULL) {
			string_array_destroy(tokens_string);
			return 0;
		}

		if(tokens_string[2] == NULL) {
			string_array_destroy(tokens_string);
			return 0;
		}

		for(int i=0; i<strlen(tokens_string[1]); i++) {
			if(!isdigit(tokens_string[1][i])){
				string_array_destroy(tokens_string);
				return 0;
			}
		}

		for(int i=0; i<strlen(tokens_string[2]); i++) {
			if(!isdigit(tokens_string[2][i])){
				string_array_destroy(tokens_string);
				return 0;
			}
		}
		string_array_destroy(tokens_string);
		return 1;

	} else {
		string_array_destroy(tokens_string);
		return !strcmp("EXIT\n", instruccion);

	}
}


int traer_identificador(char* codigo) {
	if(!strcmp("NO_OP", codigo)){
		return 0;
	} else if(!strcmp("I/O", codigo)) {
		return 1;
	} else if(!strcmp("READ", codigo)) {
		return 2;
	} else if(!strcmp("WRITE", codigo)) {
		return 3;
	} else if(!strcmp("COPY", codigo)) {
		return 4;
	} else {
		return 5;
	}
}

instruccion* armar_y_devolver_instruccion (char* linea_leida){

	instruccion* nuevo = malloc(sizeof(instruccion));

	char** tokens_string = string_split(linea_leida, " ");
    nuevo->id_operacion = traer_identificador(tokens_string[0]);
    if(!strcmp("NO_OP", tokens_string[0]) || !strcmp("I/O", tokens_string[0]) || !strcmp("READ", tokens_string[0])) {

    	nuevo->operando1 = atoi(tokens_string[1]);
    	nuevo->operando2 = 0;

    } else if (!strcmp("COPY", tokens_string[0]) || !strcmp("WRITE", tokens_string[0])) {

    	nuevo->operando1 = atoi(tokens_string[1]);
    	nuevo->operando2 = atoi(tokens_string[2]);

    } else {

    	nuevo->operando1 = 0;
    	nuevo->operando2 = 0;

    }
    string_array_destroy(tokens_string);
    return nuevo;
}

