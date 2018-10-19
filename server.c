#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 2048

int main()
{

	int sock;                           //This will be our socket
  int connectionSock;
	struct sockaddr_in server_addr, client_addr;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	char* method;
	char* splitInput;
	char fileName[MAXBUFSIZE];
	int fd;
	int len;

	char png_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/png; charset=UTF-8\r\n\r\n";
  char gif_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/gif; charset=UTF-8\r\n\r\n";
  char html_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\n";
  char text_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/plain; charset=UTF-8\r\n\r\n";
  char css_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/css; charset=UTF-8\r\n\r\n";
  char jpg_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/jpeg; charset=UTF-8\r\n\r\n";
  char js_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/javascript; charset=UTF-8\r\n\r\n";


	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&server_addr,sizeof(server_addr));                    //zero the struct
	server_addr.sin_family = AF_INET;                   //address family
	server_addr.sin_port = htons(5001);        //htons() sets the port # to network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine

	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("unable to create socket");
	}

	/******************
	  Once we've created a socket, we must bind that socket to the
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
	{
		printf("unable to bind socket\n");
		exit(1);
	}

  if(listen(sock, 5) < 0) {
    printf("unable to listen on port\n");
  }

	int client_length = sizeof(client_addr);
	bzero(&client_addr, client_length);

  // Accept Connection
  connectionSock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *) &client_length);

  if (connectionSock < 0) {
      perror("ERROR on accept");
      exit(1);
   }

   /* If connection is established then start communicating */
	 bzero(buffer,sizeof(buffer));
   nbytes = read(connectionSock, buffer, MAXBUFSIZE);

   if (nbytes < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("%s\n",buffer);

	 // GET or POST
	 method = strtok(buffer, " ");

	 // Get filename
	 splitInput = strtok(NULL, " ");
	 //fileName = strtok(NULL, " ");
	 if (splitInput[0] == '/') splitInput++;
	 sprintf(fileName, "%s", splitInput);
	 //splitInput[strlen(splitInput) - 1] = '\0';


	 printf("%s\n", splitInput);
	 //fd = open(filename, O_RDONLY);

	 //nbytes = send(sock, &over, sizeof(over), 0, (struct sockaddr *) &remote, sizeof(remote));

	 len = send(connectionSock, html_response, sizeof(html_response) - 1, 0);

	 bzero(buffer,sizeof(buffer));
	 fd = open(fileName, O_RDONLY);
	 while(read(fd, buffer, sizeof(buffer)) != 0) {
		 printf("hit\n" );
	   len = send(connectionSock, buffer, strlen(buffer), 0);
	 	 bzero(buffer,sizeof(buffer));
	 }


	 //len = send(connectionSock, &fd, sizeof(fd) - 1, 0);
	 printf("%i\n", len);
	 close(sock);

}
