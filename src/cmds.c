#include "string.h"
#include <stdio.h>
#include "types.h"
extern FILE *out;
uint d = 0;
static char
	prt[64] = "printf",
	c[MAX_LINE], a[MAX_LINE];
extern char
	sp[2],cz[5],
	bool8[8],bool16[9],
	bool32[9],bool64[9],
	btr[5],bfa[6], //  1 0
	eq[4],co[3],   //  = ,
	lt[4], gt[4],  //  < >
	loe[5],goe[5], // =< >=
	is[5], isn[5], // == !=
	ae[5], oe[5],  // &= |=
	or[3], and[3], // && ||
	sl[5], sr[5],  // << >>
	po[3], pc[3],  //  ( )
	add[4],sub[4]; //  + -

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
	strAddChar(a, sizeof(a), ai, ',');
	if (mir) strAddChar(a, sizeof(a), ai, ' ');
	strAddStr(a, sizeof(a), ai, s);
}
static void parseArg(const uint start, const uint argc, char argv[MAX_ARGS][MAX_LINE]) {
	c[0] = '\0';
	a[0] = '\0';
	uint ci = 0, ai = 0;
	for (uint i = start; i < argc; i++) {
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
		else if (_chk(s)) _pv_add(argv[i], len, &ci, &ai, 's');
	}
}

#define VOID "static void "
#define TEXT "__qcp_text_"
__cmd(use) { __cu(1, 1, "(<lib_name>)")
	if (strIs(argv[2], "text")) {
		_add(VOID TEXT "addc(char* s%sconst uint l%suint* i%sconst char v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!v%sreturn;", po,or,or,or,pc)
		_add("if%s*i%sl%s1%s{", po,lt,sub,pc) d++;
		_add("s[(*i)++]%sv;", eq)
		_add("s[*i]%s'\\0';", eq)
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT "add(char* s%sconst uint l%suint* i%sconst char* v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!v%sreturn;", po,or,or,or,pc)
		_add("if%s!v%sv%s\"(null)\";", po, pc, eq)
		_add("while%s*v%s" TEXT "addc(s%sl%si%s*v++%s;", po,pc,co,co,co,pc)
		d--; _add("}")

		_add(VOID TEXT "shift(char* s%sconst uint l%suint* i%sconst int v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!v%sreturn;", po,or,or,or,pc)
		_add("const int r%sv%s0%s?%s-v%s:%sv;", eq,lt,sp,sp,sp,sp)
		_add("if%sv%s0%s{", po,gt,pc) d++;
		_add("for%sint c%s*i;%sc%s0;%sc--%s{", po,eq,sp,goe,sp,pc) d++;
		_add("const uint n%sc%s+%sr;", eq,sp,sp)
		_add("if%sn%sl%s1%ss[n]%ss[c];", po,lt,sub,pc,eq)
		d--; _add("}")
		_add("for%suint c%s0;%sc%sr%sc%sl%s1;%sc++%ss[c]%s' ';", po,eq,sp,lt,and,lt,sub,sp,pc,eq)
		_add("*i%s(*i%sr%sl%s1%s?%s*i%sr%s:%sl%s1);", eq,add,lt,sub,pc,po,add,pc,po,sub)
		_add("s[*i]%s'\\0';", eq)
		d--; _add("}%selse%s{", sp,sp) d++;
		_add("if%sr%s*i%s{", po,goe,pc) d++;
		_add("s[0]%s%s;", eq,cz)
		_add("*i%s0;", eq)
		_add("return;")
		d--; _add("}")
		_add("uint n%s0;", eq)
		_add("for%suint c%sr;%sc%s*i;%sc++%ss[n++]%ss[c];", po,eq,sp,lt,sp,pc,eq)
		_add("s[n]%s%s;", eq,cz)
		_add("*i%sn;", eq)
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT "trimc(char* s%sconst uint l%suint* i%sconst %s e%sconst char c%s{", co,co,co,bool8,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!c%sreturn;", po,or,or,or,pc)
		_add("if%se%s1%se%s2%s{", po,is,or,is,pc) d++;
		_add("while%s*i%s0%ss[*i%s1]%sc%s(*i)--;", po,lt,and,sub,is,pc)
		_add("s[*i]%s%s;", eq,cz)
		d--; _add("}");
		_add("if%se%s0%se%s2%s{", po,is,or,is,pc) d++;
		_add("uint n%s0;", eq)
		_add("while%ss[n]%ss[n]%sc%sn++;", po,and,is,pc)
		_add("if%s!n%sreturn;", po,pc)
		_add("uint m%s0;", eq)
		_add("while%ss[n%sm]%s{", po,add,pc) d++;
		_add("s[m]%ss[n%sm];", eq,add)
		_add("m++;")
		d--; _add("}")
		_add("s[m]%s%s;", eq,cz)
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT startWith"(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "endWith(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "contains(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "copy(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "replace(char* s%sconst uint l%suint* i%sconst char* a%sconst uint al%sconst char* b%sconst uint bl%s{", co,co,co,co,co,co,pc) d++;
		_add("if (!s || !l || !i || !a || !al || al > l) return;")
		_add("if (!b) b = "";
		_add("/* contains */")
		_add("const int d = (int)bl - (int)al;")
		_add("uint n = 0;")
		_add("while (s[n]) {") d++;
		_add("if (1 /* start with */) {") d++;
		d--; _add("}")
		d--; _add("}")
		_add("")
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "a(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "b(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")

		_add(VOID TEXT "c(char* s%sconst uint l%suint* i%s{", co,co,pc) d++;
		_add("")
		_add("")
		_add("")
		d--; _add("}")
	}
}
__cmd(text) { __cu(1, MAX_ARGS, "cmd(<str_name>, ...)")
	if (strIs(argv[1], "add")) { __cu(2, 2, "add(<str_name>, <text>)")
		const uint l = strLen(argv[3]) - 2;
		if (argv[3][0] == '\"') strShiftLeft(argv[3], sizeof(argv[3]), 1);
		if (argv[3][l] == '\"') argv[3][l] = '\0';
		if (l == 1 || (l == 2 && argv[3][0] == '\\')) _add(TEXT "addc(%s%ssizeof(%s)%s&__str_%s%s'%s');", argv[2],co,argv[2],co,argv[2],co,argv[3])
		else _add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s\"%s\");", argv[1],argv[2],co,argv[2],co,argv[2],co,argv[3])
	}
	else if (strIs(argv[1], "shift")) _add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%s);", argv[1],argv[2],co,argv[2],co,argv[2],co,argv[3])
	else if (strIs(argv[1], "trimc")) _add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%s%s%s);", argv[1],argv[2],co,argv[2],co,argv[2],co,"2",co,argv[3])
}

