#include <stdlib.h>
#include "stunD.h"

int main(void) {
	if (!poll_stun_servers(AF_INET, 1))
		return -1;
	return 0;
}
