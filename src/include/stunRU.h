/*
 * Session Traversal Utilities for NAT (STUN) implementation as defined by RFC 8489
 * STUN Attribute types as defined by IANA
 */

#ifndef __STUNRU_H__
#define __STUNRU_H__

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

struct stun_msg 
{
	word type;
	word length;
	dword magic;
	dword id[3];
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
	    qword ipv6[2];
	   } address;
	  } maddr;
	 } value;
	} attribute;
};

#define stun_bind attribute.value.maddr
#define stun_addr attribute.value.maddr.address

extern const char *stunservers[];
extern const char *stunports[];

extern bool send_stun(socket_t socket, struct stun_msg *message, byte index);
extern struct stun_msg *recv_stun(socket_t socket, byte index);
extern struct sockaddr_in *stun_bind_query(void);

#endif //__STUNRU_H__
