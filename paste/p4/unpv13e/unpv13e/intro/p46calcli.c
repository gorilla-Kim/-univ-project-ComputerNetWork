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
	struct	result result1, result2;

	while(Fgets(sendline, MAXLINE, fp) != NULL){
		int n;
		n = sscanf(sendline, "%d",  &result2.sum);
        if(n>1 || n <0){
            printf("%d, 다시입력!!\n",n);
            continue;
        }
		Writen(sockfd, &result2, sizeof(result2));
		if(Readn(sockfd, &result1, sizeof(result1)) == 0)
			err_quit("str_cli: server terminated perematurely");
		printf("client recieve num : %d\n", result1.sum);
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
