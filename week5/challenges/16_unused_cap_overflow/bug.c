#include <stdio.h>
#include <string.h>

static void append_field(char *buf, size_t cap, size_t *len, const char *field, char sep)
{
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
    (void)cap;
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
    char rec[24];

    build_record(rec, sizeof rec);

    printf("record = %s\n", rec);
    return 0;
}
