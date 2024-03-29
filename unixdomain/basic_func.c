#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 4096

static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	size_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if((nread = read(fd, ptr, nleft)) < 0) {
			if(errno = EINTR)
				nread = 0;  //重新调用read函数
			else
				return -1; //出错
		} else if (nread == 0)
			break;    //遇到EOF,读取的字节不足n

		nleft -= nread;
		ptr += nread;
	}

	return (n-nleft);  
}


ssize_t writen(int fd, const void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	while (nleft > 0) {
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		
		nleft -= nwritten;
		ptr += nwritten;
	}

	return n;
}

static ssize_t my_read(int fd, char *ptr)
{
	if (read_cnt <= 0) {
		again:
			if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
				if(errno == EINTR)
					goto again;
				return -1;

			} else if (read_cnt == 0)
				return 0;
			read_ptr = read_buf;
	}

	read_cnt--;
	*ptr = *read_ptr++;
	return 1;
}

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ((rc = my_read(fd, &c)) == 1) {
			*ptr++ = c;
			if(c == '\n')
				break;  //遇到换行符，直接跳出循环
			} else if (rc == 0) {
				*ptr = 0;
				return (n - 1); //遇到EOF
			} else
				return -1;
	}	

	*ptr = 0;
	return n;

}
