#include	"unp.h"

struct args {
	int32_t	arg1;
	int32_t	arg2;
};

struct result {
	int32_t	sum;
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
str_echo (int sockfd)
{

	ssize_t	n;
	struct	args args;
	struct	result result;

	for(;;){
		if((n = Readn(sockfd, &args, sizeof(args)))==0)
			return;
		
		result.sum = args.arg1 + args.arg2;
		Writen(sockfd, &result, sizeof(result));
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
			str_echo(connfd);
			exit(0);

		}
		Close(connfd);
	}
}
