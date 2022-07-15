#ifndef SWAP_H_
#define SWAP_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../../shared/include/shared.h"
#include "utils.h"
#include <pthread.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>

void* atender_pedidos_swap();

#endif 