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
	char folderName[MAXBUFSIZE];
	char filePath[MAXBUFSIZE];
	char fileType[MAXBUFSIZE];
	char uri[MAXBUFSIZE];
	int fd;
	DIR* dir;
	struct dirent* in_file;
	int len;
	int i;
	int typeIndex;

	char png_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/png; charset=UTF-8\r\n\r\n";
  char gif_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/gif; charset=UTF-8\r\n\r\n";
  char html_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\n";
  char text_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/plain; charset=UTF-8\r\n\r\n";
  char css_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/css; charset=UTF-8\r\n\r\n";
  char jpg_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: image/jpeg; charset=UTF-8\r\n\r\n";
  char js_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/javascript; charset=UTF-8\r\n\r\n";
	char htm_response[] = "HTTP/1.1 200 OK:\r\n" "Content-Type: text/html; charset=UTF-8\r\n\r\n";

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

	while(1) {
		// Accept Connection
	  connectionSock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *) &client_length);

	  if (connectionSock < 0) {
	      perror("ERROR on accept");
	      exit(1);
	   }

	   /* If connection is established then start communicating */
		 bzero(buffer,sizeof(buffer));
		 //bzero(method,sizeof(method));
		 //bzero(splitInput,sizeof(splitInput));
		 bzero(fileName,sizeof(fileName));
		 bzero(folderName,sizeof(folderName));
		 bzero(filePath,sizeof(filePath));
		 bzero(fileType,sizeof(fileType));
		 bzero(uri,sizeof(uri));
	   nbytes = read(connectionSock, buffer, MAXBUFSIZE);

	   if (nbytes < 0) {
	      perror("ERROR reading from socket");
	      exit(1);
	   }

	   printf("%s\n",buffer);

		 // GET or POST
		 method = strtok(buffer, " ");
		 if(strcmp(method, "GET")) {
			 return -5;
		 }

		 // Get filename
		 splitInput = strtok(NULL, " ");
		 printf("request: %s\n", splitInput);
		 sprintf(uri, "%s", splitInput);
		 if(!strcmp(splitInput, "/") || !strcmp(splitInput, "/index.html")) {
			 printf("hit\n");
			 len = send(connectionSock, html_response, sizeof(html_response) - 1, 0);
			 bzero(buffer,sizeof(buffer));
			 fd = open("index.html", O_RDONLY);
			 while(read(fd, buffer, sizeof(buffer)) != 0) {
				 len = send(connectionSock, buffer, strlen(buffer), 0);
				 bzero(buffer,sizeof(buffer));
			 }

		 }
		 else {

			 if (splitInput[0] == '/') splitInput++;
			 sprintf(filePath, "%s", splitInput);
			 splitInput = strtok(filePath, "/");
			 sprintf(folderName, "%s", splitInput);
			 splitInput = strtok(NULL, "/");
			 sprintf(fileName, "%s", splitInput);

			 int uriLen = strlen(uri);
			 const char *last_four = &uri[uriLen-4];
			 printf("tyep?: %s\n", last_four);



			 for(i = uriLen; i >= 0; i--) {
				 if(!strncmp(&uri[i], ".", 1)) {
					 typeIndex = i;
					 printf("hit\n");
					 break;
				 }
			 }

			 sprintf(fileType, "%s", &uri[typeIndex + 1]);
			 printf("folderName: %s\n", folderName);
			 printf("fileName: %s\n", fileName);
			 printf("fileType: %s\n", fileType);
			 printf("uri: %s\n", uri);

			 bzero(filePath,sizeof(filePath));
			 strcat(filePath, ".");
			 strcat(filePath, uri);
			 printf("filePath: %s\n", filePath);


			if(!strcmp(fileType, "png")) {
			 len = send(connectionSock, png_response, sizeof(png_response) - 1, 0);
			}
			else if(!strcmp(fileType, "gif")) {
			 len = send(connectionSock, gif_response, sizeof(gif_response) - 1, 0);
			}
			else if(!strcmp(fileType, "html")) {
			 len = send(connectionSock, html_response, sizeof(html_response) - 1, 0);
			}
			else if(!strcmp(fileType, "txt")) {
			 len = send(connectionSock, text_response, sizeof(text_response) - 1, 0);
			}
			else if(!strcmp(fileType, "css")) {
			 len = send(connectionSock, css_response, sizeof(css_response) - 1, 0);
			}
			else if(!strcmp(fileType, "jpg")) {
			 len = send(connectionSock, jpg_response, sizeof(jpg_response) - 1, 0);
			}
			else if(!strcmp(fileType, "js")) {
			 len = send(connectionSock, js_response, sizeof(js_response) - 1, 0);
			}
			else if(!strcmp(fileType, "htm")) {
			 len = send(connectionSock, htm_response, sizeof(htm_response) - 1, 0);
			}

			 bzero(buffer,sizeof(buffer));
			 fd = open(filePath, O_RDONLY);
			 if(fd < 0) {
				 printf("Error opening file.\n");
				 return -1;
			 }

			 while(read(fd, buffer, sizeof(buffer)) != 0) {
				 printf("sending...\n");
				 printf("%s\n", buffer);
				 len = send(connectionSock, buffer, strlen(buffer), 0);
				 bzero(buffer,sizeof(buffer));
			 }

		 }
	}

	 close(sock);

}
