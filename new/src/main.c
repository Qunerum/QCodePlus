#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#define LOG 1

#define MAX_LINE 4096 + 1
#define MAX_SUB 1024 + 1
#define MAX_FUNC_LINES 2048
#define MAX_ARGS 256

void err(int i, char* text) { if (!i) return; printf("%s\n", text); exit(1); }

FILE *qasm;
void add(int t, const char* format, ...) {
	if (!qasm) return;
	va_list args;
	va_start(args, format);
	if (t) fprintf(qasm, "\t");
	vfprintf(qasm, format, args);
	fprintf(qasm, "\n");
	va_end(args);
}

typedef char _arg[MAX_ARGS][MAX_SUB];
int len(char* t) {
	int x = 0;
	while(t[x] != '\0') x++;
	return x;
}
void mvL(char* t) {
	int l = len(t);
	if (l == 1) { t[0] = '\0'; return; }
	for (int i = 1; i <= l; i++) t[i-1] = t[i];
}
void mvLbc(char* t, int cnt) { for (int i = 0; i < cnt; i++) mvL(t); }
void cutL(char* t, char c) { while(t[0] == c) mvL(t); }
void cutR(char* t, char c) {
	int l = len(t) - 1;
	while(t[l] == c && l > 0) {
		mvL(t);
		l = len(t) - 1;
	}
}
void cpy(char* from, char* to) {
	int i = 0;
	while(from[i]) {
		to[i] = from[i];
		i++;
	}
	to[i] = '\0';
}
void cpyF(char* from, char* to, int cnt) {
	int i = 0;
	for (i = 0; i < cnt; i++) {
		if (!from[i]) return;
		to[i] = from[i];
	}
	to[i] = '\0';
}
int is(char* a, char* b) {
	while(*a && *b) {
		if (*a != *b) return 0;
		a++;
		b++;
	}
	return 1;
}
int startWith(char* t, char* st) {
	while (*t && *st) {
		if (*t != *st) return 0;
		t++;
		st++;
	}
	return 1;
}
int contains(char* t, char c) {
	while(*t != '\0') {
		if (*t == c) return 1;
		t++;
	}
	return 0;
}
int fnd(char* t, char c) {
	if (!contains(t, c)) return -1;
	int i = 0;
	while(t[i] != c) i++;
	return i + 1;
}
int fndR(char* t, char c) {
	if (!contains(t, c)) return -1;
	int i = len(t) - 1;
	while(t[i] != c && i > 0) i--;
	return i;
}
void trim(char* t, char c) {
	cutL(t, c);
	cutR(t, c);
}
void split(_arg args, int* argc, char* t, char c) {
	char bfr[MAX_LINE];
	cpy(t, bfr);
	if (!contains(t, c)) {
		cpy(t, args[0]);
		trim(args[0], ' ');
		if (argc && len(args[0]) > 0) *argc = 1;
		return;
	}
	if (argc) *argc = 1;
	while (contains(bfr, c)) {
		int i = fnd(bfr, c);
		cpyF(bfr, args[0], i-1);
		trim(args[0], ' ');
		mvLbc(bfr, i);
		if (argc) *argc = *argc + 1;
	}
	cpy(bfr, args[0]);
	trim(args[0], ' ');
}

