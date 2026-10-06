/*
 * Malloc Lab - CS:APP 기반의 묵시적 가용 리스트 할당기.
 * 헤더와 푸터는 각각 4바이트이며, payload는 8바이트로 정렬합니다.
 * 블록 크기는 헤더, payload, 패딩, 푸터를 포함한 전체 크기입니다.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

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
    "Joung Seong Young",
    /* Second member's email address (leave blank if none) */
    "sam12057@gmail.com"};
/* 연산 매크로 */
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

/* 메타데이터 크기, 정렬 단위, 기본 힙 확장량, 최대 할당 크기 */
#define W_SIZE 4
#define D_SIZE 8
#define CHUNK_SIZE (1 << 12)

#define METADATA_SIZE D_SIZE
#define HDR_SIZE W_SIZE
#define FTR_SIZE W_SIZE

#define ALLOCATED 1
#define FREED 0

#define MAX_SIZE (size_t)-1

/* payload 요청에 헤더와 푸터를 더하고, 전체 크기를 8의 배수로 올림 */
#define ADJUST_SIZE(size) (((size) + METADATA_SIZE + D_SIZE - 1) & ~0x7)

/* 주소 p의 4바이트 메타데이터 읽기와 쓰기 */
#define GET(p) (*(unsigned int *)(p))
#define SET(p, value) (*(unsigned int *)(p) = (value))

/* 전체 블록 크기와 할당 비트의 조합 및 추출 */
#define SET_METADATA(asize, is_alloc) ((asize) | (is_alloc))

#define GET_SIZE(p) (GET(p) & ~0x7)
#define GET_IS_ALLOC(p) (GET(p) & 0x1)

/* 아래 주소 매크로의 b_ptr는 블록의 payload 시작 주소 */

/* 현재 블록의 헤더/푸터 주소 */
#define HDR(b_ptr) ((char *)(b_ptr) - HDR_SIZE)
#define FTR(b_ptr) ((char *)(b_ptr) + GET_SIZE(HDR(b_ptr)) - METADATA_SIZE)

/* 다음/이전 블록의 payload 시작 주소 */
#define NEXT_BLK(b_ptr) ((char *)(b_ptr) + GET_SIZE(HDR(b_ptr)))
#define PREV_BLK(b_ptr) ((char *)(b_ptr) - GET_SIZE(((char *)(b_ptr) - METADATA_SIZE)))

/* 이전 블록의 헤더/푸터 주소 */
#define PREV_HDR(b_ptr) (HDR(PREV_BLK(b_ptr)))
#define PREV_FTR(b_ptr) ((char *)(b_ptr) - METADATA_SIZE)

/* 다음 블록의 헤더 주소 */
#define NEXT_HDR(b_ptr) (HDR(NEXT_BLK(b_ptr)))
#define NEXT_FTR(b_ptr) (FTR(NEXT_BLK(b_ptr)))

static void *find_fit(size_t asize);
static void place(void *b_ptr, size_t asize);

static void *extend_heap(size_t asize);
static void *coalesce(void *b_ptr);

static char *mm_p; /* 첫 일반 블록의 payload를 가리키는 탐색 시작점 */
/*
 * mm_init - 패딩과 경계 블록을 만들고 첫 가용 블록을 확보합니다.
 * 초기 16바이트는 패딩, 프롤로그 헤더/푸터, 에필로그 헤더로 구성됩니다.
 * 성공하면 0, 힙 확보에 실패하면 -1을 반환합니다.
 */
int mm_init(void)
{
    /* 초기 경계 구조를 위한 힙 공간 확보 */
    mm_p = mem_sbrk(4 * W_SIZE);
    if (mm_p == (void *)-1)
        return -1;

    /* 첫 payload의 8바이트 정렬을 위한 패딩 */
    SET(mm_p, 0);

    /* 크기 8, 할당 상태인 프롤로그 헤더 */
    mm_p += HDR_SIZE;
    SET(mm_p, SET_METADATA(METADATA_SIZE, ALLOCATED));

    /* 프롤로그 푸터 */
    mm_p += FTR_SIZE;
    SET(mm_p, SET_METADATA(METADATA_SIZE, ALLOCATED));

    /* 크기 0, 할당 상태인 에필로그 헤더 */
    mm_p += HDR_SIZE;
    SET(mm_p, SET_METADATA(0, ALLOCATED));

    /* 4 KiB를 추가해 첫 가용 블록 생성 */
    if (extend_heap(CHUNK_SIZE) == NULL)
        return -1;

    /* 탐색 시작점을 첫 일반 블록의 payload로 이동 */
    mm_p += HDR_SIZE;
    return 0;
}

