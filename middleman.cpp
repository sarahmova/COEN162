//sarah movahedi
//coen 162 - angela musurlian
//3 may 2023
//http://students.engr.scu.edu/~smovahedi/lab3/index.html


//DIRECTIONS:
/*Your job is to develop a program that will communicate with both the 
  browser and the web server. Your program will act as a web proxy that 
  receives the HTTP requests (GET requests) from your browser and parse 
  them to extract the server name and file requested, to then connect to 
  the web server to forward the request. The proxy will wait for the 
  server’s answer to forward it to the browser. The user will not be 
  aware of the web proxy actions, but you should be able to see the whole 
  process in your terminal as you print the actions and info. Your browser 
  should be able to load the whole webpage.
*/

#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <pthread.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>

#define BUFFER_SIZE 5012


std::string parsehost(char* buffer)
{
    std::string data(buffer);
    std::string hostfind = "Host: ";
    std::string eolfind = "\r\n";
    std::string host = data.substr(data.find(hostfind)+hostfind.length());
    host = host.substr(0,host.find(eolfind));
    return host;
}
void handle_client(int client_socket, int server_port);

/*The main function takes two command-line arguments: the proxy port 
  and the server port. The program first creates a socket, binds it 
  to a local address and port, and listens for incoming connections. 
  When a client connects, the program forks a child process to handle 
  the connection.
*/

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <proxy_port> <server_port>\n", argv[0]);
        return 1;
    }

    int proxy_port = atoi(argv[1]);
    int server_port = atoi(argv[2]);
    
    // First we make the proxy socket
    int proxy_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (proxy_socket == -1) {
        perror("Proxy socket creation failed");
        return 1;
    }
    
    // Then we bind the proxy socket to a local address and port
    struct sockaddr_in proxy_addr;
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_addr.s_addr = INADDR_ANY;
    proxy_addr.sin_port = htons(proxy_port);
    if (bind(proxy_socket, (struct sockaddr*)&proxy_addr, sizeof(proxy_addr)) == -1) {
        perror("Proxy socket binding failed");
        close(proxy_socket);
        return 1;
    }

    // Here, we are listening for incoming connections
    if (listen(proxy_socket, 5) == -1) {
        perror("Proxy socket listening failed");
        close(proxy_socket);
        return 1;
    }

    printf("Proxy is listening on port %d\n", proxy_port);

    while (1) {
        // Accept client connection
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(proxy_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Client socket acceptance failed");
            close(proxy_socket);
            return 1;
        }

        printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Create child process to handle client
        pid_t pid = fork();
        if (pid == -1) {
            perror("Fork failed");
            close(client_socket);
            continue;
        } else if (pid == 0) {
            // Child process
            close(proxy_socket);
            handle_client(client_socket,server_port);
            close(client_socket);
            exit(0);
        } else {
            // Parent process
            close(client_socket);
        }
    }

    // Close up shop
    close(proxy_socket);

    return 0;
}
/*The child process reads the client request from the socket, parses 
  the host name from the request, creates a socket to connect to the 
  server, sends the request to the server, and receives the response 
  from the server. The child process then sends the response back to 
  the client and closes the sockets.
*/

void handle_client(int client_socket,int server_port) {
    char buffer[BUFFER_SIZE];
    size_t num_bytes = read(client_socket, buffer, sizeof(buffer));
    if (num_bytes == -1) {
        perror("Read from client socket failed");
        return;
    }

    // Null-terminate the received data
    buffer[num_bytes] = '\0';
    // Print client request
    printf("Received request\n");
    std::string host = parsehost(buffer);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Server socket creation failed");
        return;
    }

    // Connect to the server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80);
    server_addr.sin_addr = *((struct in_addr *) ((struct hostent *) gethostbyname(host.c_str())->h_addr));
    if(connect(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        printf("Error connecting to server\n");
        close(server_socket);
        return;
    }
    printf("Connected to %s\n",host.c_str());
    if(send(server_socket,buffer,sizeof(buffer),0)==-1){
        printf("Error sending buffer\n");
        close(server_socket);
        return;
    }
    std::memset(buffer, 0,sizeof(buffer));
    while (true) {
        int bytes_received = recv(server_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            printf("Error receiving data from socket");
            break;
        }
        if (bytes_received == 0) {
            break;
        }
        buffer[bytes_received] = '\0';
        int bytesSent = send(client_socket,buffer,sizeof(buffer),0);
        if(bytesSent == -1){
            printf("Error sending the data");
            break;
        }
        printf("%s\n",buffer);
        std::memset(buffer, 0,sizeof(buffer));
    }

    num_bytes = 0;
    // Print out server response
    close(server_socket);
    close(client_socket);

}

/*The program runs in an infinite loop, waiting for incoming connections 
  and handling each connection in a separate child process.
*/

//and thats all folks:D