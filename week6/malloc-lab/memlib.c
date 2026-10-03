/*
 * memlib.c - 메모리 시스템 시뮬레이션 모델
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

#include "memlib.h"
#include "config.h"

static char *mem_start_brk; /* 힙의 첫 번째 바이트(start)를 가리킴 */
static char *mem_brk;       /* 힙의 마지막 바이트(top)를 가리킴 */
static char *mem_max_addr;  /* 최대 허용 메모리 주소 */

/*
 * mem_init() - 메모리 시스템 초기화 함수.
 */
void mem_init(void)
{
    // MAX_HEAP 만큼 메모리 할당
    if ((mem_start_brk = (char *)malloc(MAX_HEAP)) == NULL)
    {
        fprintf(stderr, "mem_init_vm: malloc error\n");
        exit(1);
    }

    // 메모리 공간 포인터 초기화
    mem_max_addr = mem_start_brk + MAX_HEAP;
    mem_brk = mem_start_brk;
}

/*
 * mem_deinit - 메모리 시스템 해제 함수.
 */
void mem_deinit(void)
{
    free(mem_start_brk);
}

/*
 * mem_reset_brk - 메모리 시스템 리셋 함수.
 */
void mem_reset_brk()
{
    mem_brk = mem_start_brk;
}

/*
 * mem_sbrk - 힙 확장 함수.
 */
void *mem_sbrk(int incr)
{
    char *old_brk = mem_brk;

    // 잘못된 값이 들어왔을 경우
    if ((incr < 0) || ((mem_brk + incr) > mem_max_addr))
    {
        errno = ENOMEM;
        fprintf(stderr, "ERROR: mem_sbrk failed. Ran out of memory...\n");
        return (void *)-1;
    }
    mem_brk += incr;
    return (void *)old_brk;
}

/*
 * mem_heap_lo - 첫 번째 바이트의 주소를 반환하는 함수.
 */
void *mem_heap_lo()
{
    return (void *)mem_start_brk;
}

/*
 * mem_heap_hi - 마지막 바이트의 주소를 반환하는 함수.
 */
void *mem_heap_hi()
{
    return (void *)(mem_brk - 1);
}

/*
 * mem_heapsize() - 힙 크기를 반환하는 함수.
 */
size_t mem_heapsize()
{
    return (size_t)(mem_brk - mem_start_brk);
}

/*
 * mem_pagesize() - 시스템의 페이지 크기를 반환하는 함수.
 */
size_t mem_pagesize()
{
    return (size_t)getpagesize();
}
