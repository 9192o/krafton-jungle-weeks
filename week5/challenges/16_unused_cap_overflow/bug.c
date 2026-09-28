#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// cap을 안 씀(buf를 malloc 해줘야 할까?)
static void append_field(char *buf, size_t cap, size_t *len, const char *field, char sep)
{
    if (*len == cap)
    {
        cap *= 2;
        buf = realloc(buf, cap);
        if (!buf)
        {
            perror("realloc");
            free(buf);
            exit(1);
        }
    }
    if (*len > 0)
    {
        buf[(*len)++] = sep;
    }
    size_t flen = strlen(field);
    for (size_t i = 0; i < flen; i++)
    {
        buf[(*len)++] = field[i];
    }
    buf[*len] = '\0';
}

static void build_record(char *rec, size_t cap)
{
    const char *fields[] = {
        "id=1042",
        "name=Jonathan",
        "department=Engineering",
        "role=maintainer",
    };
    int n = (int)(sizeof(fields) / sizeof(fields[0]));

    size_t len = 0;
    rec[0] = '\0';
    for (int i = 0; i < n; i++)
    {
        append_field(rec, cap, &len, fields[i], '|');
    }
}

int main(void)
{
    // char rec[24]; 이대로 코드를 실행한다면, rec 배열도 계속 늘려줘야 될 것 같은데.
    char *rec = malloc(24 * sizeof(char *));

    build_record(rec, sizeof(rec));

    printf("record = %s\n", rec);
    return 0;
}
