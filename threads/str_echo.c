#include "all_package.h"


void str_echo(int sockfd)
{
	ssize_t n;
	char buf[MAXLINE];

again:
	while((n = read(sockfd, buf, MAXLINE)) > 0)
		write(sockfd, buf, n);

	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0)
		perror("str_echo read error");
}
