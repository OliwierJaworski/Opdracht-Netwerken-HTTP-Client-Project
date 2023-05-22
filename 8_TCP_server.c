#ifdef _WIN32
#define _WIN32_WINNT _WIN32_WINNT_WIN7
	#include <winsock2.h> //for all socket programming
	#include <ws2tcpip.h> //for getaddrinfo, inet_pton, inet_ntop
	#include <stdio.h> //for fprintf, perror
	#include <unistd.h> //for close
	#include <stdlib.h> //for exit
	#include <string.h> //for memset
    #include <pthread.h>
	void OSInit( void )
	{
		WSADATA wsaData;
		int WSAError = WSAStartup( MAKEWORD( 2, 0 ), &wsaData );
		if( WSAError != 0 )
		{
			fprintf( stderr, "WSAStartup errno = %d\n", WSAError );
			exit( -1 );
		}
	}
	void OSCleanup( void )
	{
		WSACleanup();
	}
	#define perror(string) fprintf( stderr, string ": WSA errno = %d\n", WSAGetLastError() )
#else
#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h> //for size_t
#include <netdb.h> //for getaddrinfo
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
#include <errno.h> //for errno
#include <stdio.h> //for fprintf, perror
#include <unistd.h> //for close
#include <stdlib.h> //for exit
#include <string.h> //for memset
#include <pthread.h>
void OSInit( void ) {}
void OSCleanup( void ) {}
#endif



int initialization();
int connection( int internet_socket );
void execution( int internet_socket, const char* client_ip );
void cleanup( int internet_socket, int client_internet_socket );


char Client_ip[INET6_ADDRSTRLEN];//ip adres van de client

int main( int argc, char * argv[] )
{

    //////////////////
    //Initialization//
    //////////////////

    OSInit();

    int internet_socket = initialization();

    //////////////
    //Connection//
    //////////////
    while (1)
    {
        int client_internet_socket = connection(internet_socket);

        /////////////
        //Execution//
        /////////////

        execution(client_internet_socket, Client_ip);


        ////////////
        //Clean up//
        ////////////

        cleanup(internet_socket, client_internet_socket);
    }
    OSCleanup();

    return 0;
}

int initialization()
{
    //Step 1.1
    struct addrinfo internet_address_setup;
    struct addrinfo * internet_address_result;
    memset( &internet_address_setup, 0, sizeof internet_address_setup );
    internet_address_setup.ai_family = AF_UNSPEC;
    internet_address_setup.ai_socktype = SOCK_STREAM;
    internet_address_setup.ai_flags = AI_PASSIVE;
    int getaddrinfo_return = getaddrinfo( NULL, "22", &internet_address_setup, &internet_address_result );
    if( getaddrinfo_return != 0 )
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
        exit( 1 );
    }

    int internet_socket = -1;
    struct addrinfo * internet_address_result_iterator = internet_address_result;
    while( internet_address_result_iterator != NULL )
    {
        //Step 1.2
        internet_socket = socket( internet_address_result_iterator->ai_family, internet_address_result_iterator->ai_socktype, internet_address_result_iterator->ai_protocol );
        if( internet_socket == -1 )
        {
            perror( "socket" );
        }
        else
        {
            //Step 1.3
            int bind_return = bind( internet_socket, internet_address_result_iterator->ai_addr, internet_address_result_iterator->ai_addrlen );
            if( bind_return == -1 )
            {
                perror( "bind" );
                close( internet_socket );
            }
            else
            {
                //Step 1.4
                int listen_return = listen( internet_socket, 1 );
                if( listen_return == -1 )
                {
                    close( internet_socket );
                    perror( "listen" );
                }
                else
                {
                    break;
                }
            }
        }
        internet_address_result_iterator = internet_address_result_iterator->ai_next;
    }

    freeaddrinfo( internet_address_result );

    if( internet_socket == -1 )
    {
        fprintf( stderr, "socket: no valid socket address found\n" );
        exit( 2 );
    }

    return internet_socket;
}

