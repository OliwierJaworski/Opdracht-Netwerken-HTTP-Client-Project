#include <sys/socket.h> //for sockaddr, socket, socket
#include <unistd.h> //for close
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h> //for htons, htonl, inet_pton
#include <string.h> //for memset
#include <stdio.h> //for fprintf, perror
#include <errno.h> //for errno
#include <stdlib.h> //for exit

int main( int argc, char * argv[] )
{
#ifdef IPv4

	struct sockaddr_in internet_address;
	memset( &internet_address, '\0', sizeof internet_address );
	internet_address.sin_family = AF_INET;
	internet_address.sin_port = htons( 24042 ); //Host to Network Short (byte order)
	internet_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK ); //INADDR_LOOPBACK == localhost

/*	int inet_pton_return;
	inet_pton_return = inet_pton( AF_INET, "127.0.0.1", &internet_address.sin_addr );
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
	internet_socket = socket( PF_INET, SOCK_DGRAM, 0 ); //UDP == SOCK_DGRAM ; TCP == SOCK_STREAM

#else

	struct sockaddr_in6 internet_address;
	memset( &internet_address, '\0', sizeof internet_address ); //https://stackoverflow.com/questions/25703798/getting-error-10049-address-not-avai-on-sendto-in-udp-connection-but-bind
	internet_address.sin6_family = AF_INET6;
	internet_address.sin6_port = htons( 24042 ); //Host to Network Short (byte order)
	internet_address.sin6_addr = in6addr_loopback; //in6addr_any == :: ; in6addr_loopback == ::1

	/*int inet_pton_return;
	inet_pton_return = inet_pton( AF_INET6, "::1", &internet_address.sin6_addr );
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

	int number_of_bytes_send = 0;
	number_of_bytes_send = sendto( internet_socket, "Hello UDP world!", 16, 0, (struct sockaddr *) &internet_address, sizeof internet_address );
	if( number_of_bytes_send == -1 )
	{
		perror( "sendto" );
	}

	close( internet_socket );

	return 0;
}