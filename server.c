#include"packet.h"


int main(int argc, char *argv[])
{
	int count = 0;
	srand(time(0));
	int cl1Size,cl2Size;
	signal(SIGUSR1,signalHandler);
	signal(SIGINT,signalHandler);
	struct sockaddr_in selfAddr, client_1_Address, client_2_Address;
	signal(SIGPIPE,SIG_IGN);

	if((fileFD=open("New_file.txt",O_CREAT|O_WRONLY)) < 0)
	{
		perror("Error opening file");
		exit(0);
	}
	if((listener=socket(PF_INET,SOCK_STREAM,0)) < 0)
	{
		perror("Error creating listening socket");
		exit(0);
	}
	memset(&selfAddr,0,sizeof(selfAddr));
	selfAddr.sin_family=AF_INET;
	selfAddr.sin_port=htons(PORT_NO);
	selfAddr.sin_addr.s_addr=inet_addr(SERVER_IP_ADDRESS);
	if((bind(listener,(struct sockaddr*)&selfAddr,sizeof(struct sockaddr_in)))==-1)
	{
		perror("Error binding listening socket");
		if(-1==close(listener))
		{
			perror("Error closing listening socket");
		}
		if(-1==close(fileFD))
		{
			perror("Error closing file");
		}
		exit(0);
	}
	if(-1==listen(listener,2))
	{
		perror("Error calling listen");
		if(close(listener) < 0)
		{
			perror("Error closing listening socket");
		}
		if(close(fileFD) < 0)
		{
			perror("Error closing file");
		}
		exit(0);
	}
	cl1Size = sizeof(client_1_Address);
	cl2Size = sizeof(client_2_Address);
	int childProcess;
	while(1)
	{
		clientSocket1=accept(listener,(struct sockaddr*)&client_1_Address,&cl1Size);
		if(cl1Size<0)
		{
			perror("Error in accepting first socket");
			if(close(listener) < 0)
			{
				perror("Error closing listening socket");
			}
			if(close(fileFD) < 0)
			{
				perror("Error closing file");
			}
			exit(0);
		}
		else if((childProcess=fork())==0)
		{
			break;
		}
		if(-1==close(clientSocket1))
		{
			perror("Error closing channel in parent.");
		}
		clientSocket2=accept(listener,(struct sockaddr*)&client_2_Address,&cl2Size);
		if(cl2Size<0)
		{
			perror("Error in accepting second socket");
			kill(childProcess,SIGUSR1);
			if(close(listener) < 0)
			{
				perror("Error closing listening socket");
			}
			if(close(fileFD) < 0)
			{
				perror("Error closing file");
			}
			shutdown(clientSocket1,2);
			exit(0);
		}
		else if(fork()==0)
		{
			break;
		}
		if(close(clientSocket1) < 0)
		{
			perror("Error closing channel in parent.");
		}
	}
	if(close(listener) < 0)
	{
		perror("Error closing the listening socket in child process");
		if(close(clientSocket1) < 0)
		{
			perror("Error closing the socket");
		}
		if(close(fileFD) < 0)
		{
			perror("Error closing the output file");
		}
		return 0;
	}
	Packet p;
	SETNOTLASTPACKET(p);
	while(!p.lastPacket)
	{
		if(0==recv(clientSocket1,&p,sizeof(p),0))
		{
			break;
		}
		if(rand()/(RAND_MAX/100 + 1)<PDR)
		{
			printf("Dropping a packet.\n");
			SETNOTLASTPACKET(p);
			continue;
		}
		printf("RCVD PKT: Seq. No %d of size %d Bytes from channel %d.\n",p.seqNo,p.payloadSize,p.channelID);
		lseek(fileFD,p.seqNo,SEEK_SET);
		if(p.payloadSize > -1)
		{
			write(fileFD,p.payload,p.payloadSize);
		}
		SETACKPACKET(p);
		if(send(clientSocket1,&p,sizeof(p),0)==-1)
		{
			perror("Error sending ACK");
		}
		printf("SENT ACK: for PKT with Seq. No %d from channel %d.\n",p.seqNo,p.channelID);
	}
	if(close(clientSocket1) < 0)
	{
		perror("Error closing the socket");
	}
	if(close(fileFD) < 0)
	{
		perror("Error closing the output file");
	}
	return 0;
}
