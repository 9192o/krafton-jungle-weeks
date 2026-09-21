
#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int key;
    long count;
} Bucket;

typedef struct
{
    Bucket *data;
    size_t len, cap;
} Histogram;

static void hist_grow(Histogram *h)
{
    h->cap = h->cap ? h->cap * 2 : 16;
    Bucket *p = realloc(h->data, h->cap * sizeof(Bucket));
    if (!p)
    {
        perror("realloc");
        free(h->data);
        exit(1);
    }
    h->data = p;
}

static Bucket *hist_add(Histogram *h, int key)
{
    if (h->len == h->cap)
        hist_grow(h);
    Bucket *b = &h->data[h->len++];
    b->key = key;
    b->count = 0;
    return b;
}

static long hist_total(const Histogram *h)
{
    long t = 0;
    for (size_t i = 0; i < h->len; i++)
        t += h->data[i].count;
    return t;
}

int main(void)
{
    Histogram h = {.data = NULL, .len = 0, .cap = 0};

    for (int k = 0; k < 200000; k++)
        hist_add(&h, k);

    Bucket *hot = &h.data[100000];
    hot->count = 1;

    for (int k = 200000; k < 600000; k++)
        hist_add(&h, k);

    hot->count += 1000;

    printf("hot=%ld total=%ld len=%zu\n", hot->count, hist_total(&h), h.len);
    free(h.data);
    return 0;
}
