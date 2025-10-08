#include <stdio.h>
#include <assert.h>

const char *strstr(const char *str1, const char *str2) {
    assert(str1 && str2);

    if (*str2 == '\0') return str1;

    for (const char *tmps = str1; *tmps != '\0'; tmps++) {
        const char *s1 = tmps;
        const char *s2 = str2;
        while (*s1 != '\0' && *s2 != '\0') {
            if (*s1 != *s2) break;
            s1++,s2++;
        }
        if (*s2 == '\0') return tmps;
    }
    return NULL;
}

int main() {
    const char *s = "hello world, welcome!";
    const char *p1 = "world";
    const char *p2 = "abc";
    const char *p3 = "";

    const char *res1 = strstr(s, p1);
    const char *res2 = strstr(s, p2);
    const char *res3 = strstr(s, p3);

    if (res1) printf("p1 found: %s\n", res1);
    else
        printf("p1 not found\n");

    if (res2) printf("p2 found: %s\n", res2);
    else
        printf("p2 not found\n");

    if (res3) printf("p3 found (empty pattern): %s\n", res3);
    else
        printf("p3 not found\n");

    return 0;
}