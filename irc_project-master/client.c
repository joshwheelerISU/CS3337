#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>


//Joshua Wheeler, 2/23/20 - 5/3/2020. Written by heavily leaning on code done in class by Professor McGregor.
//Programmed on Lubuntu using GCC, Use flag -lpthread. No IDE used.


void* receive(void* arg);

void error(char *msg)
{
	perror(msg);
	exit(0);
}

int main(int argc, char **argv)
{
	int sockdesc, port, n;
	
	struct sockaddr_in serv_addr;
	struct hostent *server;
	
	char buffer[256];
	
	//catch wrong usage
	
	if(argc < 2)
	{
		fprintf(stderr,"Usage: HOSTNAME PORT\n");
		exit(0);
	} 
	
	//pull in connection info
	port = atoi(argv[2]);
	sockdesc = socket(AF_INET, SOCK_STREAM, 0);
	
	//catch issue with provided socket description
	if(sockdesc < 0)
	{
		error("ERROR: Could not open socket!\n");
	}
	
	server = gethostbyname(argv[1]);
	
	if(server == NULL)
	{
	
		fprintf(stderr, "ERROR: No server!\n");
		exit(0);
	
	}
	
	//zero out
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	serv_addr.sin_family = AF_INET;
	
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(port);
	
	//start seperate thread to listen for incoming messages
	
	pthread_t pthread;
	
	int argpass;
	
	pthread_create(&pthread, NULL, receive, (void *)sockdesc);
	
	
	
	//begin transmission loop
	
	
	if (connect(sockdesc,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		error("ERROR: Could not Connect!");
	}
	
	printf("Chatting normally. Type !HELP for a list of commands.\n");
	
	while (1)
	{
		
		
		//zero out the buffer
		bzero(buffer, 256);
		fgets(buffer, 256, stdin);
		
		//write to server
		n = write(sockdesc, buffer, strlen(buffer));
		
		//collect errors
		if (n < 0)
		{
			error("ERROR writing to socket");
		}
		
		//rezero buffer
		bzero(buffer,256);
		
		
	}
	return 0;
}

void* receive(void* args)
{

	int sockdesc = (int)args;
	char buffer[256];
	int n;


	while(1)
	{
	//read from server
		n = read(sockdesc,buffer,256);
		
		
		//decide if transmission was received
		if (n < 0)
		{
			error("ERROR reading from socket");
		}
		printf("%s\n",buffer);
		bzero(buffer, 256);
	}
	
	return NULL;

}

