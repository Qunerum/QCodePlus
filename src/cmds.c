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

#define a(i) argv[i]
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
		const uint len = strLen(a(i)) + 1;
		if (a(i)[0] == '"') {
			strShiftRight(a(i), len, 1);
			strShiftLeft(a(i), len, 2);
			strReplace(a(i), sizeof(a(i)), "%", "%%");
			strAddStr(c, sizeof(c), &ci, a(i));
		} else if (isNum(a(i))) strAddStr(c, sizeof(c), &ci, a(i));
		else if (_chk(i)) _pv_add(a(i), len, &ci, &ai, 'i');
		else if (_chk(u)) _pv_add(a(i), len, &ci, &ai, 'u');
		else if (_chk(f)) _pv_add(a(i), len, &ci, &ai, 'f');
		else if (_chk(b)) _pv_add(a(i), len, &ci, &ai, 'b');
		else if (_chk(h)) _pv_add(a(i), len, &ci, &ai, 'x');
		else if (_chk(H)) _pv_add(a(i), len, &ci, &ai, 'X');
		else if (_chk(o)) _pv_add(a(i), len, &ci, &ai, 'o');
		else if (_chk(s)) _pv_add(a(i), len, &ci, &ai, 's');
	}
}
#define VOID "static void "
#define TEXT "__qcp_text_"
__cmd(use) { __cu(1, 1, "(<lib_name>)")
	if (strIs(a(2), "text")) {
		_add(VOID TEXT "addc(char* s%sconst uint l%suint* i%sconst char v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!v%sreturn;", po,or,or,or,pc)
		_add("if%s*i%sl%s1%s{", po,lt,sub,pc) d++;
		_add("s[(*i)++]%sv;", eq)
		_add("s[*i]%s'\\0';", eq)
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT "add(char* s%sconst uint l%suint* i%sconst char* v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%sreturn;", po,or,or,pc)
		_add("if%s!v%sv%s\"(null)\";", po, pc, eq)
		_add("while%s*v%s" TEXT "addc(s%sl%si%s*v++%s;", po,pc,co,co,co,pc)
		d--; _add("}")

		_add(VOID TEXT "set(char* s%sconst uint l%suint* i%sconst char* v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%sreturn;", po,or,or,pc)
		_add("s[0]%s%s;", eq,cz)
		_add("*i%s0;", eq)
		_add("if%s!v%sreturn;", po,pc)
		_add(TEXT "add(s%sl%si%sv);", co,co,co)
		d--; _add("}")

		_add(VOID TEXT "shift(char* s%sconst uint l%suint* i%sconst int v%s{", co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!v%sreturn;", po,or,or,or,pc)
		_add("const uint r%sv%s0%s?%s-v%s:%sv;", eq,lt,sp,sp,sp,sp)
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
		_add("if%s!s%s!l%s!i%s!e%s!c%sreturn;", po,or,or,or,or,pc)
		_add("if%s*i%sl%s*i%sl%s1;", po,goe,pc,eq,sub)
		_add("s[*i]%s%s;", eq,cz)
		_add("if%se%s&%s1%s{", po,sp,sp,pc) d++;
		_add("while%s*i%s0%ss[*i%s1]%sc%s(*i)--;", po,gt,and,sub,is,pc)
		_add("s[*i]%s%s;", eq,cz)
		d--; _add("}");
		_add("if%se%s&%s2%s{", po,sp,sp,pc) d++;
		_add("uint n%s0;", eq)
		_add("while%sn%s*i%ss[n]%sc%sn++;", po,lt,and,is,pc)
		_add("if%sn%s0%s{", po,gt,pc) d++;
		_add("uint m%s0;", eq)
		_add("while%sn%sm%s*i%s{", po,add,lt,pc) d++;
		_add("s[m]%ss[n%sm];", eq,add)
		_add("m++;")
		d--; _add("}")
		_add("s[m]%s%s;", eq,cz)
		_add("*i%sm;", eq)
		d--; _add("}")
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT "trim(char* s%sconst uint l%suint* i%sconst %s e%s{", co,co,co,bool8,pc) d++;
		_add("if%s!s%s!l%s!i%s!e%sreturn;", po,or,or,or,pc)
		_add("if%s*i%sl%s*i%sl%s1;", po,goe,pc,eq,sub)
		_add("s[*i]%s%s;", eq,cz)
		_add("if%se%s&%s1%s{", po,sp,sp,pc) d++;
		_add("while%s*i%s0%s(s[*i%s1]%s' '%ss[*i%s1]%s'\\n'%ss[*i%s1]%s'\\t'%ss[*i%s1]%s'\\r')%s(*i)--;", po,gt,and,sub,is,or,sub,is,or,sub,is,or,sub,is,pc)
		_add("s[*i]%s%s;", eq,cz)
		d--; _add("}")
		_add("if%se%s&%s2%s{", po,sp,sp,pc) d++;
		_add("uint n%s0;", eq)
		_add("while%sn%s*i%ss[n]%s(s[n]%s' '%ss[n]%s'\\n'%ss[n]%s'\\t'%ss[n]%s'\\r')%sn++;", po,lt,and,and,is,or,is,or,is,or,is,pc)
		_add("if%sn%s0%s{", po,gt,pc) d++;
		_add("uint m%s0;", eq)
		_add("while%sn%sm%s*i%ss[n%sm]%s{", po,add,lt,and,add,pc) d++;
		_add("s[m]%ss[n%sm];", eq,add)
		_add("m++;")
		d--; _add("}")
		_add("s[m]%s%s;", eq,cz)
		_add("*i%sm;", eq)
		d--; _add("}")
		d--; _add("}")
		d--; _add("}")

		_add("static %s " TEXT "startWith(const char* s%sconst char* v%s{", bool8,co,pc) d++;
		_add("if%s!s%s!v%sreturn %s;", po,or,pc,bfa)
		_add("while%s*v%s{", po,pc) d++;
		_add("if%s*s%s*v%sreturn %s;", po,isn,pc,bfa)
		_add("s++;")
		_add("v++;")
		d--; _add("}")
		_add("return %s;", btr)
		d--; _add("}")

		_add("static %s " TEXT "endWith(const char* s%sconst uint l%sconst char* v%sconst uint vl%s{", bool8,co,co,co,pc) d++;
		_add("if%s!s%s!v%svl%sl%sreturn %s;", po,or,or,gt,pc,bfa)
		_add("const char* a%ss%s(l%svl);", eq,add,sub)
		_add("while%s*v%s{", po,pc) d++;
		_add("if%s*a%s*v%sreturn %s;", po,isn,pc,bfa)
		_add("a++;")
		_add("v++;")
		d--; _add("}")
		_add("return %s;", btr)
		d--; _add("}")

		_add("static %s " TEXT "contains(const char* s%sconst char* v%s{", bool8,co,pc) d++;
		_add("if%s!s%s!v%sreturn %s;", po,or,pc,bfa)
		_add("if%s!(*v)%sreturn %s;", po,pc,btr)
		_add("while%s*s%s{", po,pc) d++;
		_add("const char *a%ss%s*b%sv;", eq,co,eq)
		_add("while%s*a%s*b%s*a%s*b%s{", po,and,and,is,pc) d++;
		_add("a++;")
		_add("b++;")
		d--; _add("}")
		_add("if%s!(*b)%sreturn %s;", po,pc,btr)
		_add("s++;")
		d--; _add("}")
		_add("return %s;", bfa)
		d--; _add("}")

		_add("static int " TEXT "find(const char* s%sconst char c%s{", co,pc) d++;
		_add("if%s!s%sreturn -1;", po,pc)
		_add("for%suint i%s0;%ss[i];%si++%sif%ss[i]%sc%sreturn (int)i;", po,eq,sp,sp,pc,po,is,pc)
		_add("return -1;")
		d--; _add("}")

		_add(VOID TEXT "upper(char* s%s{", pc) d++;
		_add("if%s!s%sreturn;", po,pc)
		_add("for%suint i%s0;%ss[i];%si++%sif%s'a'%ss[i]%ss[i]%s'z'%ss[i]%s-=%s32;", po,eq,sp,sp,pc,po,loe,and,loe,pc,sp,sp)
		d--; _add("}")

		_add(VOID TEXT "lower(char* s%s{", pc) d++;
		_add("if%s!s%sreturn;", po,pc)
		_add("for%suint i%s0;%ss[i];%si++%sif%s'A'%ss[i]%ss[i]%s'Z'%ss[i]%s+=%s32;", po,eq,sp,sp,pc,po,loe,and,loe,pc,sp,sp)
		d--; _add("}")

		_add(VOID TEXT "replace(char* s%sconst uint l%suint* i%sconst char* a%sconst uint al%sconst char* b%sconst uint bl%s{", co,co,co,co,co,co,pc) d++;
		_add("if%s!s%s!l%s!i%s!a%s!al%sal%sl%s!" TEXT "contains(s%sa)%sreturn;", po,or,or,or,or,or,goe,or,co,pc)
		_add("if%s!b%sb%s\"\";", po,pc,eq)
		_add("if%s*i%sl%s*i%sl%s1;", po,goe,pc,eq,sub)
		_add("s[*i]%s%s;", eq,cz)
		_add("const int d%s(int)bl%s(int)al;", eq,sub)
		_add("uint n%s0;", eq)
		_add("while%sn%s*i%ss[n]%s{", po,lt,and,pc) d++;
		_add("if%s" TEXT "startWith(&s[n]%sa)%s{", po,co,pc) d++;
		_add("if%sd%s0%s{", po,gt,pc) d++;
		_add("uint m%s(*i%sn)%s?%s(*i%sn)%s:%s0;", eq,gt,sp,sp,sub,sp,sp)
		_add(TEXT "shift(&s[n]%sl%sn%s&m%sd);", co,sub,co,co)
		_add("*i%sn%sm;", eq,add)
		d--; _add("}%selse if%sd%s0%s{", sp,po,lt,pc) d++;
		_add("const uint m%s(uint)(-d);", eq)
		_add("uint o%sn%sbl;", eq,add)
		_add("while%so%sm%s*i%ss[o%sm]%s{", po,add,lt,and,add,pc) d++;
		_add("s[o]%ss[o%sm];", eq,add)
		_add("o++;")
		d--; _add("}")
		_add("s[o]%s%s;", eq,cz)
		_add("*i%so;", eq)
		d--; _add("}")
		_add("for%suint j%s0;%sj%sbl%sn%sj%sl%s1;%sj++%ss[n%sj]%sb[j];", po,eq,sp,lt,and,add,lt,sub,sp,pc,add,eq)
		_add("if%s*i%sn%sbl%sn%sbl%sl%s*i%sn%sbl;", po,lt,add,and,add,lt,pc,eq,add)
		_add("s[*i]%s%s;", eq,cz)
		_add("n%s+=%sbl;", sp,sp)
		d--; _add("}%selse n++;", sp)
		d--; _add("}")
		d--; _add("}")

		_add(VOID TEXT "toInt(const char* s%s{", pc) d++;
		_add("")
		d--; _add("}")

		_add(VOID TEXT "isNum(const char* s%s{", pc) d++;
		_add("")
		d--; _add("}")

		// _add(VOID TEXT "(char* s%s{", pc) d++;
		// _add("")
		// _add("")
		// _add("")
		// d--; _add("}")
	}
}
static uint8 getS(const char* s) { return
	strIs(s, "right") ? 1 :
	strIs(s, "left") ? 2 :
	strIs(s, "both") ? 3 : 0;
}
__cmd(text) { __cu(1, MAX_ARGS, "cmd(<str_name>, ...)")
	if (strIs(a(1), "add")) {
		const uint l = strLen(a(3)) - 2;
		if (a(3)[0] == '\"') strShiftLeft(a(3), sizeof(a(3)), 1);
		if (a(3)[l] == '\"') a(3)[l] = '\0';
		if (l == 1 || (l == 2 && a(3)[0] == '\\')) _add(TEXT "addc(%s%ssizeof(%s)%s&__str_%s%s'%s');", a(2),co,a(2),co,a(2),co,a(3))
		else _add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s\"%s\");", a(1),a(2),co,a(2),co,a(2),co,a(3))
		return;
	}
	if (strIs(a(1), "shift")) {
		_add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%s);", a(1),a(2),co,a(2),co,a(2),co,a(3))
		return;
	}
	if (strIs(a(1), "trimc")) {
		_add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%u%s%s);", a(1),a(2),co,a(2),co,a(2),co,getS(a(4)),co,a(3))
		return;
	}
	if (strIs(a(1), "trim")) {
		_add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%u);", a(1),a(2),co,a(2),co,a(2),co,getS(a(3)))
		return;
	}
	if (strIs(a(1), "startWith")) {
		_add("%s%s" TEXT "%s(%s%s%s)%s%s;", a(3),oe,a(1),a(2),co,a(5),sl,a(4))
		return;
	}
	if (strIs(a(1), "endWith")) {
		_add("%s%s" TEXT "%s(%s%s__str_%s%s%s%s%u)%s%s;", a(3),oe,a(1),a(2),co,a(2),co,a(5),co,strLen(a(5))-2,sl,a(4))
		return;
	}
	if (strIs(a(1), "contains")) {
		_add("%s%s" TEXT "%s(%s%s%s)%s%s;", a(3),oe,a(1),a(2),co,a(5),sl,a(4))
		return;
	}
	if (strIs(a(1), "find")) {
		_add("%s%s" TEXT "%s(%s%s%s);", a(3),eq,a(1),a(2),co,a(4))
		return;
	}
	if (strIs(a(1), "upper")) {
		_add(TEXT "%s(%s);", a(1),a(2))
		return;
	}
	if (strIs(a(1), "lower")) {
		_add(TEXT "%s(%s);", a(1),a(2))
		return;
	}
	if (strIs(a(1), "replace")) {
		_add(TEXT "%s(%s%ssizeof(%s)%s&__str_%s%s%s%s%u%s%s%s%u);", a(1),a(2),co,a(2),co,a(2),co,a(3),co,strLen(a(3))-2,co,a(4),co,strLen(a(4))-2)
		return;
	}
}

