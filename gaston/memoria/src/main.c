#include <utils/connection.h>
#include <commons/log.h>
#include <signal.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <netinet/in.h>

#define PORT "5001" // Replace with the actual port for the memory server

t_log* logger;
int server_socket;

// Signal handler to clean up resources on termination
void handle_signal(int signal) {
    log_info(logger, "Shutting down server...");
    close(server_socket);
    log_destroy(logger);
    exit(EXIT_SUCCESS);
}

// Function to handle each client connection
void* handle_client(void* client_socket_ptr) {
    int client_socket = *(int*)client_socket_ptr;
    free(client_socket_ptr);

    log_info(logger, "New client connected: socket %d", client_socket);

    // Buffer para recibir el mensaje del cliente
    char buffer[1024] = {0};
    int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0'; // Asegurarse de que el mensaje sea un string válido
        log_info(logger, "Message received from client: %s", buffer);
        
        char* response = "Message received by the server";
        send(client_socket, response, strlen(response), 0);
    } else if (bytes_received == 0) {
        log_info(logger, "Client disconnected: socket %d", client_socket);
    } else {
        log_error(logger, "Error receiving message from client: socket %d", client_socket);
    }

    close(client_socket);
    log_info(logger, "Client disconnected: socket %d", client_socket);
    return NULL;
}

int main(int argc, char* argv[]) {
    // Create a logger to record events
    logger = log_create("memory.log", "MEMORY", true, LOG_LEVEL_INFO);

    // Set up signal handling for graceful shutdown
    signal(SIGINT, handle_signal);

    // Start the server
    server_socket = create_server(PORT);
    if (server_socket == -1) {
        log_error(logger, "Could not start the server");
        log_destroy(logger);
        return EXIT_FAILURE;
    }
    log_info(logger, "Server started on port %s", PORT);

    // Accept connections in a loop
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int* client_socket = malloc(sizeof(int));
        if (!client_socket) {
            log_error(logger, "Memory allocation failed");
            continue;
        }

        *client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (*client_socket == -1) {
            log_error(logger, "Failed to accept connection");
            free(client_socket);
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_socket) != 0) {
            log_error(logger, "Failed to create thread for client");
            close(*client_socket);
            free(client_socket);
            continue;
        }

        pthread_detach(thread_id); 
    }

    // Clean up resources (this will never be reached)
    close(server_socket);
    log_destroy(logger);
    return 0;
}