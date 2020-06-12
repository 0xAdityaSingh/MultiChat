#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>

#define BUFSIZE 1024
#define NAME "socket"
int main()

{
	while (1)
	{
		int counter = 0;
		fd_set master;
		fd_set read_fds;
		int fdmax, i,len;
		int sockfd = 0;
		struct sockaddr_un my_addr, client_addr;

		FD_ZERO(&master);
		FD_ZERO(&read_fds);
		int yes = 1;

		if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		{
			perror("Socket");
			exit(1);
		}
		my_addr.sun_family = AF_UNIX; 
		strcpy(my_addr.sun_path, NAME);
		unlink(my_addr.sun_path);
		len=strlen(my_addr.sun_path) + sizeof(my_addr.sun_family);

		if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
		{
			perror("Unable to bind");
			exit(1);
		}
		if (listen(sockfd, 10) == -1)
		{
			perror("listen");
			exit(1);
		}
		fflush(stdout);
		FD_SET(sockfd, &master);

		fdmax = sockfd;
		while (1)
		{
			read_fds = master;
			int yes = 1;
			if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
			{
				perror("select");
				exit(4);
			}
			i = 0;
			while (i <= fdmax)
			{
				if (FD_ISSET(i, &read_fds))
				{
					if (i == sockfd)
					{
						socklen_t addrlen;
						int newsockfd, j;

						addrlen = sizeof(struct sockaddr_in);

						if ((newsockfd = accept(sockfd, (struct sockaddr *)&client_addr, &addrlen)) == -1)
						{
							perror("accept");
							exit(1);
						}
						else
						{
							FD_SET(newsockfd, &master);
							if (newsockfd > fdmax)
							{
								fdmax = newsockfd;
							}
						}
					}
					else
					{
						int nbytes_recvd, j;
						char recv_buf[BUFSIZE], buf[BUFSIZE];

						if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0)
						{
							if (nbytes_recvd == 0)
							{
								printf("socket %d hung up\n", i);
							}
							else
							{
								perror("recv");
							}
							FD_CLR(i, &master);
						}
						else
						{
							for (j = 0; j <= fdmax; j++)
							{
								for (int r = 0; r < counter; r++)
								{
		
								}
								if (FD_ISSET(j, &master))
								{
									if (j != sockfd && j != i)
									{
										if (send(j, recv_buf, nbytes_recvd, 0) == -1)
										{
											perror("send");
										}
									}
								}
							}
						}
					}
				}
				i++;
			}
		}
		return 0;
	}
}
