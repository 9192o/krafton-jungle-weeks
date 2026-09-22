#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARENA_SIZE 4096
static unsigned char arena[ARENA_SIZE];
static size_t arena_off = 0;

// TODO: 걍 나중에는 동적으로 할당하게 바꾸는 게 맞는 듯
// 지금은 걍 임시 땜빵
static void *arena_alloc(size_t n)
{
    if (arena_off + n > ARENA_SIZE)
        return NULL;
    void *p = &arena[arena_off];
    arena_off += n; // n만큼 아레나 할당
    return p;
}

static char *intern(const char *s)
{
    size_t n = strlen(s) + 1;
    char *dst = arena_alloc(n);
    if (dst == NULL)
        return NULL;
    memcpy(dst, s, n);
    return dst;
}

int main(void)
{
    const char *words[] = {
        "insert",
        "delete",
        "search",
        "traverse",
        "balance",
        "rotate",
        "rehash",
        "compact",
        "serialize",
        "checkpoint",
    };
    int nwords = (int)(sizeof(words) / sizeof(words[0]));

    char *last = NULL;
    long total = 0;
    for (int i = 0; i < 100000; i++) // 10만번 작동
    {
        char buf[32];
        snprintf(buf, sizeof buf, "%s-%d", words[i % nwords], i); // words 배열 안에 들어있는 애들을 돌려가면서 버퍼에 작성
        last = intern(buf);                                       // 인턴십
        if (last == NULL)
            break;
        total += (long)strlen(last);
    }

    printf("interned, last=%s total_len=%ld\n", last, total);
    return 0;
}

/* [Thinking Point]
 * 이 arena[] 는 함수 밖에 선언된 '전역 변수'다. 만약 이걸 arena_alloc() 함수 '안'의
 * 지역 변수로 옮기면 무슨 차이가 생길까?
 *   tip 1. 저장 위치가 다르다. 전역/static 은 프로그램 내내 사는 .bss/.data 영역에,
 *          지역 변수는 함수가 실행되는 동안만 사는 '스택'에 놓인다.
 *   tip 2. 수명이 다르다. 전역은 프로그램 시작~끝까지 유지되지만, 지역은 함수가 return
 *          하면 사라진다. → 그 주소를 함수 밖으로 돌려주면 7번(stack use-after-return)!
 *   tip 3. 초기화가 다르다. 전역/static 은 자동으로 0 으로 초기화되지만(그래서 .bss),
 *          지역 변수는 초기화하지 않으면 쓰레기 값이다(8번 챌린지).
 *   생각해보기: 여러 번 호출돼도 같은 저장소를 계속 나눠 쓰려면(커서 arena_off 유지)
 *               이 버퍼는 왜 전역(또는 static)이어야 할까? */