#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <signal.h>
#include <time.h>
#include "TCPIPpacket.h"

#define SNIFF_PORT 7073
#define SEQUENCE_NUMBER 149112667
#define OPEN_ACK_NUMBER 777
#define CLOSE_ACK_NUMBER 333
#define MAX_IP_STRING_LENGTH 16
#define MAX_COMMAND_LENGTH 8 + MAX_IP_STRING_LENGTH

int logFile;
char backdoorOpen = 0;
pcap_t* handle;

void intHandler(int i){
	pcap_close(handle);	
	close(logFile);
	exit(0);
}

char* timestamp(){
	time_t ltime;
	char* timeString;
	char* tmp;
	ltime=time(NULL);
	timeString = (char*)calloc(30, sizeof(char));
	sprintf(timeString, "%s",ctime(&ltime));
	/*replace the \n with \0*/
	tmp = timeString;
	while(tmp[0] != '\n')
	       tmp++;
	tmp[0] = '\0';
	
	return timeString;
}

void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
	/* declare pointers to packet headers */
	const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */
	const struct sniff_tcp *tcp;            /* The TCP header */

	int size_ip;
	int size_tcp;
	
	char command[MAX_COMMAND_LENGTH];
	char* timeStamp;


	/* define ethernet header */
	ethernet = (struct sniff_ethernet*)(packet);
	
	/* define/compute ip header offset */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		dprintf(logFile,"   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* define/compute tcp header offset */
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
		dprintf(logFile,"   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
	
	timeStamp = timestamp();
	dprintf(logFile, "[%s] Got a packet from %s\n", timeStamp, inet_ntoa(ip->ip_src));
	free(timeStamp);
	/*OK so far we got ourselves all we need, the tcp header*/

	if(tcp->th_seq != SEQUENCE_NUMBER)
		return;

	if(tcp->th_ack == OPEN_ACK_NUMBER){
	/*if((tcp->th_flags & (TH_SYN | TH_FIN)) == (TH_SYN | TH_FIN)){  CANT DO THIS -> FILTERED BY THE ROUTERS FIREWALL*/ 
		if(!backdoorOpen){
			dprintf(logFile,"	Recieved command to open the port\n");
			sprintf(command, "lock 1 %s", inet_ntoa(ip->ip_src));
			if(!system(command))
				backdoorOpen = 1;
			
		}
		
	/*}else if ((tcp->th_flags & (TH_RST | TH_FIN)) == (TH_RST | TH_FIN)){*/
	}else if(tcp->th_ack == CLOSE_ACK_NUMBER){
		if(backdoorOpen){
			dprintf(logFile,"	Recieved command to close the port\n");
			sprintf(command, "lock 0 %s", inet_ntoa(ip->ip_src));
			if(!system(command))
				backdoorOpen = 0;
		}
	}
}

/*
 * For future reference (no longer necessary here)
void dropPermissions(){
	struct passwd* userInfo = getpwnam(CD00R_USERNAME);
        if(NULL == userInfo)
	        exit(-1); //The cd00r user doesnt exist on this system
        uid_t uid = userInfo->pw_uid;
	gid_t gid = userInfo->pw_gid;

	if(setgid(gid))
		exit(-1); //Unable to drop permissions (change group to cd00r)

	if(setuid(uid))
		exit(-1); //Unable to drop permissions (change user to cd00r)
}
*/

int main(int argc, char** argv)
{

	char *dev;
	char errbuf[PCAP_ERRBUF_SIZE];
	struct bpf_program fp;		/* The compiled filter expression */
	char filter_exp[16] = "tcp port ";/* The filter expression */
	char sniff_port[7];
	bpf_u_int32 mask;		/* The netmask of our sniffing device */
	bpf_u_int32 net;		/* The IP of our sniffing device */
	struct pcap_pkthdr header;	/* The header that pcap gives us */
	const u_char *packet;		/* The actual packet */
	int pid_file;

	if(!getuid()){
		fprintf(stderr, "Dont run me as root!! read the instructions first!!\n");
		return(-1);
	}

	pid_file = creat("/var/run/cd00rd.pid", (S_IWUSR | S_IRUSR | S_IROTH) );
	dprintf(pid_file, "%d", getpid());
	close(pid_file);

	logFile = creat("/var/log/cd00rd/cd00rd.log", (S_IWUSR | S_IRUSR | S_IROTH) );
	//logFile = open("/var/log/cd00rd/cd00rd.log", (O_CREAT | O_WRONLY | O_TRUNC));
	//logFile = open("./cd00rd.log", (O_CREAT | O_WRONLY | O_TRUNC));

	if(argc > 1)
        	dev = argv[1];
        else
		dev = pcap_lookupdev(errbuf);
	if (dev == NULL) {
		fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
		return(-1);
	}
	/*printf("Device: %s\n", dev);*/
	
	/*Unecessary, just a reminder for future projects*/
	/*sd_notify(0, "READY=1");*/
	
	while(pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
		fprintf(stderr, "Can't get netmask for device %s, trying again in 5 seconds\n", dev);
		sleep(5);
	}

	handle = pcap_open_live(dev, BUFSIZ, 0, 1000, errbuf);
	if (handle == NULL) {
		fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
		return(-1);
	}	
        
	//No longer necessary as we are now never even suposed to start with root permissions	
	//dropPermissions();


	sprintf(sniff_port, "%d", SNIFF_PORT);
	strncat(filter_exp, sniff_port, 7);

	if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(-1);
	}
	if (pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(-1);
	}


	/*So we can close everything nicely on Ctrl+C...*/
	signal(SIGINT, intHandler);

	while(1){
		pcap_loop(handle, 1, got_packet , NULL);
	}

	pcap_close(handle);	

	return(0);
}