__cmd(usec) { __cu(1, 1, "(<C_code>)")
	const uint l = strLen(argv[2]) + 1;
	if (argv[2][l - 2] == '\"') argv[2][l - 2] = '\0';
	if (argv[2][0] == '\"') strShiftLeft(argv[2], l, 1);
	if (argv[2][0] == '#') {
		if (!mir) _add("\n");
		_add("%s\n", argv[2])
		return;
	}
	if (!mir && (strStartWith(argv[2], "//") || strStartWith(argv[2], "/*") || strEndWith(argv[2], "*/"))) return;
	_add("%s", argv[2])
}

__cmd(setprt) { __cu(1, 1, "(print_void_name)")
	_add("extern void %s(const char* format, ...);", argv[2])
	strCopy(prt, argv[2]);
}
__cmd(println) {
	parseArg(2, argc, argv);
	_add("%s(\"%s\\n\"%s);", prt, c, a)
}
__cmd(print) {
	parseArg(2, argc, argv);
	_add("%s(\"%s\"%s);", prt, c, a)
}

__cmd(int) { __cu(1, 1, "<name>(<value>)") _add("int %s%s%s;", argv[1], eq, argv[2]) }
__cmd(uint) { __cu(1, 1, "<name>(<value>)") _add("uint %s%s%s;", argv[1], eq, argv[2]) }
__cmd(float) { __cu(1, 1, "<name>(<value>)")
	const state hd = strContains(argv[2], "."), hf = strEndWith(argv[2], "f");
	if (hd && hf) _add("%s %s%s%s;", argv[0], argv[1], eq, argv[2])
	else if (hd && !hf) _add("%s %s%s%sf;", argv[0], argv[1], eq, argv[2])
	else if (!hd) {
		if (hf) argv[2][strLen(argv[2]) - 1] = '\0';
		_add("float %s%s%s.0f;", argv[1], eq, argv[2])
	}
}
__cmd(str) { __cu(1, 2, "<name>(<max_length>, [value])")
	uint l = 0;
	if (argc == 3) _add("char %s[%s];", argv[1], argv[2])
	else if (argc == 4) {
		l = strLen(argv[3]) - 2;
		_add("char %s[%s]%s%s;", argv[1], argv[2], eq, argv[3])
	}
	_add("uint __str_%s%s%u;", argv[1], eq, l);
}
static void blRep(char* a) {
	if (!mir) {
		strReplace(a, MAX_LINE, "true", "1");
		strReplace(a, MAX_LINE, "false", "0");
	}
}
__cmd(bool64) { __cu(1, 1, "<name>(<value>)")
	blRep(argv[2]);
	_add("%s %s%s%s;", bool64, argv[1], eq, argv[2])
}
__cmd(bool32) { __cu(1, 1, "<name>(<value>)")
	blRep(argv[2]);
	_add("%s %s%s%s;", bool32, argv[1], eq, argv[2])
}
__cmd(bool16) { __cu(1, 1, "<name>(<value>)")
	blRep(argv[2]);
	_add("%s %s%s%s;", bool16, argv[1], eq, argv[2])
}
__cmd(bool8) { __cu(1, 1, "<name>(<value>)")
	blRep(argv[2]);
	_add("%s %s%s%s;", bool8, argv[1], eq, argv[2])
}
__cmd(perm) { __cu(2, 2, "<var_type>(<var_name>, <value>)")
	if (!strIs(argv[1],   "int") &&
		!strIs(argv[1], "float") &&
		!strIs(argv[1], "bool8") &&
		!strIs(argv[1],"bool16") &&
		!strIs(argv[1],"bool32") &&
		!strIs(argv[1],"bool64")
	) return;
	if (!mir) {
		strReplace(argv[3], sizeof(argv[3]), "true", "1");
		strReplace(argv[3], sizeof(argv[3]), "false", "0");
	}
	_add("const %s %s%s%s;", argv[1], argv[2], eq, argv[3])
}

