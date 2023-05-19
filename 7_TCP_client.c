#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
// Windows headers
#define _WIN32_WINNT _WIN32_WINNT_WIN7
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
// Unix/Linux headers
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

void OSInit(void)
{
#ifdef _WIN32
    WSADATA wsaData;
	int WSAError = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (WSAError != 0)
	{
		fprintf(stderr, "WSAStartup errno = %d\n", WSAError);
		exit(-1);
	}
#endif
}

void OSCleanup(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
}

int initialization()
{
    struct addrinfo hints;
    struct addrinfo *server_info;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int result = getaddrinfo("127.0.0.1", "24042", &hints, &server_info);
    if (result != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        exit(1);
    }

    int internet_socket = socket(server_info->ai_family, server_info->ai_socktype, server_info->ai_protocol);
    if (internet_socket == -1)
    {
        perror("socket");
        exit(2);
    }

    result = connect(internet_socket, server_info->ai_addr, server_info->ai_addrlen);
    if (result == -1)
    {
        perror("connect");
        close(internet_socket);
        exit(3);
    }

    freeaddrinfo(server_info);

    return internet_socket;
}

void execution(int internet_socket)
{
    // Step 2.1
    const int BUFFER_SIZE = 100;
    char buffer[BUFFER_SIZE];

    // Generate random operation
    int number1 = rand() % 100;
    int number2 = rand() % 100;
    char operators[] = "+-*/";
    char operator = operators[rand() % 4];

    snprintf(buffer, BUFFER_SIZE, "%d %c %d", number1, operator, number2);

    // Send operation to server
    int bytes_sent = send(internet_socket, buffer, strlen(buffer), 0);
    if (bytes_sent == -1)
    {
        perror("send");
        return;
    }

    // Step 2.2
    int bytes_received = recv(internet_socket, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received == -1)
    {
        perror("recv");
        return;
    }

    buffer[bytes_received] = '\0';
    printf("Received: %s\n", buffer);

    // Verify the result
    int result;
    if (sscanf(buffer, "%d", &result) == 1)
    {
        // Compare the result with the expected value
        int expected_result;
        switch (operator)
        {
            case '+':
                expected_result = number1 + number2;
                break;
            case '-':
                expected_result = number1 - number2;
                break;
            case '*':
                expected_result = number1 * number2;
                break;
            case '/':
                expected_result = number1 / number2;
                break;
            default:
                expected_result = 0; // Invalid operator
        }

        if (result == expected_result)
        {
            printf("Result is correct!\n");
        }
        else
        {
            printf("Result is incorrect. Expected: %d, Received: %d\n", expected_result, result);
        }
    }
    else
    {
        printf("Invalid result received from the server.\n");
    }
}

void cleanup(int internet_socket)
{
    // Step 3.2
    int shutdown_return = shutdown(internet_socket, SHUT_RDWR);
    if (shutdown_return == -1)
    {
        perror("shutdown");
    }

    // Step 3.1
    close(internet_socket);
}

int main()
{
    // Initialization
    OSInit();
    int internet_socket = initialization();

    // Execution
    execution(internet_socket);

    // Cleanup
    cleanup(internet_socket);
    OSCleanup();

    return 0;
}
