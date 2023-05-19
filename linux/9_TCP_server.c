#include <sys/socket.h> //for sockaddr, socket, socket
#include <sys/types.h> //for size_t
#include <netdb.h> //for getaddrinfo
#include <unistd.h> //for close
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for htons, htonl, inet_pton, inet_ntop
#include <string.h> //for memset
#include <stdio.h> //for fprintf, perror
#include <errno.h> //for errno
#include <stdlib.h> //for exit

void print_ip_address( unsigned short family, struct sockaddr * ip )
{
	void * ip_address;
	char * ip_version;
	char ip_string[INET6_ADDRSTRLEN];

	if( family == AF_INET )
	{ // IPv4
		struct sockaddr_in *ipv4 = (struct sockaddr_in *)ip;
		ip_address = &(ipv4->sin_addr);
		ip_version = "IPv4";
	}
	else
	{ // IPv6
		struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)ip;
		ip_address = &(ipv6->sin6_addr);
		ip_version = "IPv6";
	}

	inet_ntop( family, ip_address, ip_string, sizeof ip_string );
	printf( "%s -> %s\n", ip_version, ip_string );
}

void ai_print_ip_address( struct addrinfo * ip )
{
	print_ip_address( ip->ai_family, ip->ai_addr );
}

void ss_print_ip_address( struct sockaddr_storage * ip )
{
	print_ip_address( ip->ss_family, (struct sockaddr*) ip );
}

int main( int argc, char * argv[] )
{
	struct addrinfo internet_address_setup, *result_head, *result_item;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	internet_address_setup.ai_socktype = SOCK_STREAM;
	internet_address_setup.ai_flags = AI_PASSIVE; // use ANY address for IPv4 and IPv6

	int getaddrinfo_return;
	getaddrinfo_return = getaddrinfo( NULL, "24042", &internet_address_setup, &result_head );
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 2 );
	}

	int internet_socket;

	result_item = result_head; //take first of the linked list
	while( result_item != NULL ) //while the pointer is valid
	{
		internet_socket = socket( result_item->ai_family, result_item->ai_socktype, result_item->ai_protocol );
		if( internet_socket == -1 )
		{
			perror( "socket" );
		}
		else
		{
			int return_code;
			return_code = bind( internet_socket, result_item->ai_addr, result_item->ai_addrlen );
			if( return_code == -1 )
			{
				close( internet_socket );
				perror( "bind" );
			}
			else
			{
				int listen_return;
				listen_return = listen( internet_socket, 1 );
				if( listen_return == -1 )
				{
					close( internet_socket );
					perror( "listen" );
				}
				else
				{
					printf( "listen on " );
					ai_print_ip_address( result_item );
					break; //stop running through the linked list
				}
			}
		}
		result_item = result_item->ai_next; //take next in the linked list
	}
	if( result_item == NULL )
	{
		fprintf( stderr, "socket: no valid socket address found\n" );
		exit( 5 );
	}
	freeaddrinfo( result_head ); //free the linked list

	struct sockaddr_storage client_ip_address;
	socklen_t client_ip_address_length = sizeof client_ip_address;
	int client_socket;
	client_socket = accept( internet_socket, (struct sockaddr *) &client_ip_address, &client_ip_address_length );
	if( client_socket == -1 )
	{
		close( internet_socket );
		perror( "accept" );
		exit( 6 );
	}
	close( internet_socket );
	printf( "Got connection from " );
	ss_print_ip_address( &client_ip_address );

	int number_of_bytes_received = 0;
	char buffer[1000];
	number_of_bytes_received = recv( client_socket, buffer, ( sizeof buffer ) - 1, 0 );
	if( number_of_bytes_received == -1 )
	{
		perror( "recv" );
	}
	buffer[number_of_bytes_received] = '\0';
	printf( "Got %s\n", buffer );

	int shutdown_return;
	shutdown_return = shutdown( client_socket, SHUT_RD ); //Shutdown Send == SD_SEND ; Receive == SD_RECEIVE ; Send/Receive == SD_BOTH ; https://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable --> Linux : Shutdown Send == SHUT_WR ; Receive == SHUT_RD ; Send/Receive == SHUT_RDWR
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	close( client_socket );

	return 0;
}