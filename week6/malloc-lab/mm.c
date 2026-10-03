/*
 * malloc-lab 메모리 할당기 구현 과제입니다.
 *
 * 현재 버전은 CS:APP에서 구현되어있는
 * 헤더 4바이트, 푸터 4바이트의 블록 구조, 8바이트 정렬,
 * implicit free list 방식의 명시적 할당기입니다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

team_t team = {
    /* Team name */
    "Jungle-12",
    /* First member's full name */
    "Yang Woongjin",
    /* First member's email address */
    "woong501298@gmail.com",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""};
/* 연산 매크로 */
#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* 블록/정렬/청크 규칙 */
#define W_SIZE 4
#define D_SIZE 8
#define CHUNK_SIZE (1 << 12)

#define METADATA_SIZE D_SIZE
#define HDR_SIZE W_SIZE
#define FTR_SIZE W_SIZE

#define ALLOCATED 1
#define FREED 0

/* 정렬 매크로 */
#define ALIGN(size) (((size) + METADATA_SIZE + D_SIZE - 1) & ~0x7)

/* 메모리 주소에서 4바이트 메타데이터 값 추출/입력 */
#define GET(p) (*(unsigned int *)(p))
#define SET(p, value) (*(unsigned int *)(p) = (value))

/* 메타데이터(헤더/푸터) 값 생성/추출 */
#define SET_METADATA(asize, is_alloc) ((asize) | (is_alloc))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_IS_ALLOC(p) (GET(p) & 0x1)

/* b_ptr는 블록의 payload 시작 주소입니다. */

/* 현재 블록의 헤더/푸터 주소 */
#define HDR(b_ptr) ((char *)(b_ptr) - HDR_SIZE)
#define FTR(b_ptr) ((char *)(b_ptr) + GET_SIZE(HDR(b_ptr)) - METADATA_SIZE)

/* 다음/이전 블록의 payload 시작 주소*/
#define NEXT_BLK(b_ptr) ((char *)(b_ptr) + GET_SIZE(HDR(b_ptr)))
#define PREV_BLK(b_ptr) ((char *)(b_ptr) - GET_SIZE(((char *)(b_ptr) - METADATA_SIZE)))

int mm_init(void);
void *mm_malloc(size_t size);
void *mm_realloc(void *b_ptr, size_t size);

static void *find_fit(size_t asize);
static int place(void *b_ptr, size_t asize);

void mm_free(void *b_ptr);

static void *extend_heap(size_t asize);
static void *coalesce(void *b_ptr);

static char *mm_p;
/*
 * 메모리 할당기를 초기화 합니다.
 *
 * 1. 16바이트 공간 (패딩/프롤로그 헤더/프롤로그 푸터/에필로그 헤더) 을 확장합니다.
 * 2. 4바이트 패딩을 할당합니다.
 * 3. 프롤로그 블록 (asize = 8, is_alloc = 1) 을 할당합니다.
 * 4. 에필로그 블록 (asize = 0, is_alloc = 1) 을 할당합니다.
 * 5. 포인터를 프롤로그-에필로그 사이로 옮기고 힙을 4KB로 확장합니다.
 */
int mm_init(void)
{
    // 16바이트 공간 매핑
    mm_p = mem_sbrk(4 * W_SIZE);
    if (mm_p == (void *)-1)
        return -1;

    // 패딩
    SET(mm_p, 0);

    // 프롤로그 헤더
    mm_p += HDR_SIZE;
    SET(mm_p, SET_METADATA(METADATA_SIZE, ALLOCATED));

    // 프롤로그 푸터
    mm_p += FTR_SIZE;
    SET(mm_p, SET_METADATA(METADATA_SIZE, ALLOCATED));

    // 에필로그 헤더
    mm_p += HDR_SIZE;
    SET(mm_p, SET_METADATA(0, ALLOCATED));

    // 확장
    if (extend_heap(CHUNK_SIZE) == NULL)
        return -1;

    return 0;
}

/*
 * 메모리를 size 만큼 할당합니다.
 *
 * 1. size가 8바이트보다 작거나 같을 경우, 16바이트 블록을 할당합니다.
 * 2. 그 외에는 정렬 규칙에 따라 asize를 조정합니다.
 * 3. asize에 따라서, 적합한 메모리 영역을 찾습니다.
 * 4. 적합한 메모리 영역을 찾았을 경우, 그 주소에 할당합니다.
 * 5. 적합한 메모리 영역을 찾지 못했을 경우, 힙을 확장시킵니다.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extend_size;

    char *b_ptr;

    if (size == 0)
        return NULL;

    // 최소 블록 크기
    if (size <= D_SIZE)
        asize = 2 * D_SIZE;
    else
        asize = ALIGN(size);

    // 총 할당 크기에 맞는 공간 탐색
    b_ptr = find_fit(asize);

    // 탐색 성공 시 할당
    if (b_ptr != NULL)
    {
        place(b_ptr, asize);
        return b_ptr;
    }
    // 탐색 실패 시 힙 확장
    else
    {
        // 최소는 4KiB
        extend_size = MAX(asize, CHUNK_SIZE);

        // 확장 실패 시 NULL 반환
        if ((b_ptr = extend_heap(extend_size)) == NULL)
            return NULL;

        // 확장 성공 시 해당 위치에 할당하고 반환
        place(b_ptr, asize);
        return b_ptr;
    }
}

/*
 * TODO: FIRST FIT
 */
