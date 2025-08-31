#include <stddef.h>

char* some_text[5] = {
  "Première ligne",
  "Deuxième ligne",
  "Troisième ligne",
  "Une quatrième",
  "Pourquoi pas une 5eme :D"
};


/// UTF8 Utilities Helper
static inline int utf8_cp_len(unsigned char c) {
    if (c < 0x80) return 1;
    if ((c >> 5) == 0x6) return 2;
    if ((c >> 4) == 0xE) return 3;
    if ((c >> 3) == 0x1E) return 4;
    return 1; // invalid → treat as 1 to avoid UB
}
static inline int is_cont(unsigned char c) { return (c & 0xC0) == 0x80; }

size_t utf8_next_cp(const char *s, size_t i, size_t n) {
    if (i >= n) return n;
    int L = utf8_cp_len((unsigned char)s[i]);
    if (i + L > n) return n;
    return i + L;
}
size_t utf8_prev_cp(const char *s, size_t i) {
    if (i == 0) return 0;
    size_t j = i - 1;
    int k = 0;
    while (j > 0 && is_cont((unsigned char)s[j])) { j--; if (++k > 3) break; }
    return j;
}

