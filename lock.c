#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <arpa/inet.h>

#define IP_STRING_LENGTH 16
#define COMMAND_MAX_LENGTH 70
#define MIN_ACCEPTABLE_ARGUMENTS 3
#define ARGUMENT_IP 2
#define ARGUMENT_OPEN_CLOSE 1
#define ALLOWED_USERNAME "cd00r"

/*
 * Simple program to open and close port 22
 * Its executable file should be marked with setuid
 * And this program should only be run by cd00rd 
 * (hence the puid should be set to cd00r at program launch)
 * The return values are all 1 for failure on purpose
 */

int main(int argc, char** argv){

	struct passwd* allowedUserInfo = getpwnam(ALLOWED_USERNAME);
	if(NULL == allowedUserInfo)
		return 1; //The allowed user doesnt exist on this system

	uid_t allowedUser = allowedUserInfo->pw_uid;

	uid_t parentUser = getuid();

	if(parentUser != allowedUser)
		return 1;

	if(argc < MIN_ACCEPTABLE_ARGUMENTS)
		return 1;

	if(strlen(argv[ARGUMENT_OPEN_CLOSE]) > 1)
		return 1;
	
	char ip[IP_STRING_LENGTH];
	if(snprintf(ip, IP_STRING_LENGTH, "%s", argv[ARGUMENT_IP]) < 0)
		return 1;
	
	struct sockaddr_in address;
	if( 1 != inet_pton(AF_INET, ip, &address))
		return 1;//Argument is not a valid ip address


	char command[COMMAND_MAX_LENGTH];
	if(argv[ARGUMENT_OPEN_CLOSE][0] == '1'){
		snprintf(command, COMMAND_MAX_LENGTH, "iptables -I TCP -p tcp --dport 22 -s %s -j ACCEPT", ip);
		setuid(0);	
		return system(command);
	}else if(argv[ARGUMENT_OPEN_CLOSE][0] == '0'){
		snprintf(command, COMMAND_MAX_LENGTH, "iptables -D TCP -p tcp --dport 22 -s %s -j ACCEPT", ip);
		setuid(0);	
		return system(command);
	}else
		return 1;
}
