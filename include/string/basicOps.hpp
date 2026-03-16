#ifndef INCLUDE_STRING_BASICOPS_HPP
#define INCLUDE_STRING_BASICOPS_HPP
#include <cstddef>
#include <cstdlib>
#include <cstring>
typedef struct String {
  char* ch;
  int len;
} String;

inline int getLen(String* s) {
  int i = 0;
  while (s->ch[i] != '\0') {
    i++;
  }
  s->len = i;
  return i;
}

inline String* makeString(const char* str) {
  if (str == NULL) {
    return NULL;
  }

  int len = 0;
  while (str[len] != '\0') {
    len++;
  }

  String* s_new = (String*)malloc(sizeof(String));
  if (s_new == NULL) {
    return NULL;
  }

  s_new->ch = (char*)malloc(sizeof(char) * (len + 1));
  if (s_new->ch == NULL) {
    free(s_new);
    return NULL;
  }

  memcpy(s_new->ch, str, sizeof(char) * (len + 1));
  s_new->len = len;
  return s_new;
}

inline String* Concat(String* l, String* r) {
  if (l == NULL || r == NULL || l->ch == NULL || r->ch == NULL) {
    return NULL;
  }

  int len = l->len + r->len;

  String* s_new = (String*)malloc(sizeof(String));
  if (s_new == NULL) {
    return NULL;
  }

  char* ch_new = (char*)malloc(sizeof(char) * (len + 1));
  if (ch_new == NULL) {
    free(s_new);
    return NULL;
  }

  memcpy(ch_new, l->ch, sizeof(char) * l->len);
  memcpy(ch_new + l->len, r->ch, sizeof(char) * (r->len + 1));

  s_new->ch = ch_new;
  s_new->len = len;
  return s_new;
}
#endif  // !INCLUDE_STRING_BASICOPS_HPP
