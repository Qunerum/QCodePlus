#include "string.h"
#include "types.h"
#include <stdio.h>
#include <string.h>

#define MAX_LINE 2048
#define MAX_ARGS 64

typedef void (*qcp_handler_t)(uint argc, char argv[MAX_ARGS][MAX_LINE]);
typedef struct {
	const char* name;
	qcp_handler_t handler;
} qcpc;
FILE *out = 0;
uint d = 0;
#define __cmd(name) static void _qcp_##name(uint argc, char argv[MAX_ARGS][MAX_LINE])
#define _cmd(name) {#name, _qcp_##name}
#define _add(line, ...) { for(uint i = 0; i < d; i++) fputc('\t', out); fprintf(out, line"\n", ##__VA_ARGS__); }
#define _chk(c) strStartWith(argv[i], #c":")
#define _ic_ (void)argc;
#define _iv_ (void)argv;
#define _ib_ (void)argc; (void)argv;

state isNum(const char* s) {
	while (*s) {
		if (*s < '0' && *s > '9') return false;
		s++;
	}
	return true;
}

__cmd(int) { _ic_ _add("int %s = %s;", argv[1], argv[2]) }
__cmd(func) { _ic_
	if (strIs(argv[1], "main")) _add("int %s() {", argv[1])
	else _add("void %s() {", argv[1])
	d++;
}
__cmd(end) { _ib_
	if (d > 0) d--;
	_add("}")
}
__cmd(run) { _ic_
	_add("extern void %s();", argv[1]);
	_add("%s();", argv[1])
}
__cmd(if) { _ic_
	_add("if (%s) {", argv[2])
	d++;
}
static void _pv_add(char* s, const uint len, char* c, const uint sc, char* a, const uint sa, uint* ci, uint* ai, const char m) {
	strShiftLeft(s, len, 2);
	const int j = strFind(s, '\'');
	strAddChar(c, sc, ci, '%');
	if (j > 0) {
		s[j] = '\0';
		strAddChar(c, sc, ci, '0');
		strAddStr(c, sc, ci, s + j + 1);
	}
	strAddChar(c, sc, ci, m);
	strAddStr(a, sa, ai, ", ");
	strAddStr(a, sa, ai, s);
}
__cmd(println) {
	char c[MAX_LINE], a[MAX_LINE];
	uint ci = 0, ai = 0;
	for (uint i = 2; i < argc; i++) {
		const uint len = strLen(argv[i]) + 1;
		if (argv[i][0] == '"') {
			strShiftRight(argv[i], len, 1);
			strShiftLeft(argv[i], len, 2);
			strReplace(argv[i], sizeof(argv[i]), "%", "%%");
			strAddStr(c, sizeof(c), &ci, argv[i]);
		} else if (_chk(v)) {
			strShiftLeft(argv[i], len, 2);
			strAddStr(c, sizeof(c), &ci, "%i");
			strAddStr(a, sizeof(a), &ai, ", ");
			strAddStr(a, sizeof(a), &ai, argv[i]);
		}
		else if (_chk(b)) _pv_add(argv[i], len, c, sizeof(c), a, sizeof(a), &ci, &ai, 'b');
		else if (_chk(h)) _pv_add(argv[i], len, c, sizeof(c), a, sizeof(a), &ci, &ai, 'x');
		else if (_chk(H)) _pv_add(argv[i], len, c, sizeof(c), a, sizeof(a), &ci, &ai, 'X');
		else if (_chk(o)) _pv_add(argv[i], len, c, sizeof(c), a, sizeof(a), &ci, &ai, 'o');
	}
	_add("printf(\"%s\" \"\\n\"%s);", c, a)
}
const qcpc cmds[] = {
	_cmd(int),
	_cmd(func),
	{"}",_qcp_end},
	{"@",_qcp_run},
	_cmd(if),
	_cmd(println)
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
	if (argc <= 2) return 1;
	FILE *in = fopen(argv[1], "r");
	if (!in) return 1;
	out = fopen(argv[2], "w");
	if (!out) {
		fclose(in);
		return 1;
	}
	_add("#include <stdio.h>")
	char arg_slots[MAX_ARGS][MAX_LINE];
	uint qargc = 0;
	char l[MAX_LINE];
	while (fgets(l, sizeof(l), in)) {
		strTrim(l);
		for (uint i = 0; i < cmdCnt; i++) {
			if (strStartWith(l, cmds[i].name)) {
				parseToSingleArgv(l, cmds[i].name, arg_slots, &qargc);
				if (cmds[i].handler != NULL) cmds[i].handler(qargc, arg_slots);
			}
		}
	}
	fclose(in);
	fclose(out);
	return 0;
}
