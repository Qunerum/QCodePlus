#include "string.h"
#include "types.h"
#include <stdio.h>



const qcpc cmds[] = {
	{"int"},
	{"func"}, // cmd arg0(args)
	{"if"}, // cmd (args)
	{"println"}
};

const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);
void parseLine(char* l) {
	strTrim(l);
}
int main() {
	FILE *in = fopen("program.qcp", "r");
	if (!in) return 1;
	FILE *out = fopen("out.c", "w");
	if (!out) {
		fclose(in);
		return 1;
	}

	char l[4097];
	while (fgets(l, sizeof(l), in)) {
		parseLine(l);
		for (uint i = 0; i < cmdCnt; i++) {
			if (strStartWith(l, cmds[i].name)) {
				uint nameL = strLen(cmds[i].name);
				strShiftLeft(l, sizeof(l), nameL);
				printf("%s\n", l);
			}
		}
	}

	fclose(in);
	fclose(out);
	return 0;
}
