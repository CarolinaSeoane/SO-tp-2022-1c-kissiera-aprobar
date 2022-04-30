#ifndef DISPATCH_H_
#define DISPATCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "../../shared/include/shared.h"
#include "utils.h"

void atender_dispatch(void*);
void recv_proceso(Proceso_CPU*, args_dispatch*);

#endif