static void *find_fit(size_t asize)
{
}

/*
 * TODO: 실제 할당
 */
static int place(void *b_ptr, size_t asize)
{
}

void *mm_realloc(void *b_ptr, size_t size)
{
}

/*
 * 블록을 해제합니다.
 *
 * 1. 블록 사이즈를 저장합니다.
 * 2. 헤더/푸터에서 할당 여부 영역을 FREED로 변경합니다.
 * 3. 현재 위치에서 병합을 시도합니다.
 */
void mm_free(void *b_ptr)
{
    // 블록 크기 저장
    size_t asize = GET_SIZE(HDR(b_ptr));

    // 헤더/푸터 갱신
    SET(HDR(b_ptr), SET_METADATA(asize, FREED));
    SET(FTR(b_ptr), SET_METADATA(asize, FREED));

    // 병합 시도
    coalesce(b_ptr);
}

/*
 * 크기를 bytes로 받아 힙을 확장합니다.
 * 8바이트 정렬 조건 불만족시 Size를 늘리지 않고 종료합니다.
 *
 * 1. brk를 할당될 크기만큼 뒤로 조정합니다.
 * 2. 에필로그 블록을 해제하고, 새로운 헤더와 푸터를 세팅합니다.
 * 3. 에필로그 블록을 새로 할당합니다.
 * 4. 현재 위치에서 병합을 시도합니다.
 */
static void *extend_heap(size_t asize)
{
    // 정렬 조건 검사
    if (asize % 8 || asize < 16)
        return NULL;

    // brk 조정
    char *b_ptr = mem_sbrk(asize);
    if (b_ptr == (void *)-1)
        return NULL;

    // 에필로그 블록을, 새로운 블록으로 세팅
    SET(HDR(b_ptr), SET_METADATA(asize, FREED));
    SET(FTR(b_ptr), SET_METADATA(asize, FREED));

    // 새 에필로그 블록 할당
    SET(HDR(NEXT_BLK(b_ptr)), SET_METADATA(0, ALLOCATED));

    // 병합 시도
    return coalesce(b_ptr);
}

/*
 * 현재 블록 포인터에서 병합을 시도합니다.
 * 반환값은 "첫" free 블록을 가리키는 포인터입니다.
 *
 * 1. 전 블록의 할당 상태와 후 블록의 할당 상태를 기반으로
 * 2. 4가지 상태로 분기합니다.
 * 3. 각 분기에 따라 다르게 병합하고, 블록 포인터를 반환합니다.
 */
static void *coalesce(void *b_ptr)
{
    // 전/후 블록 할당 상태 추출, 현재 free되는 size 추출
    size_t prev_alloc = GET_IS_ALLOC(FTR(PREV_BLK(b_ptr)));
    size_t next_alloc = GET_IS_ALLOC(HDR(NEXT_BLK(b_ptr)));
    size_t size = GET_SIZE(HDR(b_ptr));

    // 1. 전 할당/후 할당
    if (prev_alloc && next_alloc)
    {
        return b_ptr;
    }
    // 2. 전 할당/후 해제
    else if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDR(NEXT_BLK(b_ptr)));
        SET(HDR(b_ptr), SET_METADATA(size, FREED));
        SET(FTR(b_ptr), SET_METADATA(size, FREED));
    }
    // 3. 전 해제/후 할당
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(FTR(PREV_BLK(b_ptr)));

        SET(FTR(b_ptr), SET_METADATA(size, FREED));
        SET(HDR(PREV_BLK(b_ptr)), SET_METADATA(size, FREED));

        b_ptr = PREV_BLK(b_ptr);
    }
    // 4. 전 해제/후 해제
    else
    {
        size += GET_SIZE(HDR(PREV_BLK(b_ptr))) + GET_SIZE(FTR(NEXT_BLK(b_ptr)));

        SET(HDR(PREV_BLK(b_ptr)), SET_METADATA(size, FREED));
        SET(FTR(NEXT_BLK(b_ptr)), SET_METADATA(size, FREED));

        b_ptr = PREV_BLK(b_ptr);
    }
    return b_ptr;
}
