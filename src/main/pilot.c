#include <stdlib.h>
#include "stunD.h"

int main(void) {
	/*
	struct stun_server **servs;
	servs = malloc(sizeof(struct stun_server *) * 3);
	servs[0] = malloc(sizeof(struct stun_server));
	servs[1] = malloc(sizeof(struct stun_server));
	servs[0] -> name = "stun.l.google.com";
	servs[0] -> port = "19302";
	servs[1] -> name = "stun1.l.google.com";
	servs[1] -> port = "3478";
	servs[2] = NULL;
	if (!poll_stun_servers_by_name(AF_INET, servs))
		return -1;
	free(servs[0]);
	free(servs[1]);
	free(servs);*/
	if (!poll_stun_servers(AF_INET, num_sservers))
		return -1;
	return 0;
}