__cmd(set) { __cu(1, 1, "<var_name>(<new_value>)") _add("%s = %s;", argv[1], argv[2]) }
__cmd(setb) { __cu(2, 2, "<bool_list_name>(<bool_index>, <value>)")
	if (strIs(argv[3], "false")) _add("%s%s~(%s%s%s);", argv[1], ae, bfa, sl, argv[2])
	else if (strIs(argv[3], "true")) _add("%s%s%s%s%s;", argv[1], oe, btr, sl, argv[2])
}
__cmd(add) { __cu(1, 1, "<name>(<value>)")
	if (strIs(argv[2], "1")) _add("%s++;", argv[1])
	else _add("%s%s+=%s%s;", argv[1], sp, sp, argv[2])
}
__cmd(sub) { __cu(1, 1, "<name>(<value>)")
	if (strIs(argv[2], "1")) _add("%s--;", argv[1])
	else _add("%s%s-=%s%s;", argv[1], sp, sp, argv[2])
}
__cmd(mul) { __cu(1, 1, "<name>(<value>)") if (!strIs(argv[2], "1")) _add("%s%s*=%s%s;", argv[1], sp, sp, argv[2]) }
__cmd(div) { __cu(1, 1, "<name>(<value>)")
	__te(strIs(argv[2], "0"), "Cannot divide by zero!")
	_add("%s%s/=%s%s;", argv[1], sp, sp, argv[2])
}

__cmd(if) { __cu(1, 1, "(<value>) {")
	_add("if%s%s%s{", po, argv[2], pc)
	d++;
}
__cmd(for) { __cu(2, 3, "(<var_name>, <repeat_count>)' or 'for (<var_name>, <start_at>, <end_at>)")
	if (argc == 4) _add("for%sint %s%s0;%s%s%s%s;%s%s++%s{", po, argv[2], eq, sp, argv[2], lt, argv[3], sp, argv[2], pc)
	else if (argc == 5) _add("for%sint %s%s%s;%si%s%s;%si++%s{", po, argv[2], eq, argv[3], sp, lt, argv[4], sp, pc)
	d++;
}
__cmd(rep) {  __cu(1, 1, "(<repeat_count>)")
	static uint r = 0;
	r++;
	uint s = 0;
	strAddChar(argv[1], sizeof(argv[1]), &s, '_');
	strAddStr(argv[1], sizeof(argv[1]), &s, "rep");
	strAddNum(argv[1], sizeof(argv[1]), &s, r, 16);
	strAddChar(argv[1], sizeof(argv[1]), &s, '_');
	_add("for%suint %s%s0;%s%s%s%s;%s%s++%s{", po, argv[1], eq, sp, argv[1], lt, argv[2], sp, argv[1], pc)
	d++;
}
__cmd(func) { __cu(0, MAX_ARGS, "<func_name>([args])")
	if (strIs(argv[1], "main")) _add("int %s(%s{%s", argv[1], pc, rio ? "" : "putchar(0);")
	else _add("void %s(%s{", argv[1], pc)
	d++;
}
__cmd(run) { _ic_
	_add("extern void %s();", argv[1])
	_add("%s();", argv[1])
}
__cmd(end) { _ib_
	if (d > 0) d--;
	_add("}")
}
const qcpc cmds[] = {
	_cmd(use),
	_cmd(text),

	_cmd(usec),
	_cmd(setprt),

	_cmd(println),
	_cmd(print),

	_cmd(int),
	_cmd(uint),
	_cmd(float),
	_cmd(str),
	_cmd(bool64),
	_cmd(bool32),
	_cmd(bool16),
	_cmd(bool8),
	_cmd(perm),

	_cmd(set),
	_cmd(setb),
	_cmd(add),
	_cmd(sub),
	_cmd(mul),
	_cmd(div),

	_cmd(if),
	// elif , else , while
	_cmd(for),
	_cmd(rep),
	// end
	_cmd(func),
	{"@",_qcp_run},
	_cmd(end)
};
const uint cmdCnt = sizeof(cmds) / sizeof(qcpc);
