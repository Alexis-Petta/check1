#include "../../utils/src/connection.h"
#include <commons/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "runCPU.c"

t_log* logger;

int main() {
    //start connections//
    RUN_CPU();
    
    return 0;
}
