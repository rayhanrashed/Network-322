#include <cstdio>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

int main(){

	int sockfd; 
	int bind_flag;
	int bytes_received;
	socklen_t addrlen;
	unsigned char buffer[1024];
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(4747);
	server_address.sin_addr.s_addr = inet_addr("192.168.0.3");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	bind_flag = bind(sockfd, (struct sockaddr*) &server_address, sizeof(sockaddr_in));

	while(true){
		bytes_received = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr*) &client_address, &addrlen);
		int x;
		x=buffer[12]<<8|buffer[13];
		printf("%d %d\n", x,sizeof(buffer));
		x=buffer[13]<<8|buffer[12];
		printf("%d\n", sizeof(x));
		printf("[%s:%d]: %s\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port), buffer);
	}

	return 0;

}
