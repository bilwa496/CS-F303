#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<string.h>
#include<signal.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include<stdlib.h>
#include<time.h>
#include<fcntl.h>


#ifndef MESSAGEDEF_H
#define MESSAGEDEF_H

#define PACKET_SIZE 1000
#define PORT_NO 9000
#define SERVER_IP_ADDRESS "127.0.0.1"

int clientSocket1,clientSocket2;

typedef struct{
	int payloadSize;
	int seqNo;
	unsigned int lastPacket:1;
	unsigned int dataPacket:1;		
	unsigned int channelID:1;		
	char payload[PACKET_SIZE];
}Packet;

#define SETDATAPACKET(p) p.dataPacket=1
#define SETACKPACKET(p) p.dataPacket=0
#define SETLASTPACKET(p) p.lastPacket=1
#define SETNOTLASTPACKET(p) p.lastPacket=0
#endif

#define PDR 10			
int fileFD,listener;

void signalHandler(int sig)
{
	if(sig==SIGUSR1)
		close(clientSocket1);
	close(fileFD);
	close(listener);
	exit(0);
}