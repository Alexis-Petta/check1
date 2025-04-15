#include "config.h"

t_config* load_config()
{
    t_config* config = config_create("ipPort.config");

    if (config == NULL)
    {
        printf("No se pudo cargar el archivo de configuración.\n");
        return NULL;
    }

    return config;
}

char* search_port(t_config* config, char* PORT_KEY)
{
    return config_get_string_value(config, PORT_KEY);
}

char search_ip(t_config* config)
{
    return config_get_string_value (config, "SERVER_IP");
}
