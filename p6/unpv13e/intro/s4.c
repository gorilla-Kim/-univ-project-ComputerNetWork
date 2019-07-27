#include	"unp.h"
#include	<time.h>
#include    <stdlib.h>

void
sig_chld(int signo)
{
    pid_t    pid;
    int     stat;
    
    while((pid=waitpid(-1, &stat, WNOHANG))>0)
        printf("child %d terminated\n", pid);
    return;
}

void
str_echo(int sockfd){
    ssize_t n;
    char buff[MAXLINE],recvline[MAXLINE+1];
    
    for(;;){
        if((n = Readline(sockfd, recvline, MAXLINE))==0){
            printf("client terminated\n");
            return;
        }
        /* 데이터 전송구간 */
        printf("send> ");
        srand(time(NULL));                // 난수 발생
        sprintf(buff,"%d",rand()%999+1);  // 난수 초기화
        printf("%s\n",buff);              // 난수 화면에 출력
        Writen(sockfd, buff, strlen(buff)); // 입력한 값 전송
        
        /* 데이터 받는구간 */
        printf("recv> ");
        Fputs(recvline, stdout);
    }
}


int
main(int argc, char **argv)
{
	int					listenfd, connfd;
    pid_t               childpid;
    socklen_t           len;
	struct sockaddr_in	servaddr, cliaddr;
	char				buff[MAXLINE], recvline[MAXLINE+1];
	time_t				ticks;
    
    if(argc!=2)
        err_quit("usage: a.out<IPaddress>");
    if(atoi(argv[1])<=1024)
        err_sys("Well known port number error");
    
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));	/* daytime server */

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

    // Signal(SIGCHLD, sig_chld);
    
	for ( ; ; ) {
        len = sizeof(cliaddr);
		if((connfd = Accept(listenfd, (SA *) &cliaddr, &len))<0){
            if(errno == EINTR)
                continue;
            else
                err_sys("accept error");
        }
        printf("[%s:%d] is connected!!\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
        
        str_echo(connfd);

		Close(connfd);
        printf("[%s:%d] is disconnected!!\n\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
	}
}
