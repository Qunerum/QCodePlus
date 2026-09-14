#include "string.h"
#include "types.h"
#include <stdio.h>



const qcpc cmds[] = {
	{"int", }
};
const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);
int main() {
	FILE *in = fopen("program.qcp", "r");
	if (!in) return 1;
	char l[4097];
	while (fgets(l, sizeof(l), in)) {
		for (uint i = 0; i < cmdCnt; i++) {
			if (strStartWith(l, cmds[i].name)) {
				uint nameL = strLen(cmds[i].name);
				strShiftLeft(l, sizeof(l), nameL);
				printf("%s\n", l);
			}
		}
	}

	return 0;
}
