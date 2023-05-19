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

	return 0;
}