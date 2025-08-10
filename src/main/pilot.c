#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "stunRU.h"

int main(void) {
	struct sockaddr_in *skai;
	char addrbuf[INET_ADDRSTRLEN];

	if ((skai = (struct sockaddr_in *) stun_bind_query(AF_INET)) == NULL)
		return EXIT_FAILURE;
	skai -> sin_addr.s_addr = htonl(skai -> sin_addr.s_addr);
	printf("Transport Address : %s:%d\n", inet_ntop(skai -> sin_family, &skai -> sin_addr, addrbuf, sizeof(addrbuf)), skai -> sin_port);
	free(skai);
	return 0;
}
