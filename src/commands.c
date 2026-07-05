#include "commands.h"
#include "tools.h"
#include "main.h"
#include "codes.h"
#include <stdio.h>
extern int lnNmb, deep;
int inString = 0;
char line[MAX_LINE_SIZE];
extern struct QCP_Node* root;
extern struct QCP_Node* actualFunc;
/*
void exampleCmd(char list[16][256]) {

}
*/
void funcCmd(char list[16][256]) {
	printf("= = = RUNNING FUNC = =\n");
	deep++;
	char name[256];
	char args[1024];
	int argLen = 0;
	splitSimple(list[1], '(', name, args, NULL, &argLen);
	if (args[argLen - 1] == ')') {
		args[argLen - 1] = '\0';
		printf("Name: '%s' Args: '%s'\n", name, args);
	} else err(lnNmb, line, EXPECTED+2);
	struct QCP_Node* fn = createNode(QCP_FUNC, list[1], "");
	actualFunc = fn;
	addChild(root, fn);
}
void endFuncCmd(char list[16][256]) {

}

void intCmd(char list[16][256]) {
	// int , y , = , 7;

}

QCP_Command cmds[] = {
	{"func", 0, funcCmd},
	{"}", 0, endFuncCmd},
	{"int", 1, intCmd}
};
int cmdCount = sizeof(cmds) / sizeof(QCP_Command);
void runCmd(char list[16][256], char _line[MAX_LINE_SIZE]) {
	for (int i = 0; i < cmdCount; i++) {
		if (is(cmds[i].cmd, list[0])) {
			copy(_line, line);
			cmds[i].handler(list);
			inString = 0;
			return;
		}
	}
}
int reqSemi(char* data) { for (int i = 0; i < cmdCount; i++) { if (is(cmds[i].cmd, data)) { return cmds[i].reqSemi; } } return 1; }
