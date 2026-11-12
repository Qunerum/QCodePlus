#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>
#include <stdint.h>

typedef int64_t int64;
typedef uint64_t uint64;
typedef int32_t int32;
typedef uint32_t uint32;
typedef int16_t int16;
typedef uint16_t uint16;
typedef int8_t int8;
typedef uint8_t uint8;

typedef size_t vsize;
typedef uintptr_t uiptr;

typedef uint8 state;
#define false 0
#define true 1
typedef unsigned int uint;

#define MAX_LINE 2048
#define MAX_ARGS 64

typedef void (*qcp_handler_t)(uint argc, char argv[MAX_ARGS][MAX_LINE]);
typedef struct {
	const char* name;
	qcp_handler_t handler;
} qcpc;

extern state
	rio, // remove stdio
	mir, // make it readable
	rac; // run after compilation

#define __cmd(name) static void _qcp_##name(uint argc, char argv[MAX_ARGS][MAX_LINE])
#define _cmd(name) {#name, _qcp_##name}
#define _add(line, ...) { if (mir) { \
	for (uint i = 0; i < d; i++) fputc('\t', out); \
		if (line[strLen(line)-1] == '\n') \
			fprintf(out, line, ##__VA_ARGS__); \
		else fprintf(out, line "\n", ##__VA_ARGS__); \
	} else fprintf(out, line, ##__VA_ARGS__); \
}
#define _chk(c) strStartWith(argv[i], #c":")
#define _ic_ (void)argc;
#define _iv_ (void)argv;
#define _ib_ (void)argc; (void)argv;

#endif