__cmd(usec) { __cu(1, 1, "(<C_code>)")
	const uint l = strLen(a(2)) + 1;
	if (a(2)[l - 2] == '\"') a(2)[l - 2] = '\0';
	if (a(2)[0] == '\"') strShiftLeft(a(2), l, 1);
	if (a(2)[0] == '#') {
		if (!mir) _add("\n");
		_add("%s\n", a(2))
		return;
	}
	if (!mir && (strStartWith(a(2), "//") || strStartWith(a(2), "/*") || strEndWith(a(2), "*/"))) return;
	_add("%s", a(2))
}

__cmd(setprt) { __cu(1, 1, "(print_void_name)")
	_add("extern void %s(const char* format, ...);", a(2))
	strCopy(prt, a(2));
}
__cmd(println) {
	parseArg(2, argc, argv);
	_add("%s(\"%s\\n\"%s);", prt, c, a)
}
__cmd(print) {
	parseArg(2, argc, argv);
	_add("%s(\"%s\"%s);", prt, c, a)
}

__cmd(int) { __cu(0, 1, "<name>([value])") _add("int %s%s%s;", a(1), eq, argc == 3 ? a(2) : "0") }
__cmd(uint) { __cu(0, 1, "<name>([value])") _add("uint %s%s%s;", a(1), eq, argc == 3 ? a(2) : "0") }
__cmd(float) { __cu(0, 1, "<name>([value])")
	if (argc == 2) _add("float %s%s0.0f;", a(1), eq)
	else {
		const state hd = strContains(a(2), "."), hf = strEndWith(a(2), "f");
		if (hd && hf) _add("%s %s%s%s;", a(0), a(1), eq, a(2))
		else if (hd && !hf) _add("%s %s%s%sf;", a(0), a(1), eq, a(2))
		else if (!hd) {
			if (hf) a(2)[strLen(a(2)) - 1] = '\0';
			_add("float %s%s%s.0f;", a(1), eq, a(2))
		}
	}
}
__cmd(str) { __cu(1, 2, "<name>(<max_length>, [value])")
	uint l = 0;
	if (argc == 3) _add("char %s[%s];", a(1), a(2))
	else if (argc == 4) {
		l = strLen(a(3)) - 2;
		_add("char %s[%s]%s%s;", a(1), a(2), eq, a(3))
	}
	_add("uint __str_%s%s%u;", a(1), eq, l);
}
static void blRep(char* a) {
	if (!mir) {
		strReplace(a, MAX_LINE, "true", "1");
		strReplace(a, MAX_LINE, "false", "0");
	}
}
__cmd(bool64) { __cu(0, 1, "<name>([value])")
	if (argc == 3) blRep(a(2));
	_add("%s %s%s%s;", bool64, a(1), eq, argc == 3 ? a(2) : bfa)
}
__cmd(bool32) { __cu(0, 1, "<name>([value])")
	blRep(a(2));
	_add("%s %s%s%s;", bool32, a(1), eq, argc == 3 ? a(2) : bfa)
}
__cmd(bool16) { __cu(0, 1, "<name>([value])")
	blRep(a(2));
	_add("%s %s%s%s;", bool16, a(1), eq, argc == 3 ? a(2) : bfa)
}
__cmd(bool8) { __cu(0, 1, "<name>([value])")
	blRep(a(2));
	_add("%s %s%s%s;", bool8, a(1), eq, argc == 3 ? a(2) : bfa)
}
__cmd(perm) { __cu(2, 2, "<var_type>(<var_name>, <value>)")
	if (!strIs(a(1),   "int") &&
		!strIs(a(1), "float") &&
		!strIs(a(1), "bool8") &&
		!strIs(a(1),"bool16") &&
		!strIs(a(1),"bool32") &&
		!strIs(a(1),"bool64")
	) return;
	if (!mir) {
		strReplace(a(3), sizeof(a(3)), "true", "1");
		strReplace(a(3), sizeof(a(3)), "false", "0");
	}
	_add("const %s %s%s%s;", a(1), a(2), eq, a(3))
}

