#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROWS 32
#define COLS 4

static void dirty_heap(void)
{
    void *scratch = malloc(ROWS * sizeof(int *));
    if (scratch)
    {
        memset(scratch, 0xAB, ROWS * sizeof(int *));
        free(scratch);
    }
}

static int **make_matrix(void)
{

    int **rows = malloc(ROWS * sizeof(int *));
    if (!rows)
    {
        perror("malloc");
        exit(1);
    }

    for (int i = 0; i < ROWS; i += 2)
    {
        int *r = malloc(COLS * sizeof(int));
        for (int j = 0; j < COLS; j++)
            r[j] = i * COLS + j;
        rows[i] = r;
    }
    return rows;
}

static long row_sum(int **rows, int nrows)
{
    long total = 0;
    for (int i = 0; i < nrows; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            total += rows[i][j];
        }
    }
    return total;
}

int main(void)
{
    dirty_heap();

    int **rows = make_matrix();
    printf("summing %dx%d matrix...\n", ROWS, COLS);

    long s = row_sum(rows, ROWS);

    printf("sum = %ld\n", s);

    for (int i = 0; i < ROWS; i += 2)
        free(rows[i]);
    free(rows);
    return 0;
}
