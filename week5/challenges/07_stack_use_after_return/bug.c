#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES 8
typedef struct
{
    char *lines[MAX_LINES];
    int count;
} LineView;

static void split_lines(LineView *out, char *text)
{
    out->count = 0;
    for (char *ln = strtok(text, "\n"); ln && out->count < MAX_LINES; ln = strtok(NULL, "\n"))
    {
        out->lines[out->count++] = ln;
    }
}

static void warm_stack(void)
{
    char *scratch[MAX_LINES];
    for (int i = 0; i < MAX_LINES; i++)
        scratch[i] = (char *)0x4141414141414141ULL;
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
    warm_stack();          // 원래 순서에서 바꿈. 위로 올림.
    split_lines(&v, text); // alpha, beta, gamma로 토큰을 나누어서 LineView v 객체 (lines)에 세팅

    long checksum = 0;
    for (int i = 0; i < v.count; i++)
        checksum += (unsigned char)v.lines[i][0]; // <- 여기

    printf("lines = %d, checksum = %ld\n", v.count, checksum);
    return 0;
}
