#include "connection.h"
#include <commons/config.h>


// Serializes a packet into a byte stream
void* serialize_packet(t_package* packet, int total_bytes) {
    void* serialized_data = malloc(total_bytes);
    int offset = 0;

    memcpy(serialized_data + offset, &(packet->operation_code), sizeof(int));
    offset += sizeof(int);
    memcpy(serialized_data + offset, &(packet->buffer->size), sizeof(int));
    offset += sizeof(int);
    memcpy(serialized_data + offset, packet->buffer->stream, packet->buffer->size);

    return serialized_data;
}


// Crea una conexión cliente a un servidor remoto

int create_connection(char* PORT_KEY) { //modificado para realizar con el config, server lo dejamos para no modificar y nos queden distintos para evaluar
    int client_socket;
    struct addrinfo hints, *server_info, *p;

    // Load .config
    t_config* config = load_config();
    //	char * config_get_string_value (t_config *, char *key)

    char* port = search_port(config, PORT_KEY);
    char* ip = search_ip(config);
    
    // Configurar hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP

    // Obtener información de la dirección
    if (getaddrinfo(ip, port, &hints, &server_info) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    // Crear el socket y conectarse
    for (p = server_info; p != NULL; p = p->ai_next) {
        client_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (client_socket == -1) {
            perror("socket");
            continue;
        }

        if (connect(client_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(client_socket);
            perror("connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to connect to server\n");
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);
    return client_socket;
}


// Crea un servidor que escucha conexiones entrantes
int create_server(const char* port) {
    int server_socket;
    struct addrinfo hints, *server_info, *p;

    // Configurar hints
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_flags = AI_PASSIVE;     // Usar la IP de la máquina

    // Obtener información de la dirección
    if (getaddrinfo(NULL, port, &hints, &server_info) != 0) {
        perror("getaddrinfo");
        return -1;
    }

    // Crear el socket y configurarlo
    for (p = server_info; p != NULL; p = p->ai_next) {
        server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (server_socket == -1) {
            perror("socket");
            continue;
        }

        // Reutilizar el puerto si está en uso
        int optval = 1;
        setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        // Enlazar el socket
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1) {
            close(server_socket);
            perror("bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "Failed to bind socket\n");
        freeaddrinfo(server_info);
        return -1;
    }

    // Escuchar conexiones
    if (listen(server_socket, SOMAXCONN) == -1) {
        perror("listen");
        close(server_socket);
        freeaddrinfo(server_info);
        return -1;
    }

    freeaddrinfo(server_info);
    return server_socket;
}

// Sends a simple message to the server
void send_message(char* message, int client_socket) {
    t_package* packet = malloc(sizeof(t_package));
    packet->operation_code = MESSAGE;
    packet->buffer = malloc(sizeof(t_buffer));
    packet->buffer->size = strlen(message) + 1;
    packet->buffer->stream = malloc(packet->buffer->size);
    memcpy(packet->buffer->stream, message, packet->buffer->size);

    int total_bytes = packet->buffer->size + 2 * sizeof(int);
    void* serialized_data = serialize_packet(packet, total_bytes);

    send(client_socket, serialized_data, total_bytes, 0);

    free(serialized_data);
    delete_packet(packet);
}

// Initializes the buffer for a packet
void create_buffer(t_package* packet) {
    packet->buffer = malloc(sizeof(t_buffer));
    packet->buffer->size = 0;
    packet->buffer->stream = NULL;
}

// Creates a new packet
t_package* create_packet(void) {
    t_package* packet = malloc(sizeof(t_package));
    packet->operation_code = PACKAGE;
    create_buffer(packet);
    return packet;
}

// Adds data to a packet
void add_to_packet(t_package* packet, void* value, int size) {
    packet->buffer->stream = realloc(packet->buffer->stream, packet->buffer->size + size + sizeof(int));

    memcpy(packet->buffer->stream + packet->buffer->size, &size, sizeof(int));
    memcpy(packet->buffer->stream + packet->buffer->size + sizeof(int), value, size);

    packet->buffer->size += size + sizeof(int);
}

// Sends a packet to the server
void send_packet(t_package* packet, int client_socket) {
    int total_bytes = packet->buffer->size + 2 * sizeof(int);
    void* serialized_data = serialize_packet(packet, total_bytes);

    send(client_socket, serialized_data, total_bytes, 0);

    free(serialized_data);
}

// Frees the memory allocated for a packet
void delete_packet(t_package* packet) {
    free(packet->buffer->stream);
    free(packet->buffer);
    free(packet);
}

// Closes the connection to the server
void free_connection(int client_socket) {
    close(client_socket);
}