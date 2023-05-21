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

int main( int argc, char * argv[] )
{

#ifdef IPv4

	struct sockaddr_in internet_address;
	memset( &internet_address, '\0', sizeof internet_address );
	internet_address.sin_family = AF_INET;
	internet_address.sin_port = htons( 24042 ); //Host to Network Short (byte order)
	internet_address.sin_addr.s_addr = htonl( INADDR_LOOPBACK ); //INADDR_LOOPBACK == localhost

	/*int inet_pton_return;
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
	internet_socket = socket( PF_INET, SOCK_STREAM, 0 ); //UDP == SOCK_DGRAM ; TCP == SOCK_STREAM

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
	internet_socket = socket( PF_INET6, SOCK_STREAM, 0 ); //https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket

#endif

	if( internet_socket == -1 )
	{
		perror( "socket" );
		exit( 3 );
	}

	int connect_return;
	connect_return = connect( internet_socket, (struct sockaddr *) &internet_address, sizeof internet_address );
	if( connect_return == -1 )
	{
		perror( "connect" );
		close( internet_socket );
		exit( 4 );
	}

	int number_of_bytes_send = 0;
	number_of_bytes_send = send( internet_socket, "Hello TCP world!", 16, 0 );
	if( number_of_bytes_send == -1 )
	{
		perror( "send" );
	}

	int shutdown_return;
	shutdown_return = shutdown( internet_socket, SHUT_WR ); //Shutdown Send == SD_SEND ; Receive == SD_RECEIVE ; Send/Receive == SD_BOTH ; https://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable --> Linux : Shutdown Send == SHUT_WR ; Receive == SHUT_RD ; Send/Receive == SHUT_RDWR
	if( shutdown_return == -1 )
	{
		perror( "shutdown" );
	}

	close( internet_socket );

	return 0;
}