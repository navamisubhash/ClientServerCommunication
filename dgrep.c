#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/signal.h>
#include <ctype.h>

int main(int argc, char *argv[]){//E.g. 3 Client
for(int i=1; i<argc; i++) {
	//printf("\narg%d=%s",i,argv[i]);
}

char messageSocket[255];
char buffer2Socket[255];
char buffer[255];

socklen_t len;
struct sockaddr_in servAdd;

int sid, portNumber, pid, pid2, n, socketWords = 0, socketWords2, length =0, n2;

char c;

FILE *f, *fc, *f2, *f3;

if(argc != 6){
printf("Call model: %s <IP> <Port #>\n", argv[0]);
exit(0);
}
if((sid=socket(AF_INET, SOCK_STREAM, 0))<0){
fprintf(stderr, "Cannot create socket\n");
exit(1);
}
servAdd.sin_family = AF_INET;
sscanf(argv[2], "%d", &portNumber);
servAdd.sin_port = htons((uint16_t)portNumber);

//The inet_pton() function converts an Internet address in its standard text format into its numeric binary form
if(inet_pton(AF_INET, argv[1],&servAdd.sin_addr)<0){
fprintf(stderr, " inet_pton() has failed\n");
exit(2);
}
if(connect(sid, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
fprintf(stderr, "connect() has failed, exiting\n");
exit(3);
}

read(sid, messageSocket, 255);
//printf("pattern = %s, file1= %s, file2 = %s", pattern, file1, file2);
fprintf(stderr, "Message Received from Server: %s\n", messageSocket);
pid=fork();

if(pid)//Parent
{
	while(1) /* reading sid's messages */
	{
		if(n=read(sid, messageSocket, 255))	{
			messageSocket[n]='\0';
			fprintf(stderr,"%s\n", messageSocket);
			if(!strcasecmp(messageSocket, "Bye\n")) {
				kill(pid, SIGTERM);//pid=process id of the child
				exit(0);
			}
		}
	}
}
else 
{	
	while(1) // sending messages to sid
	{	
		write(sid, argv[3], strlen(argv[3])+1);
		if(n=read(0, messageSocket, 255)) {
			messageSocket[n]='\0';
			write(sid, messageSocket, strlen(messageSocket)+1);
				
			if(!strcasecmp(messageSocket, "Bye\n")) {
				kill(getppid(), SIGTERM);//getppid()=process id of the parent
				exit(0);
			}
			else if(!strcasecmp(messageSocket, "File\n")) {
				f = fopen(argv[5], "r");
				if (f == NULL) {
					fprintf(stderr, "File 2 doesnt exist\n");
					//exit(0);
					return;
				}
				while((c = getc(f)) != EOF) {
					fscanf(f, "%s", buffer);
					if(isspace(c) || c=='\t')
						socketWords++;						
				}
				write(sid, &socketWords, sizeof(int));
				rewind(f);
					
				char ch;
				while(ch != EOF) {
					fscanf(f, "%s", buffer);
					write(sid, buffer, 255);
					ch = fgetc(f);
				}
					
				//client grep operation
				f2 = fopen(argv[4], "r");
				f3 = fopen("client_final.txt","w");
				if (f2 == NULL) {
					fprintf(stderr, "File 1 doesnt exist\n");
					//exit(0);
					return;
				}
					
				//CHANGES
				pid2 = fork();
				if(pid2==0) { // child
					execlp("grep","grep","-w", "--color=auto",argv[3],argv[4],"client_final.txt", NULL);
				} 
					
				else if(pid2) { //parent
				int child_status;	
				waitpid(pid2, &child_status, 0);
					
					/*fc = fopen("client_final.txt","a");
					rewind(f3);
					char ch2;
					read(sid, &words2, sizeof(int));
					while(ch2 != words2) {
						read(sid, buffer2Socket, 255);
						fprintf(f3, "%s ", buffer2Socket);
						ch2++;
					}*/
					
					
				}
					
					//END CHANGES
					
					
					//execlp("grep","grep","-w", "--color=auto",argv[3],argv[4], NULL);
					
					
					
					//printf("File successfully transferred");
				}
			//}
		}
		
		//continue;
		
		
	}
}
}//End main