/*
 * mm_malloc - size바이트의 payload를 담을 블록을 할당합니다.
 * first-fit으로 가용 블록을 찾고, 없으면 힙을 확장합니다.
 * size가 0이거나 힙 확장에 실패하면 NULL을 반환합니다.
 */
void *mm_malloc(size_t size)
{
    size_t asize;
    size_t extend_size;

    char *b_ptr;

    /* 의미 없는 할당/확보 불가능한 크기 할당 요청을 거절 */
    if (size == 0 || size > MAX_SIZE - 16)
        return NULL;

    /* 메타데이터와 정렬을 반영하며, 최소 블록 크기는 16바이트 */
    asize = ADJUST_SIZE(size);

    /* 전체 블록 크기 asize를 수용하는 가용 블록 탐색 */
    b_ptr = find_fit(asize);

    /* 찾은 가용 블록에 배치 */
    if (b_ptr != NULL)
    {
        place(b_ptr, asize);
        return b_ptr;
    }
    /* 맞는 블록이 없으면 힙 확장 */
    else
    {
        /* 필요한 블록 크기와 4 KiB 중 큰 값만큼 확장 */
        extend_size = MAX(asize, CHUNK_SIZE);

        /* 힙 확장 실패 */
        if ((b_ptr = extend_heap(extend_size)) == NULL)
            return NULL;

        /* 확장과 병합으로 확보한 가용 블록에 배치 */
        place(b_ptr, asize);
        return b_ptr;
    }
}

/*
 * find_fit - asize 이상인 첫 가용 블록의 payload 주소를 반환합니다.
 * 크기 0인 에필로그까지 탐색하며, 적합한 블록이 없으면 NULL을 반환합니다.
 */
static void *find_fit(size_t asize)
{
    char *cur = mm_p;
    while (GET_SIZE(HDR(cur)))
    {
        /* 할당되지 않았고 크기가 충분한 첫 블록 선택 */
        if (!GET_IS_ALLOC(HDR(cur)) && asize <= GET_SIZE(HDR(cur)))
            return cur;
        cur = NEXT_BLK(cur);
    }

    /* 에필로그까지 적합한 블록이 없음 */
    return NULL;
}

/*
 * place - 선택한 가용 블록에 asize 크기의 할당 블록을 배치합니다.
 * 나머지가 16바이트 이상이면 분할하고, 작으면 원래 블록 전체를 할당합니다.
 * b_ptr는 크기가 asize 이상인 가용 블록의 payload 주소여야 합니다.
 */
static void place(void *b_ptr, size_t asize)
{
    size_t original_size = GET_SIZE(HDR(b_ptr));
    size_t splited_size = original_size - asize;

    /* 나머지를 독립된 일반 블록으로 만들 수 없으면 전체를 할당 */
    if (splited_size < 16)
    {
        /* 다음 블록의 위치를 보존하도록 원래 크기 유지 */
        SET(HDR(b_ptr), SET_METADATA(original_size, ALLOCATED));
        SET(FTR(b_ptr), SET_METADATA(original_size, ALLOCATED));
    }
    /* 할당 블록과 나머지 가용 블록으로 분할 */
    else
    {
        /* 앞 헤더를 갱신한 뒤, 새 크기로 푸터 위치 계산 */
        SET(HDR(b_ptr), SET_METADATA(asize, ALLOCATED));
        SET(FTR(b_ptr), SET_METADATA(asize, ALLOCATED));

        /* 변경된 앞 블록 크기를 기준으로 나머지 헤더와 푸터 기록 */
        SET(HDR(NEXT_BLK(b_ptr)), SET_METADATA(splited_size, FREED));
        SET(FTR(NEXT_BLK(b_ptr)), SET_METADATA(splited_size, FREED));
    }
}

/*
 * mm_realloc - 블록의 payload 크기를 변경합니다.
 * 새 블록에 기존 payload 용량과 새 요청 크기 중 작은 만큼 복사하고 기존 블록을 해제합니다.
 * b_ptr가 NULL이면 mm_malloc(size)를 호출하고, size가 0이면 기존 블록을 해제합니다.
 * 새 할당에 실패하면 기존 블록과 데이터를 유지하고 NULL을 반환합니다.
 *
 * TODO: 축소나 제자리 확장도 새 할당/복사를 수행합니다. 필요하면 제자리 재할당으로 개선합니다.
 */
