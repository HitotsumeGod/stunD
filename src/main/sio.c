/*
 * Socket I/O.
 */

#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "stunD.h"

struct stun_server sservers[] = {
	{ "stun.l.google.com",  "19302" },
	{ "stun1.l.google.com", "3478"  },
	{ "stun2.l.google.com", "19302" },
	{ "stun3.l.google.com", "3478"  },
	{ "stun4.l.google.com", "19302" }
};

const int num_sservers = 5;

bool send_stun(int af, socket_t sock, struct stun_msg *msg, struct stun_server *serv)
{
	struct addrinfo sai, *spai;
	int errcode;

	if (!msg || !serv || sock <= 0) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("send_stun()");
		return false;
	}
	memset(&sai, 0, sizeof(sai));
	if (af == AF_INET)
		sai.ai_family = AF_INET;
	else if (af == AF_INET6)
		sai.ai_family = AF_INET6;
	else {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("send_stun()");
		return false;
	}
	sai.ai_socktype = SOCK_DGRAM;
	if ((errcode = getaddrinfo(serv -> name, serv -> port, &sai, &spai)) != 0) {
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

struct stun_msg *recv_stun(socket_t sock)
{
	struct stun_msg *msg;
	byte buf[120];
	int place;

	if (sock <= 0) {
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
	for (int i = 0; i < STUN_MSG_ID_LEN; i++)
		for (int ii = 0; ii < sizeof(dword); ii++)
			msg -> id[i] = (msg -> id[i] << 8) | buf[place++];
	msg -> attribute.type = (buf[place++] << 8) | buf[place++];
	msg -> attribute.length = (buf[place++] << 8) | buf[place++];
	switch (msg -> attribute.type) {
	case STUN_TYPE_MAPPED_ADDR:
	case STUN_TYPE_XOR_MAPPED_ADDR:
		msg -> stun_bind.reserved = buf[place++];
		msg -> stun_bind.family = buf[place++];
		msg -> stun_bind.port = (buf[place++] << 8) | buf[place++];
		if (msg -> stun_bind.family == STUN_FAMILY_IPV4)
			for (int i = 0; i < sizeof(dword); i++)
				msg -> stun_addr.ipv4 = (msg -> stun_addr.ipv4 << 8) | buf[place++];
		else if (msg -> stun_bind.family == STUN_FAMILY_IPV6)
			for (int i = 0; i < STUN_ADDR_IPV6_LEN; i++)
				for (int ii = 0; ii < sizeof(dword); ii++)
					msg -> stun_addr.ipv6[i] = (msg -> stun_addr.ipv6[i] << 8) | buf[place++];
		break;
	}
	return msg;
}
