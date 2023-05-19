// gcc 4_UDP_server.c -lws2_32 -o 4_UDP_server.exe -D IPv4
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
		printf( "errno = %d\n", WSAGetLastError() ); //https://docs.microsoft.com/en-us/windows/win32/winsock/windows-sockets-error-codes-2
		perror( "recvfrom" );
	}
	buffer[number_of_bytes_received] = '\0';
	printf( "Got %s from ", buffer );
	print_ip_address( &client_ip_address );

	close( internet_socket );

	WSACleanup();

	return 0;
}