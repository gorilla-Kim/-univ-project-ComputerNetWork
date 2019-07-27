#include "unp.h"

int main(int argc, char** argv){

	int n, sockfd;
	char recvline[MAXLINE+1];
	struct sockaddr_in servaddr;

	if(argc != 2)
		err_quit("err1");
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
		err_sys("err2");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(13);
	
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
		err_quit("err3");
	if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr))<0)
		err_sys("err4");

	while((n=read(sockfd, recvline, MAXLINE))>0){

		recvline[n]=0;
		
		if(fputs(recvline, stdout)==EOF)
			err_sys("err5");
	}
	if(n<0)
		err_sys("err6");
	exit(0);
}
