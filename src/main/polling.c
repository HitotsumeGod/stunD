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

int poll_stun_roundtrip(int af)
{
	struct sockaddr_in *sk1, *sk2, *trip;
	struct sockaddr_in6 *sks1, *sks2, *trips;
	struct addrinfo sai, *spai;
	socket_t serv_sock, cli_sock;
	char *yo = "yolo!!";
	int errcode;
	byte dummy;
	
	memset(&sai, 0, sizeof(sai));
	sai.ai_family = af;
	sai.ai_socktype = SOCK_DGRAM;
	sai.ai_protocol = IPPROTO_UDP;
	sai.ai_flags = AI_PASSIVE;
	if ((errcode = getaddrinfo(NULL, STUN_CLIENT_BINDPORT, &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return EXIT_FAILURE;
	}
	if ((serv_sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return EXIT_FAILURE;
	}
	if (bind(serv_sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return EXIT_FAILURE;
	}
	freeaddrinfo(spai);
	if (af == AF_INET) {
		if ((sk1 = (struct sockaddr_in *) stun_bind_query(af, serv_sock, sservers)) == NULL)
			return EXIT_FAILURE;
		if ((sk2 = (struct sockaddr_in *) stun_bind_query(af, serv_sock, sservers + 1)) == NULL)
			return EXIT_FAILURE;
		if (sk1 -> sin_port != sk2 -> sin_port)
			return HARD_NAT;
		sk1 -> sin_addr.s_addr = htonl(sk1 -> sin_addr.s_addr);
	} else if (af == AF_INET6) {
		if ((sks1 = (struct sockaddr_in6 *) stun_bind_query(af, serv_sock, sservers)) == NULL)
			return EXIT_FAILURE;
		if ((sks2 = (struct sockaddr_in6 *) stun_bind_query(af, serv_sock, sservers + 1)) == NULL)
			return EXIT_FAILURE;
		if (sks1 -> sin6_port != sks2 -> sin6_port)
			return HARD_NAT;
	}
	if (fcntl(serv_sock, F_SETFL, O_NONBLOCK) == -1) {
		perror("fcntl() error");
		return EXIT_FAILURE;;
	}
	if ((errcode = getaddrinfo(NULL, TRIP_CLIENT_BINDPORT, &sai, &spai)) != 0) {
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(errcode));
		return EXIT_FAILURE;
	}
	if ((cli_sock = socket(spai -> ai_family, spai -> ai_socktype, spai -> ai_protocol)) == -1) {
		perror("socket() error");
		return EXIT_FAILURE;
	}
	if (bind(cli_sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return EXIT_FAILURE;
	}
	freeaddrinfo(spai);
	if (af == AF_INET) {
		if (sendto(cli_sock, yo, strlen(yo), 0, (struct sockaddr *) sk1, sizeof(struct sockaddr)) == -1) {
			perror("sendto() error");
			return EXIT_FAILURE;
		}
	} else if (af == AF_INET6) {
		if (sendto(cli_sock, yo, strlen(yo), 0, (struct sockaddr *) sks1, sizeof(struct sockaddr)) == -1) {
			perror("sendto() error");
			return EXIT_FAILURE;
		}
	}
	/* if initial test succeeds, return confirming no firewall
	 * otherwise, holepunch the firewall using the serving socket and try again
	 */
	if (recvfrom(serv_sock, &dummy, sizeof(byte), 0, NULL, NULL) == -1) {
		if (errno == EAGAIN || errno == EWOULDBLOCK) {
			if (af == AF_INET) {
				//get the client socket's transport address
				if ((trip = (struct sockaddr_in *) stun_bind_query(af, cli_sock, sservers)) == NULL)
					return EXIT_FAILURE;
				trip -> sin_addr.s_addr = htonl(trip -> sin_addr.s_addr);
			} else if (af == AF_INET6) {
				//get the client socket's transport address
				if ((trips = (struct sockaddr_in6 *) stun_bind_query(af, cli_sock, sservers)) == NULL)
					return EXIT_FAILURE;
			}
			for (int i = 0; i < 10; i++)
				//probe the client through the server, which SHOULD, in theory, allow reciprocal communication
				if (sendto(serv_sock, yo, strlen(yo), 0, (struct sockaddr *) trip, sizeof(struct sockaddr)) == -1) {
					perror("sendto() error");
					return EXIT_FAILURE;
				}
			//we've tried our best to breach the firewall with no success, so return unsuccessful
			if (recvfrom(serv_sock, &dummy, sizeof(byte), 0, NULL, NULL) == -1)
				return HARD_FW;
			return NORM_FW;
		} else {
			perror("recvfrom() error");
			return EXIT_FAILURE;
		}
	}
	close(serv_sock);
	close(cli_sock);
	return NO_FW;
}

bool poll_recv_test(void)
{
	struct sockaddr_in *skai;
	struct addrinfo sai, *spai;
	socket_t sock;
	char addrbuf[INET_ADDRSTRLEN], recvbuf[5000];
	int errcode;
	size_t siz;
	FILE *dump;
	
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
	if (bind(sock, spai -> ai_addr, spai -> ai_addrlen) == -1) {
		perror("bind() error");
		return false;
	}
	freeaddrinfo(spai);
	if ((skai = (struct sockaddr_in *) stun_bind_query(AF_INET, sock, sservers)) == NULL)
		return false;
	skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
	printf("%s --> Transport Address : %s:%d\n", sservers[0].name, inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
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
