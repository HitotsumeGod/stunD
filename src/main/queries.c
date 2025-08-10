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
	struct sockaddr_in skai;
	struct stun_msg msg, *kickback;
	socket_t sock;

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
	if ((kickback = recv_stun(sock, 0)) == NULL) {
		return NULL;
	}
	close(sock);
	if (kickback -> type != STUN_BIND_RESP) {
		fprintf(stderr, "Improper STUN type\n");
		return NULL;
	}
	if (kickback -> magic != STUN_MAGIC_COOKIE) {
		fprintf(stderr, "Improper STUN cookie");
		return NULL;
	}
	for (int i = 0; i < 3; i++)
		if (ntohl(kickback -> id[i]) != msg.id[i]) {
			fprintf(stderr, "Improper STUN id\n");
			return NULL;
		}
	printf("Type : 0x%04X\n", kickback -> type);
	printf("Length : 0x%04X\n", kickback -> length);
	printf("Length : 0x%04X\n", kickback -> magic);
	printf("ID : ");
	for (int i = 0; i < 3; i++)
		printf("0x%04X ", kickback -> id[i]);
	printf("\n");
	printf("Attrib Type : 0x%04X\n", kickback -> attribute.type);
	printf("Attrib Length : 0x%04X\n", kickback -> attribute.length);
	printf("MADDR Family : 0x%04X\n", kickback -> stun_bind.family);
	printf("MADDR Port : 0x%04X\n", kickback -> stun_bind.port);
	return NULL;
}
