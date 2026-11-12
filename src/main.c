#include "string.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE 2048
#define MAX_ARGS 64

typedef void (*qcp_handler_t)(uint argc, char argv[MAX_ARGS][MAX_LINE]);
typedef struct {
	const char* name;
	qcp_handler_t handler;
} qcpc;
static FILE *out = 0;
static uint d = 0;
static state
	rio = false, // remove stdio
	mir = false, // make it readable
	rac = false; // run after compilation

#define __cmd(name) static void _qcp_##name(uint argc, char argv[MAX_ARGS][MAX_LINE])
#define _cmd(name) {#name, _qcp_##name}
#define _add(line, ...) {                              \
	if (mir) {                                         \
		for (uint i = 0; i < d; i++) fputc('\t', out); \
		if (line[strLen(line)-1] == '\n')              \
			fprintf(out, line, ##__VA_ARGS__);     \
		else fprintf(out, line "\n", ##__VA_ARGS__);        \
	} else fprintf(out, line, ##__VA_ARGS__);          \
}
#define _chk(c) strStartWith(argv[i], #c":")
#define _ic_ (void)argc;
#define _iv_ (void)argv;
#define _ib_ (void)argc; (void)argv;
state isNum(const char* s) {
	while (*s) {
		if (*s < '0' || *s > '9') return false;
		s++;
	}
	return true;
}

__cmd(int) { _ic_ _add("int %s = %s;", argv[1], argv[2]) }
__cmd(uint) { _ic_ _add("unsigned int %s = %s;", argv[1], argv[2]) }
__cmd(float) { _ic_
	const state hd = strContains(argv[2], "."), hf = strEndWith(argv[2], "f");
	if (hd && hf) _add("%s %s = %s;", argv[0], argv[1], argv[2])
	else if (hd && !hf) _add("%s %s = %sf;", argv[0], argv[1], argv[2])
	else if (!hd) {
		if (hf) argv[2][strLen(argv[2]) - 1] = '\0';
		_add("float %s = %s.0f;", argv[1], argv[2])
	}
}
__cmd(set) { _ic_ _add("%s = %s;", argv[1], argv[2]) }
__cmd(perm) { _ic_
	if (!strIs(argv[1],   "int") &&
		!strIs(argv[1], "float") &&
		!strIs(argv[1], "bool8") &&
		!strIs(argv[1],"bool16") &&
		!strIs(argv[1],"bool32") &&
		!strIs(argv[1],"bool64")
	) return;
	_add("const %s %s = %s;", argv[1], argv[2], argv[3])
}
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
static char prt[64] = "printf";
__cmd(setprt) { _ic_
	_add("extern void %s(const char* format, ...);", argv[2]);
	strCopy(prt, argv[2]);
}
static char c[MAX_LINE], a[MAX_LINE];
static void _pv_add(char* s, const uint len, uint* ci, uint* ai, const char m) {
	strShiftLeft(s, len, 2);
	const int j = strFind(s, '\'');
	strAddChar(c, sizeof(c), ci, '%');
	if (j > 0) {
		s[j] = '\0';
		strAddChar(c, sizeof(c), ci, '0');
		strAddStr(c, sizeof(c), ci, s + j + 1);
	}
	strAddChar(c, sizeof(c), ci, m);
	strAddStr(a, sizeof(a), ai, ", ");
	strAddStr(a, sizeof(a), ai, s);
}
static void mkPrt(const uint argc, char argv[MAX_ARGS][MAX_LINE]) {
	uint ci = 0, ai = 0;
	for (uint i = 2; i < argc; i++) {
		const uint len = strLen(argv[i]) + 1;
		if (argv[i][0] == '"') {
			strShiftRight(argv[i], len, 1);
			strShiftLeft(argv[i], len, 2);
			strReplace(argv[i], sizeof(argv[i]), "%", "%%");
			strAddStr(c, sizeof(c), &ci, argv[i]);
		} else if (isNum(argv[i])) strAddStr(c, sizeof(c), &ci, argv[i]);
		else if (_chk(i)) _pv_add(argv[i], len, &ci, &ai, 'i');
		else if (_chk(u)) _pv_add(argv[i], len, &ci, &ai, 'u');
		else if (_chk(f)) _pv_add(argv[i], len, &ci, &ai, 'f');
		else if (_chk(b)) _pv_add(argv[i], len, &ci, &ai, 'b');
		else if (_chk(h)) _pv_add(argv[i], len, &ci, &ai, 'x');
		else if (_chk(H)) _pv_add(argv[i], len, &ci, &ai, 'X');
		else if (_chk(o)) _pv_add(argv[i], len, &ci, &ai, 'o');
	}
}
__cmd(print) {
	c[0] = '\0';
	a[0] = '\0';
	mkPrt(argc, argv);
	_add("%s(\"%s\"%s);", prt, c, a)
}
__cmd(println) {
	c[0] = '\0';
	a[0] = '\0';
	mkPrt(argc, argv);
	_add("%s(\"%s\" \"\\n\"%s);", prt, c, a)
}

static const qcpc cmds[] = {
	_cmd(println),
	_cmd(setprt),
	_cmd(print),
	_cmd(float),
	_cmd(perm),
	_cmd(func),
	_cmd(uint),
	_cmd(int),
	_cmd(set),
	_cmd(if),
	{"}",_qcp_end},
	{"@",_qcp_run}
};
static const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);

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
#define _chkv(flag, v) if (strIs(argv[i], "-"#flag)) { i++; if (i < argc) v = argv[i]; }
#define _chkf(flag) if (strIs(argv[i], "-"#flag)) flag = true;
int main(int argc, char **argv) {
	char *inp = 0, *outp = 0, *bfp = 0,
	#if defined(_WIN32) || defined(_WIN64)
	*ccp = "x86_64-w64-mingw32-gcc";
	#else
	*ccp = "gcc";
	#endif
	for (int i = 1; i < argc; i++) {
		     _chkv(o, outp)
		else _chkv(i, inp)
		else _chkv(cc, ccp)
		else _chkv(bfc, bfp)
		else _chkf(rio)
		else _chkf(mir)
		else _chkf(rac)
	}

	if (!inp) return 2;
	const char *lastSlash = strrchr(inp, '/');
	#if defined(_WIN32) || defined(_WIN64)
	const char *lastBackslash = strrchr(inp, '\\');
	if (lastBackslash && (!lastSlash || lastBackslash > lastSlash)) lastSlash = lastBackslash;
	#endif
	const char *filename = lastSlash ? lastSlash + 1 : inp;
	char baseName[MAX_LINE];
	snprintf(baseName, sizeof(baseName), "%s", filename);
	char *dot = strrchr(baseName, '.');
	if (dot) *dot = '\0';
	char cPath[MAX_LINE + 32], oPath[MAX_LINE + 32], ePath[MAX_LINE + 32];
	if (bfp) {
		snprintf(cPath, sizeof(cPath), "%s/_qcp_%s.c", bfp, baseName);
		snprintf(oPath, sizeof(oPath), "%s/%s.o", bfp, baseName);
	} else {
		snprintf(cPath, sizeof(cPath), "%s.c", baseName);
		snprintf(oPath, sizeof(oPath), "%s.o", baseName);
	}
	if (outp) snprintf(ePath, sizeof(ePath), "%s", outp);
	else if (bfp) snprintf(ePath, sizeof(ePath), "%s/%s", bfp, baseName);
	else snprintf(ePath, sizeof(ePath), "%s", baseName);
	FILE *in = fopen(inp, "r");
	if (!in) return 3;
	out = fopen(cPath, "w");
	if (!out) {
		fclose(in);
		return 4;
	}
	if (!rio) _add("#include <stdio.h>\n")
	_add("#include <stdint.h>\n")
	char argslt[MAX_ARGS][MAX_LINE], l[MAX_LINE];
	uint qargc = 0;
	while (fgets(l, sizeof(l), in)) {
		strTrim(l);
		if (strStartWith(l, "//")) continue;
		for (uint i = 0; i < cmdCnt; i++) {
			if (strStartWith(l, cmds[i].name)) {
				parseToSingleArgv(l, cmds[i].name, argslt, &qargc);
				if (cmds[i].handler != NULL) cmds[i].handler(qargc, argslt);
				break;
			}
		}
	}
	fclose(in);
	fclose(out);
	char cmd[MAX_LINE * 3];
	snprintf(cmd, sizeof(cmd), "%s -O2 -c \"%s\" -o \"%s\"", ccp, cPath, oPath);
	const int to = system(cmd) == 0;
	snprintf(cmd, sizeof(cmd), "%s \"%s\" -o \"%s\"", ccp, oPath, ePath);
	const int tf = system(cmd) == 0;
	if (!(to && tf)) return 5;
	if (rac) {
		char run_cmd[MAX_LINE + 64];
		snprintf(run_cmd, sizeof(run_cmd), "./%s", ePath);
		system(run_cmd);
	}
	return 0;
}
