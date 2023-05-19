// gcc 0_UDP_client.c -lws2_32 -o 0_UDP_client.exe

#include <winsock2.h> //for all network functions
#include <unistd.h> //for close()

int main( int argc, char * argv[] )
{
	WSADATA wsaData;
	WSAStartup( MAKEWORD(2,0), &wsaData );

	struct sockaddr internet_address;
	internet_address.sa_family = AF_INET;
	internet_address.sa_data[0] = 24042 >> 8;
	internet_address.sa_data[1] = 24042 & 0xFF;
	internet_address.sa_data[2] = 127;
	internet_address.sa_data[3] = 0;
	internet_address.sa_data[4] = 0;
	internet_address.sa_data[5] = 1;
	internet_address.sa_data[6] = 0;
	internet_address.sa_data[7] = 0;
	internet_address.sa_data[8] = 0;
	internet_address.sa_data[9] = 0;
	internet_address.sa_data[10] = 0;
	internet_address.sa_data[11] = 0;
	internet_address.sa_data[12] = 0;
	internet_address.sa_data[13] = 0;

	int internet_socket;
	internet_socket = socket( PF_INET, SOCK_DGRAM, 0 );

	sendto( internet_socket, "Hello UDP world!", 16, 0, &internet_address, sizeof internet_address );

	close( internet_socket );

	WSACleanup();

	return 0;
}