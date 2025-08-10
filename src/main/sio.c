/*
 * Socket I/O.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "stunRU.h"

const char *stunservers[] = {
	"stun.l.google.com",
	"stun1.l.google.com",
	"stun2.l.google.com",
	"stun3.l.google.com",
	"stun4.l.google.com"
};

const char *stunports[] = {
	"19302",
	"3478",
	"19302",
	"3478",
	"19302"
};

bool send_stun(socket_t sock, struct stun_msg *msg, byte ind)
{
	struct addrinfo sai, *spai;
	int errcode;

	if (!msg || sock <= 0 || ind >= sizeof(stunservers)) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("send_stun()")
		return false;
	}
	memset(&sai, 0, sizeof(sai));
	sai.ai_family = AF_INET;
	sai.ai_protocol = SOCK_DGRAM;
	if ((errcode = getaddrinfo(stunservers[ind], stunports[ind], &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	if (sendto(sock, msg, 120, 0, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("sendto() error");
		return false;
	}
	freeaddrinfo(spai);
	return true;
}

struct stun_msg *recv_stun(socket_t sock, byte ind)
{
	struct stun_msg *msg;

	if (sock <= 0 || ind >= sizeof(stunservers)) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("recv_stun()")
		return NULL;
	}
	if ((msg = malloc(sizeof(struct stun_msg))) == NULL) {
		perror("malloc() error");
		return NULL;
	}
	if (recvfrom(sock, msg, 120, 0, NULL, NULL) == -1) {
		perror("recvfrom() error");
		return NULL;
	}
	return msg;
}
