#pragma once
int readn(int fd, char* buf, int count)
{
	int nleft = count;
	int nread = 0;
	char* pbuf = buf;
	while (nleft > 0)
	{
		nread = read(fd, pbuf, nleft);
		if (nread == -1)
		{
			perror("read");
		}
		else if (nread == 0)
		{
			return count - nleft;
		}
		pbuf += nread; //÷∏’Î∫Û“∆
		nleft -= nread;
	}
	return count;
}

void writen(int fd, const char* msg,int len)
{
	char* buf = new char[len+sizeof(int)];
	int netlen = htonl(len);
	memcpy(buf, (char*)&netlen, sizeof(int));
	memcpy(buf + sizeof(int), msg, len);
	write(fd, buf, len + sizeof(int));
}
