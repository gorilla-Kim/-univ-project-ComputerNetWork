#include "unp.h"

void
sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
	pid=wait(&stat);
	printf("child %d terminated\n",pid);
	return;
}

int
main(int argc, char** argv)
{
	int listenfd, connfd, udpfd, nready, maxfdp1;
//     추가한 내용
    int sockfd, i, maxi, client[FD_SETSIZE]; 
    
	char mesg[MAXLINE];
	pid_t childpid;
	fd_set rset;
//     추가한 내용
    fd_set allset;
    char buf[MAXLINE];
    
	ssize_t	n;
	socklen_t len;
	const int on = 1;
	struct sockaddr_in cliaddr, servaddr;
	void sig_chld(int);
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0 );

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	
	Setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	Bind(listenfd, (SA*)&servaddr, sizeof(servaddr));
	Listen(listenfd, LISTENQ);
    
	udpfd = Socket(AF_INET, SOCK_DGRAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(udpfd, (SA*)&servaddr, sizeof(servaddr));
	Signal(SIGCHLD, sig_chld);

//     새로 추가한 내용
    maxi = -1;  /* index into client[] array */ 
    for (i = 0; i < FD_SETSIZE; i++) 
        client[i] = -1; /* -1 indicates available entry */ 
    FD_ZERO (&allset); 
    
	// FD_ZERO(&rset);
	maxfdp1 = max(listenfd, udpfd) + 1;

	for(;;){
//         새로 추가한 내용
        printf("%d\n",maxfdp1);
        rset = allset;  /* structure assignment */
        
		FD_SET(listenfd, &rset);
		FD_SET(udpfd, &rset);
		if((nready=select(maxfdp1+1, &rset, NULL, NULL, NULL))<0){
			if(errno == EINTR)
				continue;
			else
				err_sys("select error");
		}
		if(FD_ISSET(listenfd, &rset)){
			len = sizeof(cliaddr);
			connfd = Accept(listenfd,(SA*)&cliaddr, &len);
            
//             새로추가한 내용
            for ( i = 0; i < FD_SETSIZE; i++) { 
                if (client[i] < 0) { 
                    client[i] = connfd;/*save descriptor*/ 
                    // printf("client의 %d번째에 새롭게 추가한 항목 %d\n",i,client[i]);
                    break; 
                }
            }
            if (i == FD_SETSIZE) 
                err_quit ("too many clients"); 
            FD_SET(connfd, &allset);/*add new descriptor to set*/ 
            if ( connfd > maxfdp1) 
                maxfdp1 = connfd; /* for select */ 
            if (i > maxi) 
                maxi = i;/* max index in client[] array */ 
            if (--nready <= 0){
                // printf("다시한번더 입력합니다. %d \n",nready);
                continue;/* no more readable descriptor */ 
            }
		}
//      여기까지
        // printf("maxi 는 %d입니다.\n",maxi);
//      새로 추가한 내용
        for (i = 0; i <= maxi; i++) { /*check all clients for data */ 
            if ( (sockfd = client[i] ) < 0) 
                continue; 
            // printf("client의 %d번째 항목 선택!! %d\n",i,sockfd);
            if (FD_ISSET (sockfd, &rset) ) { 
                if ( (n = Read (sockfd, buf, MAXLINE)) == 0) { /* connection closed by client */ 
                    // buf[n]=0;
                    Close (sockfd);
                    FD_CLR (sockfd, &allset); 
                    client[i] = -1; 
                    // printf("Read succes!!\n해당항목 초기화 %d\n",client[i]);
                } else 
                    Writen (sockfd, buf, n); 
                if (--nready <= 0) 
                    break; /*no more descriptors */ 
            }
        }
//             여기까지
            
		if(FD_ISSET(udpfd,&rset)){
            printf("udp에 새롭게 추가한 항목 %d\n",udpfd);
			len= sizeof(cliaddr);
			n = Recvfrom(udpfd, mesg, MAXLINE, 0, (SA*)&cliaddr, &len);

			Sendto(udpfd, mesg, n, 0, (SA*) &cliaddr, len);
		}
	}
}
