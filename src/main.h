#ifndef MAIN_H
#define MAIN_H

#define LOGS 1
#define MAX_LINE_SIZE 4096
#define RST      "\033[0m"
#define TEXT     "\033[0;38;5;244m"
#define RED      "\033[1;38;5;160m"
#define COMPLETE "\033[1;38;5;2m"
#define ONGOING  "\033[0;38;5;220m"
#define PL       "\033[1;38;5;130m"
#define PLQ      "\033[1;38;5;206m"

typedef enum {
	QCP_ROOT, // 0
	QCP_FUNC, // 1
	QCP_ARG, // 2
	QCP_RETURN, // 3

	QCP_LOCAL_INT, // 4
	QCP_INT // 5
} QCP_Node_Type;
struct QCP_Node {
	QCP_Node_Type type;
	char name[256];
	char value[256];
	char asmCmd[65];
	struct QCP_Node* child;
	struct QCP_Node* next;
};
void err(int line, const char* lineContent, int code);
struct QCP_Node* createNode(QCP_Node_Type type, char* name, char* value);
void addChild(struct QCP_Node* parent, struct QCP_Node* newChild);

#endif
