#ifndef __CERRORS_H__
#define __CERRORS_H__

#include <stdio.h>
#include <errno.h>

#define MALLOC_ERR -2
#define REALLOC_ERR -3
#define FORMAT_ERR -4
#define BAD_ARGS_ERR -5
#define PRINTING_ERR -6

#define PRINT_CERR(func)						\
	switch (errno) {						\
	case MALLOC_ERR:						\
		fprintf(stderr, "%s error : MALLOC_ERR\n", func);	\
		break;							\
	case REALLOC_ERR:						\
		fprintf(stderr, "%s error : REALLOC_ERR\n", func);	\
		break;							\
	case FORMAT_ERR:						\
		fprintf(stderr, "%s error : FORMAT_ERR\n", func);	\
		break;							\
	case BAD_ARGS_ERR:						\
		fprintf(stderr, "%s error : BAD_ARGS_ERR\n", func);	\
		break;							\
	case PRINTING_ERR:						\
		fprintf(stderr, "%s error : PRINTING_ERR\n", func);	\
		break;							\
	}								

#endif //__CERRORS_H__
