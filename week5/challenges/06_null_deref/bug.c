#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEADERS 32
typedef struct
{
    char *keys[MAX_HEADERS];
    char *vals[MAX_HEADERS];
    int count;
} Headers;

static char *skip_ws(char *s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

static void parse_headers(char *text, Headers *h)
{
    for (char *line = strtok(text, "\n"); line != NULL; line = strtok(NULL, "\n")) // Header line Tokenized
    {
        // colon 위치 찾아서 (그 주소부터 뒤...)
        // 근데 colon이 없으면 얘는 NULL 반환함.
        char *colon = strchr(line, ':');

        if (colon == NULL) // <- 따라서 NULL이면 그냥 다음줄로 넘김.
            continue;

        // 그 콜론을 '\0' 으로 만들고 (문자열 끝 추정)
        // 여기서 colon이 NULL 그게 문제임.
        *colon = '\0';

        char *key = line;
        char *val = skip_ws(colon + 1);

        if (h->count < MAX_HEADERS)
        {
            h->keys[h->count] = key;
            h->vals[h->count] = val;
            h->count++;
        }
    }
}

int main(void)
{

    char raw[] =
        "Host: example.com\n"
        "Accept: */*\n"
        "Connection\n" // <- 얘가 문제임
        "User-Agent: memdbg-cli\n";

    Headers h = {.count = 0};
    parse_headers(raw, &h);

    printf("parsed %d headers\n", h.count);
    for (int i = 0; i < h.count; i++)
        printf("  %s = %s\n", h.keys[i], h.vals[i]);
    return 0;
}
