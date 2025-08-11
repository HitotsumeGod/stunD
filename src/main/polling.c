#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "stunD.h"

bool poll_stun_servers(int af, int amou)
{
	struct sockaddr_storage **cargo, **dummy;
	struct sockaddr_in *skai;
	struct sockaddr_in6 *skai6;
	socket_t sock;
	size_t cargolen;
	char addrbuf[INET_ADDRSTRLEN], sixbuf[INET6_ADDRSTRLEN];
	int dead = 0, count = 0;

	if (amou < 0) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers");
		return false;
	} else if (amou == 0 || amou > num_sservers)
		amou = num_sservers;
	if ((cargo = malloc(sizeof(struct sockaddr_storage *) * (cargolen = 2))) == NULL) {
		perror("malloc() error");
		return false;
	}
	if (af == AF_INET) {
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			perror("socket() error");
			return false;
		}
	} else if (af == AF_INET6) {
		if ((sock = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
			perror("socket() error");
			return false;
		}
	} else {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers()");
		return false;
	}
	for (; count < amou; count++) {
		if (count == cargolen) {
			if ((dummy = realloc(cargo, sizeof(struct sockaddr_storage *) * (cargolen *= 2))) == NULL) {
				perror("realloc() error");
				return false;
			}
			cargo = dummy;
		}
		if ((cargo[count] = stun_bind_query(af, sock, sservers + count)) == NULL)
			++dead;
	}
	for (int i = 0; i < count; i++)
		if (af == AF_INET) {
			if ((skai = (struct sockaddr_in *) cargo[i]) == NULL)
				continue;
			skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
			printf("%s --> Transport Address : %s:%d\n", sservers[i].name, inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
			free(skai);
		}
	if (dead > 0)
		printf("%d dead servers...\n", dead);
	free(cargo);
	return true;
}

bool poll_stun_servers_by_name(int af, struct stun_server **servs)
{
	struct sockaddr_storage **cargo, **dummy;
	struct sockaddr_in *skai;
	struct sockaddr_in6 *skai6;
	socket_t sock;
	size_t cargolen;
	char addrbuf[INET_ADDRSTRLEN], sixbuf[INET6_ADDRSTRLEN];
	int dead = 0, count = 0;

	if (!servs) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers_by_name()");
		return false;
	}
	if ((cargo = malloc(sizeof(struct sockaddr_storage *) * (cargolen = 2))) == NULL) {
		perror("malloc() error");
		return false;
	}
	if (af == AF_INET) {
		if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			perror("socket() error");
			return false;
		}
	} else if (af == AF_INET6) {
		if ((sock = socket(AF_INET6, SOCK_DGRAM, 0)) == -1) {
			perror("socket() error");
			return false;
		}
	} else {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers()");
		return false;
	}
	while (servs[count]) {
		if (count == cargolen) {
			if ((dummy = realloc(cargo, sizeof(struct sockaddr_storage *) * (cargolen *= 2))) == NULL) {
				perror("realloc() error");
				return false;
			}
			cargo = dummy;
		}
		if ((cargo[count] = stun_bind_query(af, sock, servs[count])) == NULL)
			++dead;
		++count;
	}
	count++;
	fprintf(stderr, "%d dead servers...\n", dead);
	for (int i = 0; i < count; i++)
		if (af == AF_INET) {
			if ((skai = (struct sockaddr_in *) cargo[i]) == NULL)
				continue;
			skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
			printf("Transport Address : %s:%d\n", inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
			free(skai);
		}
	free(cargo);
	return true;
}
