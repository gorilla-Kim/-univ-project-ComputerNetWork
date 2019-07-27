#include	"unp.h"
#include <stdint.h>
struct args {
	int32_t	arg1;
	int32_t	arg2;
};

struct result {
	int32_t	sum;
};

void
str_cli (FILE *fp, int sockfd)
{

	char 	sendline[MAXLINE];
	struct	args args;
	struct	result result;

	while(Fgets(sendline, MAXLINE, fp) != NULL){
		
		if(sscanf(sendline, "%d%d",  &args.arg1 , &args.arg2) != 2){
			printf("invalid input: %s", sendline);
			continue;
		}
		Writen(sockfd, &args, sizeof(args));

		if(Readn(sockfd, &result, sizeof(result)) == 0)
			err_quit("str_cli: server terminated perematurely");
		printf("%d\n", result.sum);
	}
}

int
main(int argc, char **argv)
{
	int	sockfd;
	struct	sockaddr_in servaddr;
	
	if(argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET,SOCK_STREAM,0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *)&servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);

	exit(0);
}