int connection( int internet_socket )
{

    //Step 2.1
    struct sockaddr_storage client_internet_address;
    socklen_t client_internet_address_length = sizeof client_internet_address;
    int client_socket = accept( internet_socket, (struct sockaddr *) &client_internet_address, &client_internet_address_length );
    if( client_socket == -1 )
    {
        perror( "accept" );
        close( internet_socket );
        exit( 3 );
    }

    if(client_internet_address.ss_family ==AF_INET)
    {
        struct  sockaddr_in *ipv4 =(struct sockaddr_in *)&client_internet_address;
        inet_ntop(AF_INET, &(ipv4->sin_addr),Client_ip, INET6_ADDRSTRLEN);
    }

    else if (client_internet_address.ss_family ==AF_INET6)
    {
        struct sockaddr_in6 *ipv6 =(struct sockaddr_in6 *)&client_internet_address;
        inet_ntop(AF_INET6, &(ipv6->sin6_addr),Client_ip, INET6_ADDRSTRLEN);
    }

    else
    {
        fprintf(stderr,"unknown address family\n");
        close(internet_socket);
        exit(3);
    }
    printf("client IP: %s\n",Client_ip);

    return client_socket;
}

void execution( int internet_socket, const char* client_ip )
{

    // Step 3.2
    char wget_command[256];
    sprintf(wget_command, "wget -O temp.json http://ip-api.com/json/%s", client_ip);

    int system_result = system(wget_command);

    if (system_result == -1)
    {
        perror("system");

    }

    else
    {
        if (system_result == 0)
        {
            //--test--printf("API request successful. Response saved in temp.json\n");
            FILE *temp_file = fopen("temp.json", "r");
            if (temp_file == NULL)
            {
                perror("geen data");
            } else {
                //logs openen in append modus
                FILE *output_file = fopen("logs.txt", "a");
                if (output_file == NULL)
                {
                    perror("geen data");
                } else
                {
                    fprintf(output_file, "\n");
                    int ch;
                    while ((ch = fgetc(temp_file)) != EOF) {
                        fputc(ch, output_file);
                    }
                    fclose(output_file);
                }

                fclose(temp_file);
                //  temp files verwijderen
                remove("temp.json");
            }
        }

    }

    FILE *log_file =fopen("datasend.txt","w+");

    if(log_file ==NULL)
    {
        perror("Failed to open log file");
        return;
    }

    int total_bytes_send =0;

    while(1)
    {
        //Step 3.1
        int number_of_bytes_received = 0;
        char buffer[1000];
        number_of_bytes_received = recv(internet_socket, buffer, (sizeof buffer) - 1, 0);
        if (number_of_bytes_received == -1) {
            perror("recv");
            break;  //break  loop
        } else if (number_of_bytes_received == 0) {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;// Client disconnected, break the loop
        }

        else
        {
            buffer[number_of_bytes_received] = '\0';

            // Save the received message in Messages.txt
            FILE *message_file = fopen("Messages.txt", "a");
            if (message_file == NULL) {
                perror("geen data");
            } else {
                fprintf(message_file, "%s\n", buffer);
                fclose(message_file);
            }

            printf("Received : %s\n", buffer);
        }

        // Send random data to the client
        char random_data[1024];
        for (int i = 0; i < 1024; i++) {
            random_data[i] = rand() % 2;
        }

        int bytes_send = send(internet_socket, random_data, sizeof(random_data), 0);

        if (bytes_send == -1) {
            perror("Failed to send data");
            break;  // Error occurred, break the loop
        } else if (bytes_send == 0) {
            // Connection closed by the client
            printf("Client disconnected.\n");
            break;  // Client disconnected, break the loop
        }

        printf("Send random data to the client.\n");

        usleep(1000000);  // 1 second delay

        total_bytes_send += bytes_send;

        fprintf(log_file,"Total Bits Send: %d\n",total_bytes_send);
        fflush(log_file);
    }

}

void cleanup( int internet_socket, int client_internet_socket )
{
    //Step 4.2
    int shutdown_return = shutdown( client_internet_socket, SD_RECEIVE );
    if( shutdown_return == -1 )
    {
        perror( "shutdown" );
    }

    //Step 4.1
    close( client_internet_socket );
    close( internet_socket );
}
