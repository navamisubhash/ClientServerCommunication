#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
int sd, csd, portNumber, status;
socklen_t len;
struct sockaddr_in servAdd;//ipv4

if((sd = socket(AF_INET, SOCK_STREAM, 0))<0){
printf("Cannot create socket\n");
exit(1);
}
//Add information to the servAdd struct variable of type sockaddr_in
servAdd.sin_family = AF_INET;
//When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.
//The htonl function takes a 32-bit number in host byte order and returns a 32-bit number in the network byte order used in TCP/IP networks
servAdd.sin_addr.s_addr = htonl(INADDR_ANY); //Host to network long 

sscanf(argv[1], "%d", &portNumber);
//uint16_t: optional integer type of exactly 16 bits
//htonos: Host to network short-byte order
servAdd.sin_port = htons((uint16_t)portNumber); 

//struct sockaddr is the generic structure to store socket addresses
//The procedure it to typecast the specific socket addreses of type sockaddr_in, sockaddr_in6, sockaddr_un into sockaddr

bind(sd,(struct sockaddr*)&servAdd,sizeof(servAdd));
listen(sd, 5);

while(1){
csd = accept(sd,(struct sockaddr *)NULL,NULL);
printf("Got a client\n");
if(!fork())//Child process 
handle_client(csd);
close(csd);
waitpid(0, &status, WNOHANG); // waitpid? 
}
}

void handle_client(int csd1){
char messageSocket[255];
char patternSocket[255];
char buffer[255];
char buffer2Socket[255];

int socketWords, socketWords2;
int n, pid, pid2, sid, ch = 0;
char c, ch2;


FILE *fp1, *fp2;

write(csd1, "start chatting", 16);

/* here */
if(pid=fork()) /* reading csd messages */
{	
	while(1)
	{
		if(n=read(csd1, patternSocket, 255)) {
			//reading the pattern to be grepped from the socket
		}
		if(n=read(csd1, messageSocket, 255))
		{						
			messageSocket[n]='\0';
			if(!strcasecmp(messageSocket, "Bye\n")){
				kill(pid, SIGTERM);
				exit(0);
			}

			else if (!strcasecmp(messageSocket, "File\n")) {					
				fp1 = fopen("client_file_received.txt", "a");
				fp1 = fopen("grep_output.txt", "w");

				read(csd1, &socketWords, sizeof(int));
					
				while(ch != socketWords) {
					read(csd1, buffer, 255);
					fprintf(fp1, "%s ", buffer);
					ch++;
				}
					
				rewind(fp1);			
					
				pid2 = fork();
				if(pid2==0) { // child
					execlp("grep","grep","-w", "--color=auto",patternSocket,"client_file_received.txt","grep_output.txt", NULL);
				} 

				else if(pid2) { //parent
					int child_status;	

					waitpid(pid2, &child_status, 0);
					fp2 = fopen("grep_output.txt", "r");
					if (fp2 == NULL) {
						fprintf(stderr, "File 2 doesnt exist\n");
						return;
					}
					while((c = getc(fp2)) != EOF) {
						fscanf(fp2, "%s", buffer2Socket);
						if(isspace(c) || c=='\t')
							socketWords2++;						
					}
					write(csd1, &socketWords2, sizeof(int));
					rewind(fp2);				
					while(ch2 != EOF) {
						fscanf(fp2, "%[^\n]", buffer2Socket);
						write(csd1, buffer2Socket, 255);
						ch2 = fgetc(fp2);
					}																
				}					
			}
		}
	}
}
else
{
while(1)// sending messages csd
{
if(n=read(0, messageSocket, 255)){
	messageSocket[n]='\0';
	write(csd1, patternSocket, strlen(patternSocket)+1);
	write(csd1, messageSocket, strlen(messageSocket)+1);
	if(!strcasecmp(messageSocket, "Bye\n")){
		kill(getppid(), SIGTERM);
		exit(0);
	}
	else if (!strcasecmp(messageSocket, "File\n")) {				
		fp1 = fopen("client_file_received.txt", "a");
		fp1 = fopen("grep_output.txt", "w");

				
		read(csd1, &socketWords, sizeof(int));
					
		while(ch != socketWords) {
			read(csd1, buffer, 255);
			fprintf(fp1, "%s ", buffer);
			ch++;
			}
			rewind(fp1);
					
					/*pid2 = fork();
					if(pid2==0) { // child
						fprintf(stderr, "inside child");
						execlp("grep","grep","-w", "--color=auto",pattern,"client_file_received.txt","grep_output.txt", NULL);
						fprintf(stderr,"end of child loop");
					} else if(pid2) { //parent
					int child_status;					
						fprintf(stderr, "inside parent");
						Waitpid(pid2,&child_status,0);
						fprintf(stderr,"end of parent loop");
						
					}*/
					
						//execlp("grep","grep","-w", "--color=auto",pattern,"client_file_received.txt",">","grep_output.txt", NULL);
					//execlp("ls","ls","-1", NULL);
					//execlp("cat","cat","client_file_received.txt", NULL);
					//exit(0);
	}

}
}
}
}

