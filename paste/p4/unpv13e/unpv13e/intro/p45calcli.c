#include	"unp.h"
#include <stdint.h>
struct args {
	int32_t	arg1; // int32_t 가 아닌 int를 쓰면 endian 불일치 문제가 발생합니다.
	int32_t	arg2; // 이하동일
};

struct result {
	int32_t	sum; // 이하동일
};


void
str_echo (int sockfd)
{

	ssize_t	n;
	struct	args args;
	struct	result result;

	for(;;){
		if((n = Readn(sockfd, &args, sizeof(args)))==0)
			err_quit("str_cli: server terminated perematurely");
        
		result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
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

	str_echo(sockfd);

	exit(0);
}