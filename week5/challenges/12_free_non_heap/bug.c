#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FIELDS 8
typedef struct
{
    char *base;
    char *fields[MAX_FIELDS];
    int n;
} Row;

static void parse_row(Row *r, const char *csv)
{
    r->base = strdup(csv); // 내부적으로 메모리 할당 후 문자열 복사
    if (!r->base)
    {
        perror("strdup");
        exit(1);
    }
    r->n = 0;

    // 메모리 공간에 복사된 문자열, "," 기준으로 Tokenize
    for (char *tok = strtok(r->base, ","); tok && r->n < MAX_FIELDS;
         tok = strtok(NULL, ","))
    {
        r->fields[r->n++] = strdup(tok); // 아예 토큰들도 따로 메모리 갖게 함.
    }
}

static void row_print(const Row *r)
{
    printf("%d fields:", r->n);
    for (int i = 0; i < r->n; i++)
        printf(" [%s]", r->fields[i]);
    printf("\n");
}

static void row_free(Row *r)
{
    for (int i = 0; i < r->n; i++)
    {
        free(r->fields[i]);
    }
    free(r->base); // base 메모리 공간도 free
    r->n = 0;
}

int main(void)
{
    Row r;
    parse_row(&r, "id,name,dept,salary");
    row_print(&r);

    row_free(&r);
    printf("done\n");
    return 0;
}

/* [Thinking Point]
 * _POSIX_C_SOURCE 는 "이 소스가 어느 POSIX 표준 버전까지의 API 를 쓰겠다"고 미리
 * 선언하는 '기능 시험 매크로(feature test macro)'다. 값 200809L 은 POSIX.1-2008 을 뜻한다.
 *   tip 1. 반드시 <string.h> 등 헤더를 include 하기 '전에' 정의해야 효력이 있다.
 *          (헤더가 이 값을 보고 어떤 함수 선언을 노출할지 결정하기 때문)
 *   tip 2. strdup 은 C 표준(C11)에는 없고 POSIX 에 있는 함수다. -std=c11 로 엄격히
 *          컴파일하면 이 매크로가 없을 경우 strdup 선언이 감춰져 '암시적 선언' 경고가 나고,
 *          반환값이 int 로 잘못 취급돼 포인터가 깨지는 별도 버그로 이어질 수 있다.
 *   생각해보기 1 (POSIX 란?): POSIX 는 유닉스 계열 OS 가 공통으로 제공하기로 약속한
 *               '운영체제 인터페이스 표준'이다(파일·프로세스·스레드·문자열 등의 API 규격).
 *               리눅스·macOS 등이 이를 따르므로, POSIX 함수를 쓰면 여러 OS 에서 같은
 *               코드가 동작한다. 그런데 왜 C 표준(C11)과 POSIX 를 굳이 구분할까?
 *   생각해보기 2 (버전 관리 관점): 왜 "쓸 수 있는 표준 버전"을 코드가 스스로 선언하게 할까?
 *               (숫자 200809L = 표준의 '연-월' 버전. 값이 클수록 더 최신 표준) */