#include "types.h"
#include "math.h"
#include <stdarg.h>

uint strLen(const char* str) {
	if (!str) return 0;
	uint i = 0;
	while (str[i] != '\0') i++;
	return i;
}
state strIs(const char* strA, const char* strB) {
	if (!strA || !strB) return strA == strB;
	while (*strA != '\0' && (*strA == *strB)) {
		strA++;
		strB++;
	}
	return *strA == *strB;
}
void strCopy(char* dest, const char* src) {
	if (!dest || !src) return;
	while (*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
}
void strShiftRight(char* str, const uint size, const uint shift) {
	if (!str || size == 0 || shift == 0) return;
	uint len = strLen(str);
	if (len > size - 1) len = size - 1;
	for (int i = len; i >= 0; i--) {
		const uint new_pos = i + shift;
		if (new_pos < size - 1) str[new_pos] = str[i];
	}
	for (uint i = 0; i < shift && i < size - 1; i++) str[i] = ' ';
	str[size - 1] = '\0';
}
void strShiftLeft(char* str, const uint size, const uint shift) {
	if (!str || size == 0 || shift == 0) return;
	uint len = strLen(str);
	if (len > size - 1) len = size - 1;
	if (shift >= len) {
		str[0] = '\0';
		return;
	}
	uint idx = 0;
	for (uint i = shift; i < len; i++) str[idx++] = str[i];
	str[idx] = '\0';
}
state strStartWith(const char* str, const char* prefix) {
	if (!str || !prefix) return false;
	while (*prefix != '\0') {
		if (*str != *prefix) return false;
		str++;
		prefix++;
	}
	return true;
}
state strEndWith(const char* str, const char* suffix) {
	if (!str || !suffix) return false;
	uint lenStr = strLen(str), lenSuff = strLen(suffix);
	if (lenSuff > lenStr) return false;
	const char* pStr = str + (lenStr - lenSuff);
	return strIs(pStr, suffix);
}
state strContains(const char* str, const char* sub) {
	if (!str || !sub) return false;
	if (*sub == '\0') return true;
	while (*str != '\0') {
		const char *h = str, *n = sub;
		while (*h && *n && *h == *n) {
			h++;
			n++;
		}
		if (*n == '\0') return true;
		str++;
	}
	return false;
}
int strFind(const char* str, const char c) {
	if (!str) return -1;
	for (uint i = 0; str[i] != '\0'; i++) if (str[i] == c) return i;
	return -1;
}
void strUpper(char* str) {
	if (!str) return;
	for (uint i = 0; str[i] != '\0'; i++) if (str[i] >= 'a' && str[i] <= 'z') str[i] -= 32;
}
void strLower(char* str) {
	if (!str) return;
	for (uint i = 0; str[i] != '\0'; i++) if (str[i] >= 'A' && str[i] <= 'Z') str[i] += 32;
}
void strTrimStartChar(char* str, const char c) {
	if (!str) return;
	uint i = 0;
	while (str[i] != '\0' && str[i] == c) i++;
	if (i == 0) return;
	uint j = 0;
	while (str[i + j] != '\0') {
		str[j] = str[i + j];
		j++;
	}
	str[j] = '\0';
}
void strTrimEndChar(char* str, const char c) {
	if (!str) return;
	uint len = strLen(str);
	while (len > 0 && str[len - 1] == c) len--;
	str[len] = '\0';
}
void strTrimStart(char* str) {
	if (!str) return;
	uint i = 0;
	while (str[i] != '\0' && (str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r')) i++;
	if (i == 0) return;
	uint j = 0;
	while (str[i + j] != '\0') {
		str[j] = str[i + j];
		j++;
	}
	str[j] = '\0';
}
void strTrimEnd(char* str) {
	if (!str) return;
	uint len = strLen(str);
	while (len > 0 && (str[len - 1] == ' ' || str[len - 1] == '\n' || str[len - 1] == '\t' || str[len - 1] == '\r')) len--;
	str[len] = '\0';
}
void strTrim(char* str) {
	strTrimStart(str);
	strTrimEnd(str);
}
void strAddChar(char* buf, const uint size, uint *index, const char c) {
	if (!buf || size == 0 || !index) return;
	if (*index < size - 1) {
		buf[(*index)++] = c;
		buf[*index] = '\0';
	}
}
void strAddStr(char *buf, const uint size, uint *index, const char* str) {
	if (!buf || size == 0 || !index) return;
	if (!str) str = "(null)";
	while (*str != '\0') strAddChar(buf, size, index, *str++);
}
static char tmp[65];
static const char *digits = "0123456789abcdef", *Digits = "0123456789ABCDEF";
static state du = false;
void strAddNum(char* buf, const uint size,  uint* index, int value, const uint8 base) {
	if (!buf || size == 0 || !index) return;
	if (base != 2 && base != 10 && base != 16) return;
	if (value == 0) {
		strAddChar(buf, size, index, '0');
		return;
	}
	if (value < 0) strAddChar(buf, size, index, '-');
	value = kabs(value);
	uint i = 0;
	while (value > 0) {
		tmp[i++] = (du ? Digits : digits)[value % base];
		value /= base;
	}
	while (i > 0) strAddChar(buf, size, index, tmp[--i]);
}
void strConvertV(char* buf, const uint size, const char* format, va_list args) {
	if (!buf || size == 0 || !format) return;
	uint i = 0;
	buf[0] = '\0';
	for (const char* p = format; *p != '\0'; p++) {
		if (*p != '%') {
			strAddChar(buf, size, &i, *p);
			continue;
		}
		p++;
		if (*p == '\0') break;
		switch (*p) {
			case 's': {
				const char* s = va_arg(args, const char*);
				strAddStr(buf, size, &i, s);
				break;
			}
			case 'c': {
				const char v = (char)va_arg(args, int);
				strAddChar(buf, size, &i, v);
				break;
			}
			case 'd': case 'i': {
				const int v = va_arg(args, int);
				strAddNum(buf, size, &i, v, 10);
				break;
			}
			case 'u': {
				const int v = kabs(va_arg(args, int));
				strAddNum(buf, size, &i, v, 10);
				break;
			}
			case 'x': {
				du = false;
				const int v = va_arg(args, int);
				strAddNum(buf, size, &i, v, 16);
				break;
			}
			case 'X': {
				du = true;
				const int v = va_arg(args, int);
				strAddNum(buf, size, &i, v, 16);
				break;
			}
			case 'q': {
				const uint8 v = kclamp(va_arg(args, int), 0, 255);
				strAddStr(buf, size, &i, "\xff[");
				if (v >= 100) strAddChar(buf, size, &i, ' ');
				if (v >= 10) strAddChar(buf, size, &i, ' ');
				strAddNum(buf, size, &i, v, 10);
				break;
			}
			case '%': { strAddChar(buf, size, &i, '%'); break; }
			default: {
				strAddChar(buf, size, &i, '%');
				strAddChar(buf, size, &i, *p);
				break;
			}
		}
	}
}
void strConvert(char* buf, const uint size, const char* format, ...) {
	va_list args;
	va_start(args, format);
	strConvertV(buf, size, format, args);
	va_end(args);
}
void strReplace(char* str, const uint size, const char* search, const char* replaceWith) {
	if (!str || size == 0) return;
	if (!search || *search == '\0') return;
	if (!replaceWith) replaceWith = "";
	if (!strContains(str, search)) return;
	const uint searchLen = strLen(search), replaceLen = strLen(replaceWith);
	const int diff = (int)replaceLen - (int)searchLen;
	uint i = 0;
	while (str[i] != '\0') {
		if (strStartWith(&str[i], search)) {
			if (diff > 0) strShiftRight(&str[i], size - i, diff);
			else if (diff < 0) strCopy(&str[i + replaceLen], &str[i + searchLen]);
			for (uint j = 0; j < replaceLen; j++) if (i + j < size - 1) str[i + j] = replaceWith[j];
			i += replaceLen;
		} else i++;
	}
}
int strToInt(const char* s) {
	if (!s) return 0;
	int v = 0;
	const state isN = *s == '-';
	if (isN) s++;
	while (*s) {
		v = v * 10 + (*s - '0');
		s++;
	}
	return isN ? -v : v;
}
