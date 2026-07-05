#ifndef ERRORS_H
#define ERRORS_H
#include "codes.h"
#define QCP_ERROR_LIST 30

static const char* qcp_error[QCP_ERROR_LIST] = {
	[EXPECTED+000] = "Expected ';'",
	[EXPECTED+001] = "Expected '('",
	[EXPECTED+002] = "Expected ')'",
	[EXPECTED+003] = "Expected '{'",
	[EXPECTED+004] = "Expected '}'",
	[EXPECTED+005] = "Expected identifier",
	[EXPECTED+006] = "Expected '='",

	[UNEXPECTED+000] = "Unexpected ';'",
	[UNEXPECTED+001] = "Unexpected token",

	[INVALID+000] = "Invalid operator",
};

#endif
