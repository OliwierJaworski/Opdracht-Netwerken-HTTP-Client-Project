#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h>
#include <ws2tcpip.h>
#define perror(string) fprintf(stderr, string ": WSA errno = %d\n", WSAGetLastError())
#else
#include <errno.h>
#endif

void OSInit(void) {}
void OSCleanup(void) {}

int initialization();
int connection(int internet_socket);
void execution(int client_internet_socket);
void cleanup(int internet_socket, int client_internet_socket);

int main(int argc, char* argv[])
{
    OSInit();

    int internet_socket = initialization();
    int client_internet_socket = connection(internet_socket);

    execution(client_internet_socket);

    cleanup(internet_socket, client_internet_socket);

    OSCleanup();

    return 0;
}

int initialization()
{
    struct addrinfo internet_address_setup;
    struct addrinfo* internet_address_result;
    memset(&internet_address_setup, 0, sizeof internet_address_setup);
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
    internet_address_setup.ai_flags = AI_PASSIVE;
    int getaddrinfo_return = getaddrinfo(NULL, "24042", &internet_address_setup, &internet_address_result);
    if (getaddrinfo_return != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(getaddrinfo_return));
        exit(1);
    }

    int internet_socket = -1;
    struct addrinfo* internet_address_result_iterator = internet_address_result;
    while (internet_address_result_iterator != NULL)
    {
        internet_socket = socket(internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol);
        if (internet_socket == -1)
        {
            perror("socket");
        }
        else
        {
            int bind_return = bind(internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen);
            if (bind_return == -1)
            {
                perror("bind");
                close(internet_socket);
            }
            else
            {
                int listen_return = listen(internet_socket, 1);
                if (listen_return == -1)
                {
                    close(internet_socket);
                    perror("listen");
                }
                else
                {
                    break;
                }
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo(internet_address_result);

    if (internet_socket == -1)
    {
        fprintf(stderr, "socket: no valid socket address found\n");
        exit(2);
    }

    return internet_socket;
}

int connection(int internet_socket)
{
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;
    int client_socket = accept(internet_socket, (struct sockaddr*)&client_internet_address, &client_internet_address_length);
    if (client_socket == -1)
    {
        perror("accept");
        close(internet_socket);
        exit(3);
    }
    return client_socket;
}

void execution(int client_internet_socket)
{
    int number_of_bytes_received = 0;
    char buffer[100
    int number_of_bytes_received = 0;
    char buffer[1000];
    number_of_bytes_received = recv(client_internet_socket, buffer, sizeof(buffer) - 1, 0);
    if (number_of_bytes_received == -1)
    {
        perror("recv");
    }
    else
    {
        buffer[number_of_bytes_received] = '\0';
        printf("Received: %s\n", buffer);

        // Splitting the received message into three parts: number1, operator, number2
        int number1, number2;
        char operator;
        sscanf(buffer, "%d %c %d", &number1, &operator, &number2);

        // Performing the calculation
        int result = 0;
        switch (operator)
        {
            case '+':
                result = number1 + number2;
                break;
            case '-':
                result = number1 - number2;
                break;
            case '*':
                result = number1 * number2;
                break;
            case '/':
                if (number2 != 0)
                    result = number1 / number2;
                else
                    printf("Error: Division by zero\n");
                break;
            default:
                printf("Error: Invalid operator\n");
                break;
        }

        // Converting the result to a string
        char result_string[100];
        sprintf(result_string, "%d", result);

        // Sending the result back to the client
        int number_of_bytes_sent = send(client_internet_socket, result_string, strlen(result_string), 0);
        if (number_of_bytes_sent == -1)
        {
            perror("send");
        }
        else
        {
            printf("Sent: %s\n", result_string);
        }
    }
}

void cleanup(int internet_socket, int client_internet_socket)
{
    close(client_internet_socket);
    close(internet_socket);
}