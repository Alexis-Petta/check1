#include <utils/connection.h>
#include <commons/log.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define SERVER_IP "127.0.0.1" // Memory server IP address
#define SERVER_PORT "5001"    // Memory server port

t_log* logger;

int main() {
    // Create a logger to record events
    logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO);

    // Create connection with the memory server
    int memory_socket = create_connection(SERVER_IP, SERVER_PORT);
    if (memory_socket == -1) {
        log_error(logger, "Could not connect to the memory server");
        log_destroy(logger);
        return EXIT_FAILURE;
    }
    log_info(logger, "Connected to the memory server at %s:%s", SERVER_IP, SERVER_PORT);

    // Send a test message to the server
    char* message = "Hello, from CPU";
    if (send(memory_socket, message, strlen(message), 0) == -1) {
        log_error(logger, "Error sending message to the memory server");
        close(memory_socket);
        log_destroy(logger);
        return EXIT_FAILURE;
    }

    log_info(logger, "Message sent to the server: %s", message);

      // Receive the response from the server
      char response[256]; // Buffer to store the server's response
      int bytes_received = recv(memory_socket, response, sizeof(response) - 1, 0);
      if (bytes_received == -1) {
          log_error(logger, "Error receiving response from the memory server");
          close(memory_socket);
          log_destroy(logger);
          return EXIT_FAILURE;
      }
  
      // Null-terminate the received response and log it
      response[bytes_received] = '\0';
      log_info(logger, "Server: %s", response);

    // Close the connection
    close(memory_socket);
    log_info(logger, "Connection closed with the memory server");

    // Free resources
    log_destroy(logger);
    return 0;
}