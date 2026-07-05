#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "tools.h"
#include "parser.h"
#include "compiler.h"
#include "errors.h"

#define RST    "\033[0m"
#define RED    "\033[31m"
#define GREEN  "\033[32m"
#define BLUE   "\033[34m"

void err(int line, const char* lineContent, int code) {
    printf(RED"[QCP Compiler] SYNTAX ERROR [%03d] at line %d:\n", code, line);
    printf("  %d | %s\n", line, lineContent);
    if (code < QCP_ERROR_LIST) printf("%s\n"RST, qcp_error[code]); else printf("Unkown error code!\n"RST);
    exit(1);
}

void printTree(struct QCP_Node* node, int depth) {
    if (node == NULL) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("- Type: '%d', Name: '%s', Value: '%s'\n", node->type, node->name, node->value);
    printTree(node->child, depth + 1);
    printTree(node->next, depth);
}

int main() {
    FILE* in = fopen("test.qcp", "r");
    if (!in) { printf("Cannot open the file!\n"); return 1; }
    struct QCP_Node* root = startCompiler();
    if (root == NULL) return 1;
    printf("\n\n");
    char line[MAX_LINE_SIZE];
    while (fgets(line, sizeof(line), in)) { madeLine(line); }
    fclose(in);
    printTree(root, 0);
    printf("\n\n");

    return endCompiler();
}

struct QCP_Node* createNode(QCP_Node_Type type, char* name, char* value) {
    struct QCP_Node* newNode = malloc(sizeof(struct QCP_Node));
    newNode->type = type;
    copy(name, newNode->name);
    copy(value, newNode->value);
    newNode->child = NULL;
    newNode->next = NULL;
    return newNode;
}
void addChild(struct QCP_Node* parent, struct QCP_Node* newChild) {
    if (parent->child == NULL) parent->child = newChild; else {
        struct QCP_Node* temp = parent->child;
        while (temp->next != NULL) temp = temp->next;
        temp->next = newChild;
    }
}
