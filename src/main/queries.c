#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/random.h>
#include <netdb.h>
#include "stunRU.h"

struct sockaddr_in *stun_bind_query(void)
{
	struct sockaddr_in skai;
	struct stun_msg msg;
	struct addrinfo sai, *spai;
	socket_t sock;
	int errcode;

	memset(&sai, 0, sizeof(sai));
	sai.ai_family = AF_INET;
	sai.ai_socktype = SOCK_DGRAM;
	if ((errcode = getaddrinfo(NULL, "71177", &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s", gai_strerror(errcode));
		return NULL;
	}
	if ((sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return NULL;
	}
	if (bind(sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return NULL;
	}
	freeaddrinfo(spai);
	msg.type = STUN_BIND_REQ;
	msg.length = 0x0000;
	msg.magic = STUN_MAGIC_COOKIE;
	if (getrandom(msg.id, sizeof(msg.id), 0) == -1) {
		perror("getrandom() error");
		return NULL;
	}
	msg.attribute = NULL;
	if (!send_stun(sock, &msg, 0)) {
		return NULL;
	}
	return NULL;
}
