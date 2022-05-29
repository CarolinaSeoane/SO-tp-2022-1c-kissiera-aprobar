#ifndef KERNEL_MAIN_H_
#define KERNEL_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <readline/readline.h>
#include <string.h>
#include <commons/log.h>
#include "utils.h"
#include "pcb.h"
#include "pedidos.h"
#include "planificacion.h"
#include "serializacion.h"
#include <semaphore.h>
#include "../../shared/include/shared.h"

int server_escuchar(int kernel_server);


#endif /* KERNEL_H_ */
