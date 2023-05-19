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

void print_ip_address( struct sockaddr_storage * ip )
{
	void * ip_address;
	char * ip_version;
	char ip_string[INET6_ADDRSTRLEN];

	if( ip->ss_family == AF_INET )
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

	inet_ntop( ip->ss_family, ip_address, ip_string, sizeof ip_string );
	printf( "%s -> %s\n", ip_version, ip_string );
}

int main( int argc, char * argv[] )
{
#ifdef IPv4

	struct sockaddr_in internet_address;
	memset( &internet_address, '\0', sizeof internet_address );
	internet_address.sin_family = AF_INET;
	internet_address.sin_port = htons( 24042 ); //Host to Network Short (byte order)
	internet_address.sin_addr.s_addr = htonl( INADDR_ANY ); //INADDR_ANY == all interfaces

/*	int inet_pton_return;
	inet_pton_return = inet_pton( AF_INET, "0.0.0.0", &internet_address.sin_addr );
	if( inet_pton_return < 1 )
	{
		if( inet_pton_return == 0 )
		{
			fprintf( stderr, "Bad IP format" );
		}
		else
		{
			perror( "inet_pton" );
			printf( "errno = %d\n", WSAGetLastError() ); //https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
		}
		exit( 2 );
	}*/

	int internet_socket;
	internet_socket = socket( PF_INET, SOCK_DGRAM, 0 ); //UDP == SOCK_DGRAM ; TCP == SOCK_STREAM

#else

	struct sockaddr_in6 internet_address;
	memset( &internet_address, '\0', sizeof internet_address ); //https://stackoverflow.com/questions/25703798/getting-error-10049-address-not-avai-on-sendto-in-udp-connection-but-bind
	internet_address.sin6_family = AF_INET6;
	internet_address.sin6_port = htons( 24042 ); //Host to Network Short (byte order)
	internet_address.sin6_addr = in6addr_any; //in6addr_any == :: ; in6addr_loopback == ::1

	/*int inet_pton_return;
	inet_pton_return = inet_pton( AF_INET6, "::", &internet_address.sin6_addr ); //https://stackoverflow.com/questions/7474066/how-to-listen-on-all-ipv6-addresses-using-c-sockets-api
	if( inet_pton_return < 1 )
	{
		if( inet_pton_return == 0 )
		{
			fprintf( stderr, "Bad IP format" );
		}
		else
		{
			perror( "inet_pton" );
		}
		exit( 2 );
	}*/

	int internet_socket;
	internet_socket = socket( PF_INET6, SOCK_DGRAM, 0 ); //https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket

#endif

	if( internet_socket == -1 )
	{
		perror( "socket" );
		exit( 3 );
	}

	int bind_return;
	bind_return = bind( internet_socket, (struct sockaddr *) &internet_address, sizeof internet_address );
	if( bind_return == -1 )
	{
		close( internet_socket );
		perror( "bind" );
		exit( 4 );
	}

	int number_of_bytes_received = 0;
	char buffer[1000];
	struct sockaddr_storage client_ip_address;
	socklen_t client_ip_address_length = sizeof client_ip_address;
	number_of_bytes_received = recvfrom( internet_socket, buffer, ( sizeof buffer ) - 1, 0, (struct sockaddr *) &client_ip_address, &client_ip_address_length );
	if( number_of_bytes_received == -1 )
	{
		perror( "recvfrom" );
	}
	buffer[number_of_bytes_received] = '\0';
	printf( "Got %s from ", buffer );
	print_ip_address( &client_ip_address );

	close( internet_socket );

	return 0;
}