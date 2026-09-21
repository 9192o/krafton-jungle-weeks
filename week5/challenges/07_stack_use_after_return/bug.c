#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 8
typedef struct
{
    char **lines;
    int count;
} LineView;

static void view_set(LineView *out, char **arr, int n)
{
    out->lines = arr;
    out->count = n;
}

static void split_lines(LineView *out, char *text)
{
    char *parts[MAX_LINES]; // 왜 얘랑
    int n = 0;

    for (char *ln = strtok(text, "\n"); ln && n < MAX_LINES; ln = strtok(NULL, "\n"))
        parts[n++] = ln;

    view_set(out, parts, n);
}

static void warm_stack(void)
{
    char *scratch[MAX_LINES]; // 얘랑 같은 parts 배열을 가리키고 있을까?
    for (int i = 0; i < MAX_LINES; i++)
        scratch[i] = (char *)0x4141414141414141ULL; // 오염
    __asm__ volatile(
        ""
        :
        : "r"(scratch)
        : "memory");
}

int main(void)
{
    char text[] = "alpha\nbeta\ngamma";

    LineView v;
    warm_stack(); // 원래 순서에서 바꿈. 위로 올림.
    split_lines(&v, text); // alpha, beta, gamma로 토큰을 나누어서 LineView v 객체 (lines)에 세팅
    
    long checksum = 0;
    for (int i = 0; i < v.count; i++)
        checksum += (unsigned char)v.lines[i][0]; // 메모리 오염으로 접근 자체가 안됨.

    printf("lines = %d, checksum = %ld\n", v.count, checksum);
    return 0;
}
