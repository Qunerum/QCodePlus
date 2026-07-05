#ifndef TOOLS_H
#define TOOLS_H

static inline int len(char* text) { int i = 0; while (text[i] != '\0') i++; return i; }
static inline int is(char* a, char* b) { while (*a != '\0' && *b != '\0') { if (*a != *b) return 0; a++; b++; } return (*a == *b); }
static inline int startWith(char* text, char* start) { while (*start != '\0') { if (*text == '\0' || *text != *start) return 0; text++; start++; } return 1; }
static inline void moveLeft(char* text, int n) { int l = len(text), i = 0; while (i < l - n) { text[i] = text[i + n]; i++; } text[l - n] = '\0';}
static inline void trimStart(char* text, char c) { int s = 0; while (text[s] && text[s] == c) s++; moveLeft(text, s); }
static inline int split(char* text, char d, char list[16][256]) {
	int i = 0, t = 0; while (*text != '\0' && i < 16) { if (*text == d) { list[i][t] = '\0'; i++; t = 0; } else if (t < 255) { list[i][t] = *text; t++; } text++; } list[i][t] = '\0'; return i + 1; }
static inline void copy(char* from, char* to) { int i = 0; while(from[i]) { to[i] = from[i]; i++; } to[i] = '\0'; }
static inline int contains(char* text, char d) { while (*text) { if (*text == d) return 1; text++; } return 0; }
static inline int splitSimple(char* text, char d, char* a, char* b, int* ao, int* bo) {
	if (!contains(text, d)) { copy(text, a); return 0; }
	int s = 0, ai = 0, bi = 0;
	while (*text) {
		if (*text == d) { s = 1; text++; continue; }
		if (!s) { a[ai] = *text; ai++; } else { b[bi] = *text; bi++; }
		text++;
	}
	if (ao) *ao = ai;
	if (bo) *bo = bi;
	return 1;
}

#endif
