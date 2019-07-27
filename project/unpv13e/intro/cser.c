#include "unp.h"

#define MAX_CLIENT 10				// max cient
#define CHATDATA 1024				// chatdata size


int client[MAX_CLIENT];			
int clientCount;				// number of current user count

char quit[] = "/quit";				// quit option
char list[] = "/list";				// list option
char smsg[] = "/smsg";				// smsg option

char conn[] ="hi everyone?\n";			// connect message
char full[] ="Sorry No More Connection\n";	// full client message

// connection client - pushClient function
int pushClient(int c_socket)			
{
	int i;

	for(i = 0; i < MAX_CLIENT; i++)
	{
		if(client[i] == -1){
			client[i] = c_socket;
			clientCount++;
			return i;
		}
	}

	if(i == MAX_CLIENT)
		return -1;

}



// connection closed by client - popClient function 
int popClient(int c_socket)
{
	int i;
	
	Close(c_socket);

	for(i = 0; i < MAX_CLIENT; i++)
	{
		if(c_socket == client[i]){
			client[i] = -1;
			clientCount--;
			break;
		}
	}

	return 0;
}

// chat server - main function 
int
main(int argc, char *argv[])
{
	int c_socket;				// connect socket
	int l_socket;				// listen  cosket

	struct sockaddr_in c_addr;		// connect addr
	struct sockaddr_in l_addr;		// listen  addr
	struct sockaddr_in cliaddr;		// client  addr

	int c_len;				// c_addr length
	int maxfd = 0;
	
	int i;					// for.. 
	int j;					// for..
	int k;					// for..
	int n;					// read function
	
	fd_set rset;				// select function

	char clientName[MAX_CLIENT][20];	// clients name

	char chatData[CHATDATA];		// chat date
	char connName[CHATDATA];		// connection client name
	char buf[CHATDATA];			// buf..
	char temp[MAXLINE];			// temp..
    char userInfo[CHATDATA];    // user address info

	char *token;				// /smsg option use *token

	int res;				

	char* str;
	// argv input data
	if(argc != 2)
	{
		err_quit("usage:  <Port>");
	}



	// <-- listen socket
	l_socket = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&l_addr, sizeof(l_addr) );

	l_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	l_addr.sin_family = AF_INET;
	l_addr.sin_port = htons(atoi(argv[1]));

	Bind(l_socket, (SA *) &l_addr, sizeof(l_addr));

	Listen(l_socket, LISTENQ);
	// listen socket--> 




	// server address output
	
	printf("[server address is %s : %d] \n",
		 Inet_ntop(AF_INET, &l_addr.sin_addr, buf, sizeof(buf)), 
		 ntohs(l_addr.sin_port) );
	

	// client[i] -> initialize	
	for(i = 0; i < MAX_CLIENT; i++)
		client[i] = -1;


	// clientCount -> initialize
	clientCount = 0;



	for(;;)
	{
		// initialize
		maxfd = l_socket;

		FD_ZERO(&rset);
		FD_SET(l_socket, &rset);


		// -1 indicate available entry
		for(i = 0; i < MAX_CLIENT; i++)
		{
			if(client[i] != -1)
			{
				FD_SET(client[i], &rset);
				if(client[i] > maxfd)
					maxfd = client[i];
			}
		}

		maxfd++;

		Select(maxfd, &rset, NULL, NULL, NULL);


		// new client connection //

		if(FD_ISSET(l_socket, &rset))
		{
			c_len = sizeof(c_addr);
			if( (c_socket = accept(l_socket, (SA *) &c_addr, &c_len)) < 0 ){
				if(errno == EINTR)
					continue;
				else
					err_sys("accept error");
			}
			// clientame recv
			read(c_socket, connName, sizeof(connName));
			printf("%s is connected from %s:%d\n", 
                   connName, 
                   Inet_ntop(AF_INET, &c_addr.sin_addr, buf, sizeof(buf) ),
                   ntohs(c_addr.sin_port)
                  );

			// clinet connected 
			res = pushClient(c_socket);

			// connName -> clilentName[?][20] copy				
			strcpy(clientName[res], connName);
	
			if(res < 0)
			{
				write(c_socket, full, strlen(full)); // too many client, connection fail
				Close(c_socket);
			}else{
				write(c_socket, conn, strlen(conn)); // client connection success
			}
		}


		// check all clients for data //

		for(i = 0; i < MAX_CLIENT; i++)
		{
			if( (client[i] != -1) && FD_ISSET(client[i], &rset) )
			{
				memset(chatData, 0, sizeof(chatData));
				if( (n = read(client[i], chatData, sizeof(chatData))) > 0)
				{

					// chating
					for(j = 0; j < MAX_CLIENT; j++)
					{

						if( strstr(chatData,quit) != NULL)
							break;

						if( strstr(chatData,list) != NULL)
							break;	
					
						if( strstr(chatData,smsg) != NULL)
							break;

						if( (client[i] != -1) && (client[i]!=client[j]))
						{
							snprintf(buf,sizeof(buf),"[%s] %s", clientName[i],chatData);
							write(client[j], buf, strlen(buf));
						}
					}




					// /quit option
					if(strstr(chatData,quit) != NULL)
					{
						printf("%s is leaved from %s\n",clientName[i], Inet_ntop(AF_INET, &c_addr.sin_addr, buf, sizeof(buf) ) );
						popClient(client[i]);
						break;
					}

					// /list option
					if(strstr(chatData,list) != NULL)
					{
						snprintf(buf,sizeof(buf), "[the number of current user is %d]\n", clientCount);
						write(client[i], buf, strlen(buf));
                        
                        for(int index = 0; index <= res; index++){
                            int len = sizeof(cliaddr);    // all user socketfd length
                            getpeername(client[index], (SA *) &cliaddr, &len);    // get user info
							snprintf(buf,sizeof(buf),"[ %s:%d ]\n",          // transrate 
                               Inet_ntop(AF_INET, &cliaddr.sin_addr,userInfo, sizeof(userInfo)), 
                               ntohs(cliaddr.sin_port)
                              );
                            write(client[i], buf, strlen(buf));
                        }
						break;
					}
					else{ // /smsg option
						token = strtok(chatData," ");
						if(strcmp(token,smsg) == 0)
						{   
							token = strtok(NULL," ");
							for(k = 0; k < MAX_CLIENT; k++)
							{

								if(strcmp(token,clientName[k]) == 0)
								{
									snprintf(buf,sizeof(buf),"[smsg from %s] ", token);
									write(client[k], buf, strlen(buf));
									
									while(token != NULL)
									{
										token = strtok(NULL," ");
										snprintf(buf,sizeof(buf),"%s ", token);
										write(client[k], buf, strlen(buf));
									}
									memset(buf, 0, sizeof(buf));
									strcpy(token,"\n");
									snprintf(buf,sizeof(buf),"%s",token);
									write(client[k], buf, strlen(buf));

									
								}
								break;
							}

						}

					}


				}


			}


		}


	}


}
