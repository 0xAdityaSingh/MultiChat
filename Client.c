#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <errno.h>
#include <sys/un.h>

#define BUFSIZE 1024
#define NAME "socket"
int main()
{

	int sockfd, fdmax, i, len;
	struct sockaddr_un server_addr;
	fd_set master;
	fd_set read_fds;

	{
		if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		{
			perror("Socket");
			exit(1);
		}
		server_addr.sun_family = AF_UNIX;
		strcpy(server_addr.sun_path, NAME);
		len = strlen(server_addr.sun_path) + sizeof(server_addr.sun_family);

		if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_un)) == -1)
		{
			perror("connect");
			exit(1);
		}
	}
	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(0, &master);
	FD_SET(sockfd, &master);
	fdmax = sockfd;
	printf("Port number- %d\n", getpid());

	while (1)
	{
		read_fds = master;
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}

		for (i = 0; i <= fdmax; i++)
			if (FD_ISSET(i, &read_fds))

			{

				char recv_buf[BUFSIZE];
				int nbyte_recvd;
				char send_buf[BUFSIZE];

				if (i == 0)
				{
					fgets(send_buf, BUFSIZE, stdin);

					if (strcmp(send_buf, "quit\n") == 0)
					{
						exit(0);
					}

					else

						send(sockfd, send_buf, strlen(send_buf), 0);
				}
				else
				{
					nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
					recv_buf[nbyte_recvd] = '\0';
					int x;
					sscanf(recv_buf, "%d", &x);
					if (x == 0)
					{
						printf("%s\n", recv_buf);
						fflush(stdout);
					}
					else
					{
						if (x == getpid())
						{
							char *starting = recv_buf;
							while (*starting != ' ')
							{
								starting++;
							}
							starting++;
							printf("%s\n", starting);
							fflush(stdout);
						}
						x = 0;
					}
				}
			}
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
