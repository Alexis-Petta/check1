#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>  
#include <unistd.h>

void create_connection();
void close_connection(int fd_conexion);

int main(int argc, char* argv[]) {
    create_connection();
    return 0;
}

void create_connection()
{
    int err;
    struct addrinfo hints, *server_info;

    // Inicializar la estructura hints a cero
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    // Obtener la dirección del servidor
    err = getaddrinfo("COMPLETAR", "COMPLETAR", &hints, &server_info);

    if (err != 0) {
        fprintf(stderr, "Error en getaddrinfo: %s\n", gai_strerror(err));
        exit(1); // O manejar con return -1 si prefieres
    }

    // Crear el socket
    int fd_conexion = socket(server_info->ai_family,
                            server_info->ai_socktype,
                            server_info->ai_protocol);

    if (fd_conexion == -1) {
        perror("Error al crear el socket");
        freeaddrinfo(server_info);
        exit(1);
    }

    // Momento donde se realiza la conexion al servidor
    err = connect(fd_conexion, server_info->ai_addr, server_info->ai_addrlen);

    if (err == -1) {
        perror("Error en la conexión");
        close(fd_conexion); // Cerrar el socket en caso de error
        freeaddrinfo(server_info);
        exit(1);
    }

    // Cerrar la conexión una vez que se haya terminado
    close_connection(fd_conexion);

    // Liberar la memoria de getaddrinfo
    freeaddrinfo(server_info);
}

void close_connection(int fd_conexion) {
    // Función para cerrar la conexión de forma segura
    if (fd_conexion != -1) {
        close(fd_conexion);
        printf("Conexión cerrada exitosamente\n");
    }
}
