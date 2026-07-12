#include "commands.h"
#include "tools.h"
#include "main.h"
#include "codes.h"
#include <stdio.h>

int lnNmb = 0, deep = 0;
extern struct QCP_Node* root;
struct QCP_Node* actualFunc;

void madeLine(char* line) {
	lnNmb++;
	trimStart(line, ' ');
	trimStart(line, '\t');
	int l = len(line);
	if (line[l-1] == '\n') { line[l-1] = '\0'; l--; }
	if (l >= 2 && line[0] == '/' && line[1] == '/') return;
	if (l <= 0) return;
	// rm comments
	char list[16][256];
	int max = split(line, ' ', list);
	if (max <= 0) return;
	int rs = reqSemi(list[0]);
	if (rs && line[len(line)-1] != ';') err(lnNmb, line, EXPECTED);
	else if (!rs && line[len(line)-1] == ';') { }
	else if (rs && line[len(line)-1] == ';') { }
	if (LOGS) {
		printf("[");
		for (int i = 0; i < max; i++) printf("'%s', ", list[i]);
		printf("\b\b] Length: %d\n", max);
	}
	runCmd(list, line);
}
