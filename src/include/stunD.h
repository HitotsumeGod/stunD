/*
 * Session Traversal Utilities for NAT (STUN) implementation as defined by RFC 8489
 * STUN Attribute types as defined by IANA
 */

#ifndef __STUND_H__
#define __STUND_H__

#include <netinet/in.h>
#include "std/ptools.h"

#define STUN_HDRLEN			(byte) 20
#define STUN_BIND_REQ			(word) 0x0001
#define STUN_BIND_RESP			(word) 0x0101
#define STUN_MAGIC_COOKIE		(dword) 0x2112A442
#define STUN_FAMILY_IPV4		(byte) 0x01
#define STUN_FAMILY_IPV6		(byte) 0x02

#define STUN_TYPE_MAPPED_ADDR		(word) 0x0001
#define STUN_TYPE_USERNAME		(word) 0x0006
#define STUN_TYPE_MESSAGE_INTEGRITY	(word) 0x0008
#define STUN_TYPE_ERROR_CODE		(word) 0x0009
#define STUN_TYPE_XOR_MAPPED_ADDR	(word) 0x0020

#define STUN_MSG_ID_LEN			(byte) 3
#define STUN_ADDR_IPV6_LEN		(byte) 4
#define STUN_CLIENT_BINDPORT		"1665"
#define TRIP_CLIENT_BINDPORT		"1998"

struct stun_msg 
{
	word type;
	word length;
	dword magic;
	dword id[STUN_MSG_ID_LEN];
	struct stun_attrib
	{
	 word type;
	 word length;
	 union stun_attrib_val
	 {
	  struct stun_mapped_addr
	  {
	   byte reserved;
	   byte family;
	   word port;
	   union stun_addr
	   {
	    dword ipv4;
	    dword ipv6[STUN_ADDR_IPV6_LEN];
	   } address;
	  } maddr;
	 } value;
	} attribute;
};

#define stun_bind attribute.value.maddr
#define stun_addr attribute.value.maddr.address

struct stun_server {
	char *name;
	char *port;
};

extern struct stun_server sservers[];
extern const int num_sservers;

extern bool send_stun(int family, socket_t socket, struct stun_msg *message, struct stun_server *serv);
extern struct stun_msg *recv_stun(socket_t socket);
extern struct sockaddr_storage *stun_bind_query(int family, socket_t socket, struct stun_server *serv);
extern bool poll_stun_servers(int family, int amount);
extern bool poll_stun_servers_by_name(int family, struct stun_server *servs[]);
extern bool poll_recv_test(char *hostname);

#endif //__STUND_H__
