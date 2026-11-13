#include "string.h"
#include <stdio.h>
// #include <string.h>
#include "types.h"
extern FILE *out;
uint d = 0;
static char
	prt[64] = "printf",
	c[MAX_LINE], a[MAX_LINE];
extern char
	bool8[8],
	bool16[9],
	bool32[9],
	bool64[9];

static void _pv_add(char* s, const uint len, uint* ci, uint* ai, const char m) {
	strShiftLeft(s, len, 2);
	const int j = strFind(s, '\'');
	strAddChar(c, sizeof(c), ci, '%');
	if (j > 0) {
		s[j] = '\0';
		strAddChar(c, sizeof(c), ci, '0');
		strAddStr(c, sizeof(c), ci, s + j + 1);
	}
	strAddChar(c, sizeof(c), ci, m);
	strAddStr(a, sizeof(a), ai, ", ");
	strAddStr(a, sizeof(a), ai, s);
}
static void mkPrt(const uint argc, char argv[MAX_ARGS][MAX_LINE]) {
	uint ci = 0, ai = 0;
	for (uint i = 2; i < argc; i++) {
		const uint len = strLen(argv[i]) + 1;
		if (argv[i][0] == '"') {
			strShiftRight(argv[i], len, 1);
			strShiftLeft(argv[i], len, 2);
			strReplace(argv[i], sizeof(argv[i]), "%", "%%");
			strAddStr(c, sizeof(c), &ci, argv[i]);
		} else if (isNum(argv[i])) strAddStr(c, sizeof(c), &ci, argv[i]);
		else if (_chk(i)) _pv_add(argv[i], len, &ci, &ai, 'i');
		else if (_chk(u)) _pv_add(argv[i], len, &ci, &ai, 'u');
		else if (_chk(f)) _pv_add(argv[i], len, &ci, &ai, 'f');
		else if (_chk(b)) _pv_add(argv[i], len, &ci, &ai, 'b');
		else if (_chk(h)) _pv_add(argv[i], len, &ci, &ai, 'x');
		else if (_chk(H)) _pv_add(argv[i], len, &ci, &ai, 'X');
		else if (_chk(o)) _pv_add(argv[i], len, &ci, &ai, 'o');
	}
}
__cmd(println) {
	c[0] = '\0';
	a[0] = '\0';
	mkPrt(argc, argv);
	_add("%s(\"%s\" \"\\n\"%s);", prt, c, a)
}
__cmd(setprt) { __cu(1, 1, "(print_void_name)")
	_add("extern void %s(const char* format, ...);", argv[2])
	strCopy(prt, argv[2]);
}
__cmd(print) {
	c[0] = '\0';
	a[0] = '\0';
	mkPrt(argc, argv);
	_add("%s(\"%s\"%s);", prt, c, a)
}
__cmd(float) { __cu(1, 1, "<name>(<value>)")
	const state hd = strContains(argv[2], "."), hf = strEndWith(argv[2], "f");
	if (hd && hf) _add("%s %s = %s;", argv[0], argv[1], argv[2])
	else if (hd && !hf) _add("%s %s = %sf;", argv[0], argv[1], argv[2])
	else if (!hd) {
		if (hf) argv[2][strLen(argv[2]) - 1] = '\0';
		_add("float %s = %s.0f;", argv[1], argv[2])
	}
}
__cmd(bool64) { __cu(1, 1, "<name>(<value>)")

}
__cmd(bool32) { __cu(1, 1, "<name>(<value>)")

}
__cmd(bool16) { __cu(1, 1, "<name>(<value>)")

}
__cmd(bool8) { __cu(1, 1, "<name>(<value>)")
	_add("%s %s = %s;", bool8, argv[1], argv[2])
}
__cmd(setb) { __cu(2, 2, "<bool_list_name>(<bool_index>, <value>)")
	_add("%s |= %s << %s;", argv[1], argv[3], argv[2])
}
__cmd(perm) { __cu(2, 2, "<var_type>(<var_name>, <value>)")
	if (!strIs(argv[1],   "int") &&
		!strIs(argv[1], "float") &&
		!strIs(argv[1], "bool8") &&
		!strIs(argv[1],"bool16") &&
		!strIs(argv[1],"bool32") &&
		!strIs(argv[1],"bool64")
	) return;
	_add("const %s %s = %s;", argv[1], argv[2], argv[3])
}
__cmd(func) { __cu(0, MAX_ARGS, "<func_name>([args]) {")
	if (strIs(argv[1], "main")) _add("int %s() {", argv[1])
	else _add("void %s() {", argv[1])
	d++;
}
__cmd(uint) { __cu(1, 1, "<name>(<value>)") _add("unsigned int %s = %s;", argv[1], argv[2]) }
__cmd(int) { __cu(1, 1, "<name>(<value>)") _add("int %s = %s;", argv[1], argv[2]) }
__cmd(set) { __cu(1, 1, "<var_name>(<new_value>)") _add("%s = %s;", argv[1], argv[2]) }
__cmd(for) { __cu(2, 3, "(<var_name>, <repeat_count>)' or 'for (<var_name>, <start_at>, <end_at>)")
	if (argc == 4) _add("for (int %s = 0; %s < %s; %s++) {", argv[2], argv[2], argv[3], argv[2])
	else if (argc == 5) _add("for (int %s = %s; i < %s; i++) {", argv[2], argv[3], argv[4])
	d++;
}
static uint r = 0;
__cmd(rep) {  __cu(1, 1, "(<repeat_count>)")
	r++;
	uint s = 0;
	strAddChar(argv[1], sizeof(argv[1]), &s, '_');
	strAddStr(argv[1], sizeof(argv[1]), &s, "rep");
	strAddNum(argv[1], sizeof(argv[1]), &s, r, 16);
	strAddChar(argv[1], sizeof(argv[1]), &s, '_');
	_add("for (unsigned int %s = 0; %s < %s; %s++) {", argv[1], argv[1], argv[2], argv[1])
	d++;
}
__cmd(add) { __cu(1, 1, "<name>(<value>)")
	if (strIs(argv[2], "1")) _add("%s++;", argv[1])
	else _add("%s += %s;", argv[1], argv[2])
}
__cmd(if) { __cu(1, 1, "(<value>) {")
	_add("if (%s) {", argv[2])
	d++;
}
__cmd(end) { _ib_
	if (d > 0) d--;
	_add("}")
}
__cmd(run) { _ic_
	_add("extern void %s();", argv[1])
	_add("%s();", argv[1])
}

const qcpc cmds[] = {
	_cmd(println),
	_cmd(setprt),
	_cmd(print),
	_cmd(float),
	_cmd(bool64),
	_cmd(bool32),
	_cmd(bool16),
	_cmd(bool8),
	_cmd(setb),
	_cmd(perm),
	_cmd(func),
	_cmd(uint),
	_cmd(int),
	_cmd(set),
	_cmd(for),
	_cmd(rep),
	_cmd(add),
	_cmd(if),
	{"}",_qcp_end},
	{"@",_qcp_run}
};
const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);
