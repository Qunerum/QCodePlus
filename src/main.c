#include "string.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

#define MAX_LINE 4096
#define MAX_ARGS 64

typedef void (*qcp_handler_t)(uint argc, char argv[MAX_ARGS][MAX_LINE]);
typedef struct {
	const char* name;
	qcp_handler_t handler;
} qcpc;



void _qcp_func(uint argc, char argv[MAX_ARGS][MAX_LINE]) {
	printf("[HANDLER] %s -> Name: '%s'\n", argv[0], argv[1]);
	for (uint i = 2; i < argc; i++) printf(">   arg[%u] = '%s'\n", i, argv[i]);
	putchar('\n');
}

const qcpc cmds[] = {
	{"int", _qcp_func},
	{"func", _qcp_func},
	{"@", _qcp_func},
	{"if", _qcp_func},
	{"println", _qcp_func}
};
const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);

uint parseToSingleArgv(const char* line, const char* cmdName, char argv_buf[MAX_ARGS][MAX_LINE], uint* out_argc) {
	*out_argc = 0;
	uint nameL = strLen(cmdName);
	const char* startParen = strchr(line, '(');
	if (!startParen) return 0;
	char prefix[MAX_LINE];
	uint prefixLen = (uint)(startParen - (line + nameL));
	if (prefixLen >= MAX_LINE) prefixLen = MAX_LINE - 1;
	memcpy(prefix, line + nameL, prefixLen);
	prefix[prefixLen] = '\0';
	strTrim(prefix);
	uint argc = 0;
	snprintf(argv_buf[argc++], MAX_LINE, "%s", cmdName);
	if (prefix[0] != '\0') {
		char *lastToken = prefix, *p = prefix;
		while (*p != '\0') {
			if (*p == ' ' || *p == '\t') lastToken = p + 1;
			p++;
		}
		strTrim(lastToken);
		snprintf(argv_buf[argc++], MAX_LINE, "%s", lastToken);
	} else {
		argv_buf[argc][0] = '\0';
		argc++;
	}
	const char* endParen = strrchr(startParen, ')');
	if (!endParen || endParen < startParen) {
		*out_argc = argc;
		return argc;
	}
	uint len = (uint)(endParen - (startParen + 1));
	if (len >= MAX_LINE) len = MAX_LINE - 1;
	char buf[MAX_LINE];
	memcpy(buf, startParen + 1, len);
	buf[len] = '\0';
	uint i = 0, argStart = 0;
	state inQuotes = 0;
	int parenDepth = 0;
	while (i <= len && argc < MAX_ARGS) {
		const char c = buf[i];
		if (c == '"' && (i == 0 || buf[i - 1] != '\\')) inQuotes = !inQuotes;
		else if (!inQuotes && c == '(') parenDepth++;
		else if (!inQuotes && c == ')') parenDepth--;
		if ((c == ',' && !inQuotes && parenDepth == 0) || c == '\0') {
			uint argLen = i - argStart;
			if (argLen >= MAX_LINE) argLen = MAX_LINE - 1;
			char singleArg[MAX_LINE];
			memcpy(singleArg, buf + argStart, argLen);
			singleArg[argLen] = '\0';
			strTrim(singleArg);
			if (singleArg[0] != '\0' || argLen > 0) snprintf(argv_buf[argc++], MAX_LINE, "%s", singleArg);
			argStart = i + 1;
		}
		i++;
	}
	*out_argc = argc;
	return argc;
}

int main(int argc, char **argv) {
	FILE *in = fopen("program.qcp", "r");
	if (!in) return 1;
	FILE *out = fopen("out.c", "w");
	if (!out) {
		fclose(in);
		return 1;
	}
	char arg_slots[MAX_ARGS][MAX_LINE];
	uint qargc = 0;
	char l[MAX_LINE];
	while (fgets(l, sizeof(l), in)) {
		strTrim(l);
		for (uint i = 0; i < cmdCnt; i++) {
			if (strStartWith(l, cmds[i].name)) {
				parseToSingleArgv(l, cmds[i].name, arg_slots, &qargc);
				if (cmds[i].handler != NULL) {
					printf("%s\n", l);
					cmds[i].handler(qargc, arg_slots);
				}
			}
		}
	}
	fclose(in);
	fclose(out);
	return 0;
}