__cmd(set) { __cu(1, 1, "<var_name>(<new_value>)") _add("%s = %s;", a(1), a(2)) }
__cmd(setb) { __cu(2, 2, "<bool_list_name>(<bool_index>, <value>)")
	if (strIs(a(3), "false")) _add("%s%s~(%s%s%s);", a(1), ae, bfa, sl, a(2))
	else if (strIs(a(3), "true")) _add("%s%s%s%s%s;", a(1), oe, btr, sl, a(2))
}
__cmd(add) { __cu(1, 1, "<name>(<value>)")
	if (strIs(a(2), "1")) _add("%s++;", a(1))
	else _add("%s%s+=%s%s;", a(1), sp, sp, a(2))
}
__cmd(sub) { __cu(1, 1, "<name>(<value>)")
	if (strIs(a(2), "1")) _add("%s--;", a(1))
	else _add("%s%s-=%s%s;", a(1), sp, sp, a(2))
}
__cmd(mul) { __cu(1, 1, "<name>(<value>)") if (!strIs(a(2), "1")) _add("%s%s*=%s%s;", a(1), sp, sp, a(2)) }
__cmd(div) { __cu(1, 1, "<name>(<value>)")
	__te(strIs(a(2), "0"), "Cannot divide by zero!")
	_add("%s%s/=%s%s;", a(1), sp, sp, a(2))
}

