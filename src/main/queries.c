#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/random.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "stunRU.h"

struct sockaddr_in *stun_bind_query(void)
{
	struct stun_msg msg, *res;
	socket_t sock;
	char addrbuf[INET_ADDRSTRLEN];

	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket() error");
		return NULL;
	}
	msg.type = htons(STUN_BIND_REQ);
	msg.length = 0x0000;
	msg.magic = htonl(STUN_MAGIC_COOKIE);
	if (getrandom(msg.id, sizeof(msg.id), 0) == -1) {
		perror("getrandom() error");
		return NULL;
	}
	if (!send_stun(sock, &msg, 0)) {
		return NULL;
	}
	if ((res = recv_stun(sock, 0)) == NULL) {
		return NULL;
	}
	close(sock);
	if (res -> type != STUN_BIND_RESP) {
		fprintf(stderr, "Improper STUN type\n");
		return NULL;
	}
	if (res -> magic != STUN_MAGIC_COOKIE) {
		fprintf(stderr, "Improper STUN cookie");
		return NULL;
	}
	for (int i = 0; i < 3; i++)
		if (ntohl(res -> id[i]) != msg.id[i]) {
			fprintf(stderr, "Improper STUN id\n");
			return NULL;
		}
	switch (res -> attribute.type) {
	case STUN_TYPE_MAPPED_ADDR:
		printf("Transport Address : %s:%d\n", inet_ntop(AF_INET, (struct in_addr *) &(res -> stun_addr.ipv4), addrbuf, sizeof(addrbuf)), res -> stun_bind.port);
		break;
	case STUN_TYPE_XOR_MAPPED_ADDR:
		res -> stun_bind.port ^= (STUN_MAGIC_COOKIE >> 16);
		res -> stun_addr.ipv4 ^= STUN_MAGIC_COOKIE;
		dword temp = htonl(res -> stun_addr.ipv4);
		printf("Transport Address : %s:%d\n", inet_ntop(AF_INET, (struct in_addr *) &temp, addrbuf, sizeof(addrbuf)), res -> stun_bind.port);
	}
	free(res);
	return NULL;
}