void *mm_realloc(void *b_ptr, size_t size)
{
    /* 기존 포인터가 NULL일시 */
    if (b_ptr == NULL)
        return mm_malloc(size);

    /* 새 할당 크기가 0일 시 */
    if (!size)
    {
        mm_free(b_ptr);
        return NULL;
    }

    /* 확보 불가능한 크기 할당 요청을 거절 */
    if (size > MAX_SIZE - 16)
        return NULL;

    size_t original_size = GET_SIZE(HDR(b_ptr));
    size_t realloc_size = ADJUST_SIZE(size);

    /* 새 할당 크기가 기존 payload 용량보다 작을 때 (제자리 축소) */
    if (realloc_size <= original_size)
    {
        /* 분할 후 남는 공간이 16바이트 이상이면 분할*/
        if (original_size - realloc_size >= 16)
        {
            /* 원본 블록 헤더/푸터 수정 */
            SET(HDR(b_ptr), SET_METADATA(realloc_size, ALLOCATED));
            SET(FTR(b_ptr), SET_METADATA(realloc_size, ALLOCATED));

            /* 다음 블록 (가용 블록) 헤더/푸터 수정 */
            SET(NEXT_HDR(b_ptr), SET_METADATA(original_size - realloc_size, FREED));
            SET(NEXT_FTR(b_ptr), SET_METADATA(original_size - realloc_size, FREED));

            /* 분할 될 블록 병합 시도 */
            coalesce(NEXT_BLK(b_ptr));

            return b_ptr;
        }
        /* 16바이트 미만일시 분할하지 않고 할당 */
        else
            return b_ptr;
    }
    /* 새 할당 크기가 기존 payload 용량보다 클 때 (제자리 확장/할당 후 복사)*/
    else if (realloc_size > original_size)
    {
        /* TODO: 주변이 가용 블록이고, 현재 블록 크기와 합쳤을 때 충분하다면 제자리 확장 */
        size_t rblock_size = GET_SIZE(NEXT_HDR(b_ptr));
        /* 다음 블록만 가용 상태: 오른쪽 블록 크기와 합쳐 비교 */
        if (!GET_IS_ALLOC(NEXT_HDR(b_ptr)) && realloc_size <= original_size + rblock_size)
        {
            /* 나머지를 독립된 일반 블록으로 만들 수 없으면 전체를 할당 */
            if (original_size + rblock_size - realloc_size < 16)
            {
                SET(HDR(b_ptr), SET_METADATA(original_size + rblock_size, ALLOCATED));
                SET(FTR(b_ptr), SET_METADATA(original_size + rblock_size, ALLOCATED));
            }
            /* 할당 블록과 나머지 가용 블록으로 분할 */
            else
            {
                /* 앞 헤더를 갱신한 뒤, 새 크기로 푸터 위치 계산 */
                SET(HDR(b_ptr), SET_METADATA(realloc_size, ALLOCATED));
                SET(FTR(b_ptr), SET_METADATA(realloc_size, ALLOCATED));

                /* 변경된 앞 블록 크기를 기준으로 나머지 헤더와 푸터 기록 */
                SET(HDR(NEXT_BLK(b_ptr)), SET_METADATA(original_size + rblock_size - realloc_size, FREED));
                SET(FTR(NEXT_BLK(b_ptr)), SET_METADATA(original_size + rblock_size - realloc_size, FREED));

                coalesce(NEXT_BLK(b_ptr));
            }
            return b_ptr;
        }
        /* TODO: 이전 블록만 가용 상태: 왼쪽 블록 크기와 합쳐 비교 */

        /* TODO: 양쪽 모두 가용 상태: 세 블록 크기를 합쳐 비교 */

        /* 제자리 할당 불가 시 새 블록 할당 시도*/
        void *new_ptr = mm_malloc(size);
        if (new_ptr == NULL)
            return NULL;

        /* 기존 payload 용량과 새 요청 크기 중 작은 만큼 복사 */
        size_t copy_size = MIN(original_size - METADATA_SIZE, size);
        memcpy(new_ptr, b_ptr, copy_size);

        /* 기존 블록 반환 */
        mm_free(b_ptr);

        return new_ptr;
    }
    /* 새 할당 크기가 기존 payload 용량과 같을 때 */
    else
        return b_ptr;
}

