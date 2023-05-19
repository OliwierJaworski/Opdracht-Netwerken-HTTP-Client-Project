// gcc 1_UDP_client.c -lws2_32 -o 1_UDP_client.exe -D IPv4
// _WIN32_WINNT version constants --> https://stackoverflow.com/questions/15370033/how-to-use-inet-pton-with-the-mingw-compiler
//
// #define _WIN32_WINNT_NT4                    0x0400 // Windows NT 4.0
// #define _WIN32_WINNT_WIN2K                  0x0500 // Windows 2000
// #define _WIN32_WINNT_WINXP                  0x0501 // Windows XP
// #define _WIN32_WINNT_WS03                   0x0502 // Windows Server 2003
// #define _WIN32_WINNT_WIN6                   0x0600 // Windows Vista
// #define _WIN32_WINNT_VISTA                  0x0600 // Windows Vista
// #define _WIN32_WINNT_WS08                   0x0600 // Windows Server 2008
// #define _WIN32_WINNT_LONGHORN               0x0600 // Windows Vista
// #define _WIN32_WINNT_WIN7                   0x0601 // Windows 7
// #define _WIN32_WINNT_WIN8                   0x0602 // Windows 8
// #define _WIN32_WINNT_WINBLUE                0x0603 // Windows 8.1
// #define _WIN32_WINNT_WINTHRESHOLD           0x0A00 // Windows 10
// #define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10
//
#define _WIN32_WINNT 0x0601

#include <winsock2.h>
#include <ws2tcpip.h>

#include <stdio.h>
#include <unistd.h>

int main( int argc, char * argv[] )
{
	WSADATA wsaData; //WSAData wsaData; //Could be different case
	if( WSAStartup( MAKEWORD(2,0), &wsaData ) != 0 ) // MAKEWORD(1,1) for Winsock 1.1, MAKEWORD(2,0) for Winsock 2.0:
	{
		fprintf( stderr, "WSAStartup failed.\n" );
		exit( 1 );
	}

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
		printf( "errno = %d\n", WSAGetLastError() ); //https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
		perror( "sendto" );
	}

	close( internet_socket );

	WSACleanup();

	return 0;
}