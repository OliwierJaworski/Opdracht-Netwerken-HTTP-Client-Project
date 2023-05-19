// gcc 2_DNS_client.c -lws2_32 -o 2_DNS_client.exe
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

/*
	struct addrinfo in;
	memset( &in, 0, sizeof in );
	in.ai_family = AF_INET; //IPv4
	struct addrinfo *out = NULL;
	getaddrinfo( "pxl.be", NULL, &in, &out );
	printf( "%d.%d.%d.%d \n", (unsigned char) out->ai_addr->sa_data[2], (unsigned char) out->ai_addr->sa_data[3], (unsigned char) out->ai_addr->sa_data[4], (unsigned char) out->ai_addr->sa_data[5] );
*/

	struct addrinfo internet_address_setup, *result_head, *result_item;
	memset( &internet_address_setup, 0, sizeof internet_address_setup );
	internet_address_setup.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	internet_address_setup.ai_socktype = SOCK_DGRAM;

	int getaddrinfo_return;
	getaddrinfo_return = getaddrinfo( "localhost.pxl-ea-ict.be", NULL, &internet_address_setup, &result_head );
//	getaddrinfo_return = getaddrinfo( "example.com", NULL, &hints, &result_head ); //nslookup example.com
//	getaddrinfo_return = getaddrinfo( "cnn.com", NULL, &hints, &result_head ); //https://webmasters.stackexchange.com/questions/10927/using-multiple-a-records-for-my-domain-do-web-browsers-ever-try-more-than-one
	if( getaddrinfo_return != 0 )
	{
		fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( getaddrinfo_return ) );
		exit( 2 );
	}

	result_item = result_head; //take first of the linked list
	while( result_item != NULL ) //while the pointer is valid
	{
		void * ip_address;
		char * ip_version;
		char ip_string[INET6_ADDRSTRLEN];

		if( result_item->ai_family == AF_INET )
		{ // IPv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)result_item->ai_addr;
			ip_address = &(ipv4->sin_addr);
			ip_version = "IPv4";
		}
		else
		{ // IPv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)result_item->ai_addr;
			ip_address = &(ipv6->sin6_addr);
			ip_version = "IPv6";
		}

		inet_ntop( result_item->ai_family, ip_address, ip_string, sizeof ip_string );
		printf( "%s -> %s\n", ip_version, ip_string );

		result_item = result_item->ai_next; //take next in the linked list
	}

	freeaddrinfo( result_head ); //free the linked list

	WSACleanup();

	return 0;
}