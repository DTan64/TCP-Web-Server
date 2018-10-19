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
	int readBytes;
	int typeIndex;
	struct stat st;
	int pid;

	char file_size[MAXBUFSIZE];
  FILE *requested_file;
  size_t read_bytes, total_read_bytes;

	char sendBuffer[MAXBUFSIZE];
	char contentLenght[] = "Content-Length: ";
	char png_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: image/png; charset=UTF-8\r\n";
  char gif_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: image/gif; charset=UTF-8\r\n";
  char html_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: text/html; charset=UTF-8\r\n";
  char text_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: text/plain; charset=UTF-8\r\n";
  char css_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: text/css; charset=UTF-8\r\n";
  char jpg_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: image/jpeg; charset=UTF-8\r\n";
  char js_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: text/javascript; charset=UTF-8\r\n";
	char htm_response[] = "HTTP/1.1 200 OK\r\n" "Content-Type: text/html; charset=UTF-8\r\n";

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&server_addr,sizeof(server_addr));                    //zero the struct
	server_addr.sin_family = AF_INET;                   //address family
	server_addr.sin_port = htons(5001);        //htons() sets the port # to network byte order
	server_addr.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine

	//Causes the system to create a generic socket of type TCP
	if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("unable to create socket");
	}

	//fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
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
		printf("accepting connection...\n");
	  connectionSock = accept(sock, (struct sockaddr *)&client_addr, (socklen_t *) &client_length);
		printf("Client connected: %d:%d\n", client_addr.sin_addr.s_addr, client_addr.sin_port);
		fflush(stdout);
	  if (connectionSock < 0) {
	      perror("ERROR on accept");
	      exit(1);
	   }

		 pid = fork();

		 if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }

			// Child process
			if (pid == 0) {

				/* If connection is established then start communicating */
	 		 bzero(buffer,sizeof(buffer));
	 		 //bzero(method,sizeof(method));
	 		 //bzero(splitInput,sizeof(splitInput));
	 		 bzero(fileName,sizeof(fileName));
	 		 bzero(folderName,sizeof(folderName));
	 		 bzero(filePath,sizeof(filePath));
	 		 bzero(fileType,sizeof(fileType));
	 		 bzero(uri,sizeof(uri));
	 		 printf("reading...\n");
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
	 			 bzero(sendBuffer,sizeof(sendBuffer));
	 			 strcat(sendBuffer, html_response);
	 			 strcat(sendBuffer, contentLenght);
	 			 if (stat("index.html", &st) == 0)
	 			 	 	 sprintf(file_size, "%lli", st.st_size);
	 			 strcat(sendBuffer, file_size);
	 			 strcat(sendBuffer, "\r\n\r\n");
	 			 printf("%s\n", sendBuffer);
	 			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			 bzero(buffer,sizeof(buffer));
	 			 fd = open("index.html", O_RDONLY);
	 			 while(read(fd, buffer, sizeof(buffer)) != 0) {
	 				 len = send(connectionSock, buffer, strlen(buffer), 0);
	 				 bzero(buffer,sizeof(buffer));
	 			 }
	 			 close(fd);

	 		 }
	 		 else {

	 			 if (splitInput[0] == '/') splitInput++;
	 			 sprintf(filePath, "%s", splitInput);
	 			 splitInput = strtok(filePath, "/");
	 			 sprintf(folderName, "%s", splitInput);
	 			 splitInput = strtok(NULL, "/");
	 			 sprintf(fileName, "%s", splitInput);

	 			 int uriLen = strlen(uri);

	 			 for(i = uriLen; i >= 0; i--) {
	 				 if(!strncmp(&uri[i], ".", 1)) {
	 					 typeIndex = i;
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
	 			  bzero(sendBuffer,sizeof(sendBuffer));
	  			  strcat(sendBuffer, png_response);
	  			  strcat(sendBuffer, contentLenght);
	  			  if (stat(filePath, &st) == 0)
	  			  	 	 sprintf(file_size, "%lli", st.st_size);
	  			  strcat(sendBuffer, file_size);
	  			  strcat(sendBuffer, "\r\n\r\n");
	  			  printf("%s\n", sendBuffer);
	  			  len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "gif")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, gif_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "html")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, html_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "txt")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, text_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "css")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, css_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "jpg")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, jpg_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "js")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, js_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}
	 			else if(!strcmp(fileType, "htm")) {
	 				bzero(sendBuffer,sizeof(sendBuffer));
	   			 strcat(sendBuffer, htm_response);
	   			 strcat(sendBuffer, contentLenght);
	   			 if (stat(filePath, &st) == 0)
	   			 	 	 sprintf(file_size, "%lli", st.st_size);
	   			 strcat(sendBuffer, file_size);
	   			 strcat(sendBuffer, "\r\n\r\n");
	   			 printf("%s\n", sendBuffer);
	   			 len = send(connectionSock, sendBuffer, strlen(sendBuffer), 0);
	 			}

	 			 bzero(buffer,sizeof(buffer));
	 			 fd = open(filePath, O_RDONLY);
	 			 if(fd < 0) {
	 				 printf("Error opening file.\n");
					 exit(0);
	 			 }

	 			 while(1) {
	 				 printf("sending...\n");
	 				 readBytes = read(fd, buffer, sizeof(buffer));
	 				 if(readBytes == 0) {
	 					 printf("break\n");
	 					 break;
	 				 }
	 				 len = send(connectionSock, buffer, readBytes, 0);
	 				 bzero(buffer,sizeof(buffer));
	 			 }
	 			 close(fd);

	 		 }

			 	printf("child exiting\n");
				exit(0);
      }
			close(connectionSock);
	}

	 close(sock);

}
