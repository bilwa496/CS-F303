#include"packet.h"

#define RETRANSMISSIONRATE 4


int main(int argc, char *argv[])
{
	if(argc==1)
	{
	    printf("No input file name was given. \n");
		exit(0);
	}
	int fileFD, endOfFile;
	if((fileFD=open(argv[1],O_RDONLY))==-1)
	{
	    perror("Error opening input file");
		exit(0);
	}
	signal(SIGPIPE,SIG_IGN);
	endOfFile=lseek(fileFD,0,SEEK_END);
	lseek(fileFD,0,SEEK_SET);
	int clientSocket;
	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));
	serverAddress.sin_family=AF_INET;
	serverAddress.sin_port=htons(PORT_NO);
	serverAddress.sin_addr.s_addr=inet_addr(SERVER_IP_ADDRESS);
	Packet p;
	p.channelID=fork()==0?0:1;
	clientSocket=socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	fd_set fdset;
	SETACKPACKET(p);
	SETNOTLASTPACKET(p);
	struct timeval timeout;
	FD_ZERO(&fdset);
	if(connect(clientSocket,(struct sockaddr*)&serverAddress,sizeof(struct sockaddr_in)) < 0)
	{
	    perror("Error creating a connection");
	    exit(0);
	}
	while(!p.lastPacket)
	{
		p.seqNo=lseek(fileFD,0,SEEK_CUR);
		if(p.seqNo==endOfFile)
		{
			break; 
		}
		p.payloadSize=read(fileFD,p.payload,PACKET_SIZE);
		if(p.seqNo+p.payloadSize==endOfFile)
		{
			SETLASTPACKET(p);
		}
		SETDATAPACKET(p);
		while(1){
			timeout.tv_sec=RETRANSMISSIONRATE;
			timeout.tv_usec=0;
			FD_SET(clientSocket,&fdset);
			if(send(clientSocket,&p,sizeof(Packet),0) < 0)
			{
				perror("Error sending packet");
				exit(0);
			}
			printf("SEND PKT: Seq. No %d of size %d Bytes from channel %d.\n",p.seqNo,p.payloadSize,p.channelID);
			if(0!=select(clientSocket+1,&fdset,NULL,NULL,&timeout)&&FD_ISSET(clientSocket,&fdset)) break;
		}
		recv(clientSocket,&p,sizeof(p),0);
		printf("RCVD ACK: for PKT with Seq. No %d from channel %d.\n",p.seqNo,p.channelID);
	}
	if(close(clientSocket) < 0)
	{
	    perror("Error closing a socket");
	}
	if(close(fileFD) < 0)
	{
		perror("Error closing input file");
	}
	return 0;
}
