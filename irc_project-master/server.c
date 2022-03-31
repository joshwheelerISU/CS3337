#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#include <time.h>
#include <assert.h>


//Joshua Wheeler, 2/23/20 - 5/3/2020.


//Programmed on Lubuntu using GCC. No IDE used. Make sure to compile with gcc flag lpthread.

//I did enjoy working on this project. It isn't as complete as I would have hoped, but I do plan on polishing it up over the summer to help build resume experience.

//REQUIRED THINGS THAT I DIDNT GET TO

//Does not support channels, and as such, pretty much all of the commands are supported but unemplemented. The framework is there, with another deliverable I could have gotten it done, that's on me for not managing my time effectively. 

//The config file works, and is easily extendable to include other parameters.

//Command line arguments work and are also easily extendable. 

void* newConnection(void* arg);
void* transmissionLoop(void* arg);

//global variables

pthread_mutex_t buff;
char global_buffer[256];
bool global_Transmit = false; 
struct Node* activeClients;

bool detailedlogging = false;

int client_count = 0;
int firstsockdesc;

void errorHandler(char *msg)
{

	perror(msg);
	exit(1);

}

int main(int argc, char *argv[])

{

	//set up lists & info
	
	struct clientInfo
	{
		char nick[32];
		char full[15];
		char ipAddr[32];
		char hostn[32];
		int sockdesc;
		
		//anything else i need to keep track of per client
		
	
	};
	
	struct channelInfo
	{
		char name[32];
		char topic[32];
		
		struct Node* headOpList;
		


		//anything else i need to keep track of per channel
		
	};
	
	
	struct Node* headClientInfo;
	struct Node* headChannelInfo;
	
	
	
	

	//variable declaration
	int sockdesc, newsockdesc, port, clien, n;
	
	char buffer[256];
		
	
	struct sockaddr_in serv_addr, cli_addr;

		//config file area
		
		//help found at https://is.gd/ArnyiR
		
		FILE *file = fopen("config.txt", "r");
		
		char currentline[100];
		
		assert(file != NULL);
		
		while(fgets(currentline, sizeof(currentline), file) != NULL)
		{
		
			//interpret the line or toss it out as junk
			
			printf("Received config line: %s\n", currentline);
			
			//best option i can think of to do here is a giant if / else statement, couldn't make a switch work
		
			char input[24];
		
			strcpy(input, currentline);
			
			
			char *token;
			token = strtok(input, " ");
			
			if(strcmp(token, "detailedlogging")==0)
			{
				//check for true or false on next token
				
				
				char *mod = strtok(NULL, ";");
				
				if(strcmp(mod, "true") == 0)
				{
				
					detailedlogging = true;
					
					printf("Detailed Logging Enabled!\n");
				
				}
				
			}else if(strcmp(token, "port")==0)
			{
			
				char *mod = strtok(NULL, ";");
				
				int interim = atoi(mod);
				
				if(interim == 0)
				{
					printf("Invalid Port Provided! Falling back to 25565!\n");
				}else
				{
					port = interim;
					
					printf("Valid port found in config file!\n");
				}
				
			}else
			{
			
			//no case met, toss the line
				printf("Found useless line in config file.\n");
			}
			}
			
			//   ===== EXTEND AS NEEDED ===	
		
		fclose(file);
	//error checking
	
	if(argc<2) //check if we got arguments
	{
		fprintf(stderr, "No command line arguments specified, using config file input...\n For future reference, if you want to invoke a command line argument, simply use a - to flag the type of command followed next by the value you wish to modify.\n (Currenty the only thing that works is -p, which modifies the port you wish to use.\n" );
	 
	
	}else
	{
		//read necessary parameters from command line
		int i;
		for(i = 1; i < argc; i++)
		{
		
			char argument[30]; 
			strcpy(argument, argv[i]);

			
			if(argument[0] == '-')
			{
				//got a command
				//determine which one we got
		
				if(argument[1] == 'p')
				{
					//port command
					

					char foundport[12];
					strcpy(foundport, argv[i+1]);

					port = atoi(foundport);

					
					printf("Using port from command line override!\n");
					
				}
				//extend as needed
				else
				{
					printf("Useless command line argument provided!\n");
				}
			}else
			{
				//no command for received parameter
			}
			
		}

	}
		
	sockdesc = socket(AF_INET, SOCK_STREAM, 0);
	
	
	if(sockdesc < 0)
	{
		error("ERROR: Couldn't open socket.");
	}
	
	bzero((char *) &serv_addr, sizeof(serv_addr));
	
	
	//pull and assign listening port

	
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);
	
	//bind
	
	bind(sockdesc, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	
	printf("Server started on port %d\n", port);
		
	
	FILE *f = fopen("server.log", "a");
	fprintf(f, "NEW INSTANCE=========================================== \nStarted server. Listening on port %d\n\n\n\n", port);
	fclose(f);
	
	//listen loop
	
	listen(sockdesc, 5);
	clien = sizeof(cli_addr);
	
	while(1) 
	
	{
	
		pthread_t pthread;
		
		newsockdesc = accept(sockdesc, (struct sockaddr *) &cli_addr, &clien);
		
		if(newsockdesc < 0)
		{
			error("ERROR: client wasn't accepted! \n");
		}
		
		
		//add sockdesc to list of clients
		
		printf("Client with FD %d connected. \n", newsockdesc);
		
		size_t size1 = sizeof(newsockdesc);
		
		//push(&activeClients, &newsockdesc, size1);
		
		pthread_mutex_lock(&buff);
		
		FILE *f = fopen("server.log", "a");
		fprintf(f, "Client with SFD %d Connected.\n\n\n\n", newsockdesc);
		fclose(f);
		
		pthread_mutex_unlock(&buff);
		
		
		if(client_count == 0)
		{
			
			firstsockdesc = newsockdesc;
		
		}
		
		client_count++;
		
		//spin off new thread
		pthread_create(&pthread, NULL, newConnection, (void *)newsockdesc);

	}
	
	return 0;
	
}