/*
 * mm_free - 할당 블록을 가용 상태로 바꾸고 인접 가용 블록과 병합합니다.
 * b_ptr는 유효한 할당 블록의 payload 주소여야 합니다.
 */
void mm_free(void *b_ptr)
{
    /* 잘못된 포인터 검사 */
    if (b_ptr == NULL)
        return;
    /* 전체 블록 크기 확인 */
    size_t asize = GET_SIZE(HDR(b_ptr));

    /* 블록 크기를 유지하고 할당 비트만 가용 상태로 변경 */
    SET(HDR(b_ptr), SET_METADATA(asize, FREED));
    SET(FTR(b_ptr), SET_METADATA(asize, FREED));

    /* 인접 가용 블록과 즉시 병합 */
    coalesce(b_ptr);
}

/*
 * extend_heap - asize바이트만큼 힙을 확장하고 가용 블록을 만듭니다.
 * asize는 16바이트 이상인 8의 배수여야 하며, 함수에서 크기를 올림하지 않습니다.
 * 병합 결과의 payload 주소를 반환하며, 조건 불만족 또는 확장 실패 시 NULL입니다.
 *
 * TODO: 힙의 끝에 가용 블록이 있는지 확인하고, 필요한 크기를 계산하여 효율적으로 확장하는 기능 구현
 */
static void *extend_heap(size_t asize)
{
    /* 확장량의 정렬, 최소 블록 크기 */
    if (asize % 8 || asize < 16)
        return NULL;

    /* 타입 변환 검사 */
    if (asize > (size_t)INT_MAX)
        return NULL;

    /* 힙을 확장하고 이전 brk를 새 블록의 payload 주소로 사용 */
    char *b_ptr = mem_sbrk(asize);
    if (b_ptr == (void *)-1)
        return NULL;

    /* 기존 에필로그 헤더를 가용 블록 헤더로 덮어쓰고 새 푸터 기록 */
    SET(HDR(b_ptr), SET_METADATA(asize, FREED));
    SET(FTR(b_ptr), SET_METADATA(asize, FREED));

    /* 확장된 힙 끝에 새 에필로그 헤더 기록 */
    SET(HDR(NEXT_BLK(b_ptr)), SET_METADATA(0, ALLOCATED));

    /* 이전 블록이 가용 상태라면 병합 */
    return coalesce(b_ptr);
}

/*
 * coalesce - 현재 가용 블록을 인접한 가용 블록과 병합합니다.
 * 이전 블록의 푸터와 다음 블록의 헤더로 할당 상태를 확인합니다.
 * 병합 결과 블록의 payload 시작 주소를 반환합니다.
 */
static void *coalesce(void *b_ptr)
{
    /* 이웃 블록의 할당 상태와 현재 블록의 전체 크기 확인 */
    size_t prev_alloc = GET_IS_ALLOC(PREV_FTR(b_ptr));
    size_t next_alloc = GET_IS_ALLOC(NEXT_HDR(b_ptr));
    size_t size = GET_SIZE(HDR(b_ptr));

    /* 이전과 다음이 모두 할당 상태: 병합하지 않음 */
    if (prev_alloc && next_alloc)
    {
        return b_ptr;
    }
    /* 다음 블록만 가용 상태: 오른쪽으로 병합 */
    else if (prev_alloc && !next_alloc)
    {
        size += GET_SIZE(HDR(NEXT_BLK(b_ptr)));
        SET(HDR(b_ptr), SET_METADATA(size, FREED));
        SET(FTR(b_ptr), SET_METADATA(size, FREED));
    }
    /* 이전 블록만 가용 상태: 왼쪽으로 병합 */
    else if (!prev_alloc && next_alloc)
    {
        size += GET_SIZE(FTR(PREV_BLK(b_ptr)));

        SET(FTR(b_ptr), SET_METADATA(size, FREED));
        SET(HDR(PREV_BLK(b_ptr)), SET_METADATA(size, FREED));

        b_ptr = PREV_BLK(b_ptr);
    }
    /* 양쪽 모두 가용 상태: 세 블록을 병합 */
    else
    {
        size += GET_SIZE(HDR(PREV_BLK(b_ptr))) + GET_SIZE(FTR(NEXT_BLK(b_ptr)));

        SET(HDR(PREV_BLK(b_ptr)), SET_METADATA(size, FREED));
        SET(FTR(NEXT_BLK(b_ptr)), SET_METADATA(size, FREED));

        b_ptr = PREV_BLK(b_ptr);
    }
    return b_ptr;
}