// ! = = = = = = = = = = FUNCTIONS = = = = = = = = = = !
int mainCreated = 0;
typedef struct {
	char name[MAX_SUB];
	int lineCount;
	char lines[MAX_FUNC_LINES][MAX_LINE];
} qcpFunc;
qcpFunc* funcs;
int funcCount = 0;
void createFunc(char* name) {
	qcpFunc* new = realloc(funcs, (funcCount + 1) * sizeof(qcpFunc));
	err(!new, "Cannot add a function!");
	funcs = new;
	cpy(name, funcs[funcCount].name);
	if (is(name, "main")) mainCreated = 1;
	funcs[funcCount].lineCount = 0;
	funcCount++;
}
void addLine(char* line) {
	int i = funcCount - 1;
	cpy(line, funcs[i].lines[funcs[i].lineCount]);
	funcs[i].lineCount++;
}
// ! = = = = = = = = = = VARS = = = = = = = = = = !
#define type_int 0
typedef struct { char name[MAX_SUB]; int type; } qcpVar;
qcpVar* vars;
int varCount = 0;
void createVar(char* name, int type, char* value) {
	qcpVar* new = realloc(vars, (varCount + 1) * sizeof(qcpVar));
	err(!new, "Cannot add a variable!");
	vars = new;
	cpy(name, vars[varCount].name);
	vars[varCount].type = type;
	// value
}
// ! = = = = = = = = = = COMMANDS = = = = = = = = = = !
typedef struct { int isBlock; char* cmd; void (*handler)(char*, _arg, int); int args; } qcpCmd;
void qvFunc(char* name, _arg args, int argc) {
	createFunc(name);
}
// = = = Variables
void qvInt(char* name, _arg args, int argc) {
	printf("Creating int with value: %s\n", args[0]);
}
// = = = Commands
void qvPrintln(char* name, _arg args, int argc) {
	//
}
qcpCmd cmds[] = {
	{1,"func", qvFunc, -1},
	{0,"int", qvInt, 1},

	{0,"println", qvPrintln, -1}
};
int cmdCount = sizeof(cmds) / sizeof(qcpCmd);
// ! = = = = = = = = = = MAIN = = = = = = = = = = !
int main() {
	FILE *file = fopen("program.qcp", "r");
	if (!file) {
		file = fopen("program.qcp", "w");
		fprintf(file, "func main() {\n");
		fprintf(file, "\tint num(26);\n\n");
		fprintf(file, "\tprintln(\"Decimal: \", v:num);\n");
		fprintf(file, "\tprintln(\"Binary (8): \", b:num'8);\n");
		fprintf(file, "\tprintln(\"HEX (4): \", h:num'4);\n");
		fprintf(file, "}\n");
		fclose(file);
	}
	file = fopen("program.qcp", "r");
	err(!file, "Failed to open qcp file!\n");
	qasm = fopen(".obj/out.qa", "w");
	add(0,". ; db - 255 - 8B");
	add(0,". ; dw - 65.535 - 16B");
	add(0,". ; dd - 4.294.967.295 - 32B");
	add(0,". ; dq - 18.446.744.073.709.551.615 - 64B\n");
	add(0,". ; sm - start main");
	add(0,". ; em - end main");
	add(0,". ; step - function");
	add(0,". ; es - end func");
	add(0,". ; call - call");
	add(0,". ; prt - print text");
	add(0,". ; prtn - print num");
	add(0,". ; prtnb - print num binary");
	add(0,". ; prtnh - print num hex");
	add(0,". ; prti - print int");
	add(0,". ; prtib - print int binary");
	add(0,". ; prtih - print int hex");
	add(0,". ; prtln - print line\n");
	add(0,". ; add - add num to int");
	add(0,". ; addi - add int to int");
	add(0,". ; sub - sub num from int");
	add(0,". ; subi - sub int from int");
	add(0,". ; mul - mul num by int");
	add(0,". ; muli - mul int by int");
	add(0,". ; div - div int by num");
	add(0,". ; divi - div int by int\n");
	add(0,". ; . - write in assembler");
	add(0,". ; .t - write in assembler with tab\n");
	add(0,"data");
	add(0,"end");
	add(0,"");
	add(0,"");
	char buffer[MAX_LINE];
	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		if (buffer[0] == '\n' || buffer[0] == '\0') continue;
		int l = len(buffer);
		if (buffer[l - 1] == '\n') { buffer[l - 1] = '\0'; l--; }
		cutL(buffer, ' ');
		cutL(buffer, '\t');
		int ps = fnd(buffer, '('), pe = fndR(buffer, ')');
		if (ps < 0 || pe < 0 || pe < ps) continue;
		char argBfr[MAX_LINE];
		_arg args = {0};
		cpy(buffer, argBfr);
		mvLbc(argBfr, ps);
		pe = fndR(argBfr, ')');
		argBfr[pe] = '\0';
		int argc = 0;
		split(args, &argc, argBfr, ',');
		for (int i = 0; i < cmdCount; i++) {
			if (startWith(buffer, cmds[i].cmd)) {
				if (LOG) printf("cmd: '%s'\n", cmds[i].cmd);
				char r = ';';
				if (cmds[i].isBlock) r = '{';
				err(buffer[len(buffer) - 1] != r, "You need ';' or '{'!");
				int cl = len(cmds[i].cmd);
				mvLbc(buffer, cl);
				int s = fnd(buffer, '(');
				err(s < 0, "Error");
				buffer[s-1] = '\0';
				cutL(buffer, ' ');
				if (LOG) printf("    Name: '%s'\n", buffer);
				if (LOG) printf("    Req argC: %i\n", cmds[i].args);
				if (LOG) printf("    Args: '%s' [%i]\n", argBfr, argc);
				cmds[i].handler(buffer, args, argc);
			}
		}
	}
	fclose(file);
	fclose(qasm);
	if (LOG) {
		printf("FUNCS:\n");
		for (int i = 0; i < funcCount; i++) {
			printf("- %s:\n", funcs[i].name);
			for (int l = 0; l < funcs[i].lineCount; l++) printf("---|%s\n", funcs[i].lines[l]);
		}
		printf("\n");
	}
	if (!mainCreated) return 1;
	return 0;
}
