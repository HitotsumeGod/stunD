#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include "stunD.h"

bool poll_stun_servers(int af, int amou)
{
	struct sockaddr_storage **cargo, **dummy;
	struct sockaddr_in *skai;
	struct addrinfo sai, *spai;
	socket_t sock;
	size_t cargolen;
	char addrbuf[INET_ADDRSTRLEN];
	int dead = 0, count = 0, errcode;

	if (amou < 0) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers");
		return false;
	} else if (amou == 0 || amou > num_sservers)
		amou = num_sservers;
	memset(&sai, 0, sizeof(sai));
	if ((cargo = malloc(sizeof(struct sockaddr_storage *) * (cargolen = 2))) == NULL) {
		perror("malloc() error");
		return false;
	}
	sai.ai_family = af;
	sai.ai_socktype = SOCK_DGRAM;
	sai.ai_protocol = IPPROTO_UDP;
	sai.ai_flags = AI_PASSIVE;
	if ((errcode = getaddrinfo(NULL, STUN_CLIENT_BINDPORT, &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	if ((sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return false;
	}
	if (bind(sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return false;
	}
	freeaddrinfo(spai);
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
	if (dead > 1)
		printf("%d dead servers...\n", dead);
	else if (dead > 0)
		printf("1 dead server...\n");
	close(sock);
	free(cargo);
	return true;
}

bool poll_stun_servers_by_name(int af, struct stun_server **servs)
{
	struct sockaddr_storage **cargo, **dummy;
	struct sockaddr_in *skai;
	struct addrinfo sai, *spai;
	socket_t sock;
	size_t cargolen;
	char addrbuf[INET_ADDRSTRLEN];
	int dead = 0, count = 0, errcode;

	if (!servs) {
		errno = BAD_ARGS_ERR;
		PRINT_CERR("poll_stun_servers_by_name()");
		return false;
	}
	memset(&sai, 0, sizeof(sai));
	if ((cargo = malloc(sizeof(struct sockaddr_storage *) * (cargolen = 2))) == NULL) {
		perror("malloc() error");
		return false;
	}
	sai.ai_family = af;
	sai.ai_socktype = SOCK_DGRAM;
	sai.ai_protocol = IPPROTO_UDP;
	sai.ai_flags = AI_PASSIVE;
	if ((errcode = getaddrinfo(NULL, STUN_CLIENT_BINDPORT, &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	if ((sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return false;
	}
	if (bind(sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return false;
	}
	freeaddrinfo(spai);
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
	for (int i = 0; i < count; i++)
		if (af == AF_INET) {
			if ((skai = (struct sockaddr_in *) cargo[i]) == NULL)
				continue;
			skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
			printf("%s ---> Transport Address : %s:%d\n", servs[i] -> name, inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
			free(skai);
		}
	if (dead > 1)
		printf("%d dead servers...\n", dead);
	else if (dead > 0)
		printf("1 dead server...\n");
	close(sock);
	free(cargo);
	return true;
}

bool poll_recv_test(char *host)
{
	struct sockaddr_in *skai;
	struct addrinfo sai, *spai;
	socket_t sock;
	char *pmsg, addrbuf[INET_ADDRSTRLEN], recvbuf[5000];
	int errcode;
	size_t siz;
	FILE *dump;
	
	pmsg = "I'm a holepuncher!";
	memset(&sai, 0, sizeof(sai));
	sai.ai_family = AF_INET;
	sai.ai_socktype = SOCK_DGRAM;
	sai.ai_protocol = IPPROTO_UDP;
	sai.ai_flags = AI_PASSIVE;
	if ((errcode = getaddrinfo(NULL, STUN_CLIENT_BINDPORT, &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	if ((sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return false;
	}
	int val = 1;
	if (setsockopt(sock, SOL_SOCKET, IP_MTU_DISCOVER, &val, sizeof(val)) == -1) {
		perror("setsockopt() error");
		return false;
	}
	if (bind(sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return false;
	}
	freeaddrinfo(spai);
	if ((skai = (struct sockaddr_in *) stun_bind_query(AF_INET, sock, sservers)) == NULL)
		return false;
	skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
	printf("%s --> Transport Address : %s:%d\n", sservers[0].name, inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
	if ((errcode = getaddrinfo(host, "7447", &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return false;
	}
	for (int i = 0; i < 100; i++)
		if (sendto(sock, pmsg, strlen(pmsg), 0, spai -> ai_addr, spai -> ai_addrlen) == -1) {
			perror("sendto() error");
			return false;
		}
	freeaddrinfo(spai);
	if ((siz = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL)) == -1) {
		perror("recvfrom() error");
		return false;
	}
	if ((dump = fopen("packet.cap", "wb")) == NULL) {
		perror("fopen() error");
		return false;
	}
	fwrite(recvbuf, siz, 1, dump);
	fclose(dump);
	close(sock);
	return true;
}
