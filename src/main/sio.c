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
		PRINT_CERR("send_stun()");
		return false;
	}
	memset(&sai, 0, sizeof(sai));
	sai.ai_family = AF_INET;
	sai.ai_socktype = SOCK_DGRAM;
	if ((errcode = getaddrinfo(stunservers[ind], stunports[ind], &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	if (sendto(sock, msg, STUN_HDRLEN + msg -> length, 0, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("sendto() error");
		return false;
	}
	freeaddrinfo(spai);
	return true;
}

struct stun_msg *recv_stun(socket_t sock, byte ind)
{
	struct stun_msg *msg;
	byte buf[120];
	int place;

	if (sock <= 0 || ind >= sizeof(stunservers)) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("recv_stun()");
		return NULL;
	}
	if ((msg = malloc(sizeof(struct stun_msg))) == NULL) {
		perror("malloc() error");
		return NULL;
	}
	memset(msg, 0, sizeof(struct stun_msg));
	if (recvfrom(sock, buf, 120, 0, NULL, NULL) == -1) {
		perror("recvfrom() error");
		return NULL;
	}
	place = 0;
	msg -> type = (buf[place++] << 8) | buf[place++];
	msg -> length = (buf[place++] << 8) | buf[place++];
	for (int i = 0; i < sizeof(dword); i++)
		msg -> magic = (msg -> magic << 8) | buf[place++];
	for (int i = 0; i < sizeof(msg -> id); i++)
		for (int ii = 0; ii < sizeof(dword); ii++)
			msg -> id[i] = (msg -> id[i] << 8) | buf[place++];
	msg -> attribute.type = (buf[place++] << 8) | buf[place++];
	msg -> attribute.length = (buf[place++] << 8) | buf[place++];
	switch (msg -> attribute.type) {
	case STUN_TYPE_MAPPED_ADDR:
	case STUN_TYPE_XOR_MAPPED_ADDR:
		msg -> stun_bind.family = buf[place++];
		msg -> stun_bind.port = (buf[place++] << 8) | buf[place++];
		if (msg -> stun_bind.family == AF_INET)
			for (int i = 0; i < sizeof(dword); i++)
				msg -> stun_addr.ipv4 = (msg -> stun_addr.ipv4 << 8) | buf[i];
		break;
	}
	return msg;
}
