#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/random.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "stunD.h"

struct errep *stun_bind_query(int af, socket_t sock, struct stun_server *serv, struct sockaddr_storage *result)
{
        struct errep *err;
        char *fnname = "stun_bind_query()";
	struct sockaddr_in *skai;
	struct sockaddr_in6 *skai6;
	struct stun_msg *msg, *res;
	bool sockset;

	sockset = false;
	if (sock <= 0) {
		sockset = true;
		if (af == AF_INET) {
			if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
                                FILL_ERREP(err, fnname, "socket could not be created");
                                return err;
			}
		} else if (af == AF_INET6) {
			if ((sock = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
                                FILL_ERREP(err, fnname, "socket could not be created");
                                return err;
			}
		} else {
                        FILL_ERREP(err, fnname, "function was passed a nonstandard address family");
                        return err;
		}
	}
	if ((msg = malloc(sizeof(struct stun_msg))) == NULL) {
                FILL_ERREP(err, fnname, "stun_msg could not be allocated memory");
                return err;
	}
	memset(msg, 0, sizeof(struct stun_msg));
	msg -> type = htons(STUN_BIND_REQ);
	msg -> length = 0x0000;
	msg -> magic = htonl(STUN_MAGIC_COOKIE);
	if (getrandom(msg -> id, 3, 0) == -1) {
                FILL_ERREP(err, fnname, "error generating random number with getrandom()");
                return err;
	}
	if ((err = send_stun(af, sock, msg, serv)) -> msg != NULL) {
		FILL_ERREP(err -> next, fnname, "failure send stun message");
                return err;
	}
	if ((err = recv_stun(sock, res)) -> msg -> != NULL) {
		return NULL;
	}
	if (sockset)
		close(sock);
	if (res -> type != STUN_BIND_RESP) {
		fprintf(stderr, "Improper STUN type\n");
		return NULL;
	}
	if (res -> magic != STUN_MAGIC_COOKIE) {
		fprintf(stderr, "Improper STUN cookie");
		return NULL;
	}
	for (int i = 0; i < STUN_MSG_ID_LEN; i++)
		if (ntohl(res -> id[i]) != msg -> id[i]) {
			fprintf(stderr, "Improper STUN id\n");
			return NULL;
		}
	switch (res -> attribute.type) {
	case STUN_TYPE_MAPPED_ADDR:
	case STUN_TYPE_XOR_MAPPED_ADDR:
		if (res -> stun_bind.family == STUN_FAMILY_IPV4) {
			if (res -> attribute.type == STUN_TYPE_XOR_MAPPED_ADDR) {
				res -> stun_bind.port ^= (STUN_MAGIC_COOKIE >> 16);
				res -> stun_addr.ipv4 ^= STUN_MAGIC_COOKIE;
			}
			if ((skai = malloc(sizeof(struct sockaddr_in))) == NULL) {
				perror("malloc() error");
				return NULL;
			}
			skai -> sin_family = AF_INET;
			memcpy(&skai -> sin_port, &res -> stun_bind.port, sizeof(word));
			memcpy(&skai -> sin_addr, &res -> stun_addr.ipv4, sizeof(dword));
			free(msg);
			free(res);
			return (struct sockaddr_storage *) skai;
		}
		if (res -> stun_bind.family == STUN_FAMILY_IPV6) {
			if (res -> attribute.type == STUN_TYPE_XOR_MAPPED_ADDR) {
				res -> stun_bind.port ^= (STUN_MAGIC_COOKIE >> 16);
				res -> stun_addr.ipv6[0] ^= STUN_MAGIC_COOKIE;
				for (int i = 1; i <= 3; i++)
					res -> stun_addr.ipv6[i] ^= msg -> id[i];
			}
			if ((skai6 = malloc(sizeof(struct sockaddr_in6))) == NULL) {
				perror("malloc() error");
				return NULL;
			}
			skai6 -> sin6_family = AF_INET6;
			memcpy(&skai6 -> sin6_port, &res -> stun_bind.port, sizeof(word));
			memcpy(&skai6 -> sin6_addr, &res -> stun_addr.ipv6, sizeof(dword) * 4);
			free(msg);
			free(res);
			return (struct sockaddr_storage *) skai6;
		}
		break;
	}
	return NULL;
}
