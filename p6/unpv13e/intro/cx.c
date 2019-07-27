#include "unp.h"
#include <stdlib.h>

void
str_cli(FILE *fp, int sockfd)
{
    ssize_t n;
    char buff[MAXLINE], recvline[MAXLINE];
    
    again:
    while((n=read(sockfd, recvline ,MAXLINE))>0){
        /* 데이터 받는구간 */
        printf("recv> ");
        Fputs(recvline, stdout);
    }
    
    if(n<0 && errno==EINTR)
        goto again;
    else if( n>0 )
        err_sys("asdasd");
    
     /* 데이터 전송구간 */
    printf("send> ");
    srand(time(NULL));                // 난수 발생
    sprintf(buff,"%d",rand()%999+1);  // 난수 초기화
    printf("%s\n",buff);              // 난수 화면에 출력
    Writen(sockfd, buff, strlen(buff)); // 입력한 값 전송

}

int
main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1];
    struct sockaddr_in servaddr;
    
    if(argc != 3)
        err_quit("usage : a.out <IPAddress>");
    
    if(atoi(argv[2])<=1024)
        err_sys("well known port number error");
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))<0)
        err_sys("socket error");
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    
    if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr)<=0)
        err_quit("inet_pton error for %s",argv[1]);
    
    if(connect(sockfd,(SA*)&servaddr, sizeof(servaddr))<0)
        err_sys("connect error");
    
    str_cli(stdin, sockfd);
    
    exit(0);
}