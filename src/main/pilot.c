#include <stdlib.h>
#include <string.h>
#include "stunD.h"

const char *usage = "usage:\n   stunD\n   stunD <num_servers>\n   stunD -names <SERVNAME:SERVPORT> ...\n   stunD -help (this)\n";

int main(int argc, char *argv[])
{
	struct stun_server **servs;
	int num, ind;

	if (argc == 1) {
		if (!poll_stun_servers(AF_INET, num_sservers))
			return EXIT_FAILURE;
	} else if (strcmp(argv[1], "-names") == 0) {
		if (argc == 2) {
			printf("\n%s\n", usage);
			return EXIT_FAILURE;
		} else {
			//argc-1 instead of argc-2 because we need the extra NULL entry to terminate the array
			if ((servs = malloc(sizeof(struct stun_server *) * argc - 1)) == NULL) {
				perror("malloc() error");
				return EXIT_FAILURE;
			}
			for (int i = 0; i < argc - 2; i++) {
				ind = 0;
				if ((servs[i] = malloc(sizeof(struct stun_server))) == NULL) {
					perror("malloc() error");
					return EXIT_FAILURE;
				}
				for (int ii = 0; ii < strlen(argv[i + 2]); ii++)
					if (argv[i + 2][ii] == ':') {
						ind = ii;
						break;
					}
				if (ind == 0) {
					printf("\n%s\n", usage);
					return EXIT_FAILURE;
				}
				servs[i] -> name = strndup(argv[i + 2], ind);
				servs[i] -> port = strdup(argv[i + 2] + ++ind);
			}
			servs[argc - 2] = NULL;
			if (!poll_stun_servers_by_name(AF_INET, servs))
				return EXIT_FAILURE;
			for (int i = 0; i < argc - 2; i++) {
				free(servs[i] -> name);
				free(servs[i] -> port);
				free(servs[i]);
			}
			free(servs);
		}
	} else if (strcmp(argv[1], "-help") == 0) { 
		printf("\n%s\n", usage);
		return EXIT_SUCCESS;
	} else if ((num = atoi(argv[1])) != 0) {
		if (!poll_stun_servers(AF_INET, num))
			return EXIT_FAILURE;
	} else {
		printf("\n%s\n", usage);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
