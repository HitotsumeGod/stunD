/*
 * Session Traversal Utilities for NAT (STUN) implementation defined by RFC 8489
 */

#ifndef __STUNRU_H__
#define __STUNRU_H__

#include <netinet/in.h>
#include "std/ptools.h"

#define STUN_BIND_REQ		0x0001
#define STUN_MAGIC_COOKIE	0x2112A442

union stun_addr {
	dword			ipv4_addr;
	qword			ipv6_addr[2];
};

struct stun_mapped_addr {
	byte			reserved;
	byte			family;
	word			port;
	union stun_addr		*address;
};

union stun_attrib_val {
	struct stun_mapped_addr	*maddr;		
};

struct stun_attrib {
	word			type;
	word			length;
	union stun_attrib_val	*value;
};

struct stun_msg {
	word			type;
	word			length;
	dword			magic;
	dword			id[3];
	struct stun_attrib	*attribute;
};

extern const char *stunservers[];
extern const char *stunports[];

extern bool send_stun(socket_t socket, struct stun_msg *message, byte index);
extern struct stun_msg *recv_stun(socket_t socket, byte index);
extern struct sockaddr_in *stun_bind_query(void);

#endif //__STUNRU_H__
