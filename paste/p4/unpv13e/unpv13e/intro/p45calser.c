#include	"unp.h"

struct args {
	int32_t	arg1; // int32_t 가 아닌 int를 쓰면 endian 불일치 문제가 발생합니다.
	int32_t	arg2; // 이하동일
};

struct result {
	int32_t	sum;  // 이하동일
};
void
sig_chld(int signo)
{

	pid_t	pid;
	int	stat;
	
	while( (pid = waitpid(-1, &stat, WNOHANG) ) > 0)
		printf("child %d terminated\n", pid);
	return;
}
void
str_cli (FILE *fp, int sockfd)
{
	char 	sendline[MAXLINE];
	struct	args args;
	struct	result result;
    printf("insert number\n");

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
main(int argc, char** argv)
{
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld(int);
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0 );

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	Signal(SIGCHLD, sig_chld);

	for(;;){
		clilen = sizeof(cliaddr);

		if( (connfd = accept(listenfd, (SA*)&cliaddr, &clilen) ) < 0){
			
			if(errno == EINTR)
				continue;
			else
				err_sys("accept error");
		}
		
		if( (childpid = Fork()) == 0){

			Close(listenfd);
			str_cli(stdin, connfd);
			exit(0);

		}
		Close(connfd);
	}
}