void* transmissionLoop(void* arg)
{


}

void* newConnection(void* arg)
{

	char buffer[256];
	char channel[128] = "UNSPECIFIED";
	int n;
	int newsockdesc = (int)arg;
	char nick[24] = "NOTSET: ";

	
	bool closeconnec = false;
	
	//main client / server loop
	
	static char timestamp[12] = {0};
	
	
	while(closeconnec == false)
	{	
		//wipe buffer
		bzero(buffer, 256);
		
		int l;
		l = read(newsockdesc, buffer, 256);
		
		//error handling
		
		if (l < 0)
		{
			error("ERROR: Could not read from socket!\n");
			
			
		}		
		//see if received message was a command
		
		if(buffer[0] == '!')
		{
		
			//command logging here
			
			pthread_mutex_lock(&buff);
		
			FILE *f = fopen("server.log", "a");
			fprintf(f, "Command %s received from SFD %d.\n", buffer, newsockdesc);
			fclose(f);
			
			pthread_mutex_unlock(&buff);
			
			
			//start logic to determine which command we received
			
			char *token;
			token = strtok(buffer, " ");
			
		
			
				if(strcmp(token, "!NICK")==0)
				{
					//nick command
					
					char *param;
					param = strtok(NULL, " ");
					int i;
					for(i=0; i<=12; i++)
  					{
   						if(param[i]=='\n')
      					{
        				param[i]=':';
       					}
      				}
      			
					
					printf("Received nickname %s.\n", param);
					
					
					
					strncpy(nick, param, 6);
					
					strcat(nick, " ");
					
					//pthread_mutex_lock(&buff);
					
					//logging
		
		
		/**
					FILE *f = fopen("server.log", "a");
					fprintf(f, "SFD #%d set own nickname to %s.\n", newsockdesc, nick);
					fclose(f);
					
					pthread_mutex_unlock(&buff);
					**/
					
				}
				
			
				else if(strcmp(token, "!USER")==0)
				{
					printf("USER \n");
				
				}
				
				else if(strcmp(token, "!OPER")==0)
				{
					//make user an operator if they have the oper password
				
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !OPER [USERNAME] [PASSWORD] \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!QUIT")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !QUIT [MODE] (UNEMPLEMENTED!) \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}else
					{
						//shutdown(newsockdesc, 2);
					}
				
				
				}else if(strcmp(token, "!JOIN")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !JOIN [CHANNEL] (UNEMPLEMENTED!) \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!PART")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !PART [CHANNEL] (UNEMPLEMENTED!)\n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!TOPIC")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !TOPIC [DESCRIPTION]  (UNEMPLEMENTED!)  \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!NAMES")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !NAMES [CHANNEL] (UNEMPLEMENTED!)  \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!LIST")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !LIST  (UNEMPLEMENTED!)  \n");
						int l = write((newsockdesc), message, strlen(message));
						
						
					}
				
				}else if(strcmp(token, "!INVITE")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !INVITE [USERNAME]  (UNEMPLEMENTED!)  \n");
						int l = write((newsockdesc), message, strlen(message));
					}
				
				}else if(strcmp(token, "!KICK")==0)
				{
					//if client is op, kick the user specified
				
				}else if(strcmp(token, "!PRIVMSG")==0)
				{
					char *argument;
					argument = strtok(NULL, " ");
					
					if(argument == NULL)
					{
						//use case message
						char *message[256];
						strcpy(message, "Use guide: !PRIVMSG [USER] (UNEMPLEMENTED!) \n");
						int l = write((newsockdesc), message, strlen(message));						
					}
				
				}else if(strcmp(token, "!HELP")==0)
				{
					char *param;
					param = strtok(NULL, " ");
					
					printf("help comand received\n");
				
					char *header[128];
					strcpy(header, "For detailed use guide, enter any command without any arguments IE !OPER .\nMAKE SURE TO FOLLOW COMMANDS WITH A SPACE. \nTo navigate a list of available commands, use !HELP [1-3]. (Ignore Brackets)\n ");
					int l = write((newsockdesc), header, strlen(header));

					if(strcmp(param, "1\n")==0)
					{
						char *message[256];
						strcpy(message, "Valid Commands:\nPRIVMSG: Send a private message.\nKICK: Op command, kicks a specified user.\nINVITE: Used to invite a user to a channel.\nLIST: Used to list available channels.\nNAMES: Provides a list of names in the server.\nPAGE 1 of 3.");
						int l = write((newsockdesc), message, strlen(message));
						
					}else if (strcmp(param, "2\n")==0)
					{
						char *message[256];
						strcpy(message, "Valid Commands:\nTOPIC: Allows user to view channel topic or Operator to set channel topic.\nMODE: Allows a user to set their mode or an OP to set the channel mode.\nPART: Allows user to leave a channel.\nJOIN: Allows user to join channel.\nPAGE 2 of 3.");
						int l = write((newsockdesc), message, strlen(message));
						
					}else if (strcmp(param, "3\n")==0)
					{
						char *message[256];
						strcpy(message, "Valid Commands:\nUSER: Access the login system. \nQuit: Leave the server completely.\nOPER: Elevate to Operator State.\nPAGE 3 of 3.");
						int l = write((newsockdesc), message, strlen(message));
						
					}
									
				}
								
				else
				{
				
					//invalid command
					printf("Invalid command %s received! Use command !HELP for a list. MAKE SURE YOU INCLUDE A SPACE AFTER EVERY COMMAND.\n", token);
					
					char *message[256];
					strcpy(message, "Invalid command! Use command !HELP for a list of valid commands.\n");
					int l = write((newsockdesc), message, strlen(message));
			
				}
						
		}else
		{
		
		//receive / transmit
		printf("Message received From client FD %d, routing to chat %s: %s\n",newsockdesc, channel, buffer);
		
		pthread_mutex_lock(&buff);
		
		//logging
		
		FILE *f = fopen("server.log", "a");
	
		//error handling for log file
		char *loghead[256];

		int g = sprintf(loghead, "Beginning log of message %s from client SFD %d.\n", buffer, newsockdesc);
		 
		fprintf(f, loghead);
				
		int sockdesc3 = firstsockdesc;
		int iterator = 0;
			
		//loop through all connected clients and send the messages around
		
			
			while(iterator < client_count)
			{
			
			
				//int sockdesc2 = (int)transmitHere->data;
				
				printf("Transmitting to SFD %d\n", (iterator +sockdesc3));
										
				//complex log
				
				//note: implement nicknames so that the clients can distinguish who's saying what
				
				char *buffernick[269];
				
				strcpy(buffernick, nick);
				
				strcat(buffernick, buffer);
								
				int l = write((sockdesc3 + iterator), buffernick, strlen(buffernick));
				//	int l = write(newsockdesc, "Message Received", 15);
									
				if(detailedlogging == true)
				{
				
				fprintf(f, "Transmitting to SFD %d...",  (iterator +sockdesc3));
				
					if (l<0)
					{
						
						fprintf(f, "Transmit failed! Moving on. \n");
			
					}else
					{
					
						fprintf(f, "Success!\n");
					
					}
							
			}				
				iterator = iterator + 1;
				bzero(buffernick, 269);
			}
						
			//complex log
						
			bzero(buffer, 256);
						
			//end log
						
			fprintf(f, "\n\n\n");
			
			fclose(f);
			
			pthread_mutex_unlock(&buff);
			}
		
		}					
	return 0;			
}
