#include <connection.h>
#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

//function prototypes
t_config* load_config();
char* search_port(t_config* config, char* PORT_KEY);
char search_ip(t_config* config);