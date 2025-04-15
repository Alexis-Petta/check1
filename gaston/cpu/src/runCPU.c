#include "runCPU.h"

t_log* logger;

void RUN_CPU(){
    // Create a logger to record events
    logger = log_create("cpu.log", "CPU", true, LOG_LEVEL_INFO); 
    memory_connection();
    kernel_connection();
    // Free resources
    log_destroy(logger);
};

// CREATE MEMORY CONNECTION
void memory_connection()
{    
    // Create connection with the memory server
    int memory_socket = create_connection("PORT_CPU_TO_MEMORY");

    if (memory_socket == -1) {
        log_error(logger, "Could not connect to the memory server");
        log_destroy(logger);
        return EXIT_FAILURE;
    }
    log_info(logger, "Connected to the memory server");
     
    /////////////////////ESTO DESPUES LO VOLAMOS/////////////////////
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
    ///////////////////// HASTA ACA /////////////////////

    
    // Close the connection
    close(memory_socket);
    log_info(logger, "Connection closed with the memory server");
}


// CREATE KERNEL CONNECTION
void kernel_connection()
{
        // Create connection with the kernel server
        int kernel_socket = create_connection("PORT_CPU_TO_KERNEL");
    
        if (kernel_socket == -1) {
            log_error(logger, "Could not connect to the kernel server");
            log_destroy(logger);
            return EXIT_FAILURE;
        }
        log_info(logger, "Connected to the kernel server");   
        
        // Close the connection
        close(kernel_socket);
        log_info(logger, "Connection closed with the kernel server");
}
