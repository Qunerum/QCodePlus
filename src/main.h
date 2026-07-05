#ifndef MAIN_H
#define MAIN_H

#define MAX_LINE_SIZE 4096

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
