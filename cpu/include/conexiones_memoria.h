#ifndef CONEXIONES_MEMORIA_H
#define CONEXIONES_MEMORIA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>
#include <commons/config.h>
#include <commons/log.h>
#include "utils.h"
#include "dispatch.h"
#include "interrupt.h"
#include <pthread.h>

void send_handshake(int);
int recv_handshake(int);

#endif /* CONEXIONES_MEMORIA_H_ */
