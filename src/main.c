#include "string.h"
#include "types.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern uint d;
FILE *out = 0;
state
	rio = false, // remove stdio
	mir = false, // make it readable
	rac = false; // run after compilation

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
extern const qcpc cmds[];
extern const uint cmdCnt;
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
