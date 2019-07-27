#include "unp.h"
#include <stdlib.h>
int
main(int argc, char **argv)
{
    int sockfd, n;
    char recvline[MAXLINE+1], sendline[MAXLINE];
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
    
    // while((n=read(sockfd, recvline, MAXLINE))>0){
    //     recvline[n] = 0;
    //     if(fputs(recvline,stdout)==EOF)
    //         err_sys("fputs error");
    // }
    // if(n<0)
    //     err_sys("read error");
    
    /* 데이터 전송구간 */
    printf("send> ");
    if(Fgets(sendline,MAXLINE,stdin)==NULL) // Ctrl + D 입력시 종료
        printf("Ctrl + D 입력!!.\n");
    Writen(sockfd, sendline, strlen(sendline)); // 입력한 값 전송
    
    /* 데이터 받는구간 */
    if(Readline(sockfd, recvline, MAXLINE)!=0){ // 읽어온 행의 갯수가 0이 아니라면, 읽어온 값을 화면에 출력!!
        printf("recv> ");
        Fputs(recvline, stdout);
        printf("\n");
    }
    exit(0);
}