__cmd(if) { __cu(1, 1, "(<value>) {")
	_add("if%s%s%s{", po, a(2), pc)
	d++;
}
__cmd(for) { __cu(2, 3, "(<var_name>, <repeat_count>)' or 'for (<var_name>, <start_at>, <end_at>)")
	if (argc == 4) _add("for%sint %s%s0;%s%s%s%s;%s%s++%s{", po, a(2), eq, sp, a(2), lt, a(3), sp, a(2), pc)
	else if (argc == 5) _add("for%sint %s%s%s;%si%s%s;%si++%s{", po, a(2), eq, a(3), sp, lt, a(4), sp, pc)
	d++;
}
__cmd(rep) {  __cu(1, 1, "(<repeat_count>)")
	static uint r = 0;
	r++;
	uint s = 0;
	strAddChar(a(1), sizeof(a(1)), &s, '_');
	strAddStr(a(1), sizeof(a(1)), &s, "rep");
	strAddNum(a(1), sizeof(a(1)), &s, r, 16);
	strAddChar(a(1), sizeof(a(1)), &s, '_');
	_add("for%suint %s%s0;%s%s%s%s;%s%s++%s{", po, a(1), eq, sp, a(1), lt, a(2), sp, a(1), pc)
	d++;
}
__cmd(func) { __cu(0, MAX_ARGS, "<func_name>([args])")
	if (strIs(a(1), "main")) _add("int %s(%s{%s", a(1), pc, rio ? "" : "putchar(0);")
	else _add("void %s(%s{", a(1), pc)
	d++;
}
__cmd(run) { _ic_
	_add("extern void %s();", a(1))
	_add("%s();", a(1))
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
