"""
[그리디 - 회의실 배정]

문제 설명:
- 하나의 회의실에 여러 회의를 배정합니다.
- 각 회의는 시작 시간과 종료 시간이 있습니다.
- 최대한 많은 회의를 배정하려고 합니다.

입력:
- meetings: [(시작, 종료), ...] 회의 리스트

출력:
- 배정된 회의 개수

예제:
입력: [(1, 4), (3, 5), (0, 6), (5, 7), (3, 8), (5, 9), (6, 10), (8, 11), (8, 12), (2, 13), (12, 14)]
출력: 4개
선택: [(1, 4), (5, 7), (8, 11), (12, 14)]

힌트:
- 종료 시간이 빠른 회의부터 선택!
- 이전 회의가 끝난 후에 시작하는 회의만 선택
"""


def select_meetings(meetings):
    """
    회의실 배정 (그리디)

    Args:
        meetings: [(시작, 종료)] 리스트

    Returns:
        (배정된 회의 개수, 선택된 회의 리스트)
    """
    selected = []
    if meetings is None:
        return 0, []
    if len(meetings) == 1:
        return 1, meetings

    # 음, 종료 시간이 빠른 애들을 기준으로 정렬하자.
    meetings.sort(key=lambda t: t[1])

    # 맨 처음 하나를 고름
    selected.append(meetings[0])

    # 그 다음 종료 시간을 봄
    # 근데, 시작 시간이, 지금 selected에 있는 종료 시간보다 더 커야함.
    tail = 0
    for i in range(1, len(meetings)):
        _, e = selected[tail]
        sm, _ = meetings[i]
        if e <= sm:
            selected.append(meetings[i])
            tail += 1

    return len(selected), selected


# 테스트 케이스
if __name__ == "__main__":
    # 테스트 케이스 1
    meetings1 = [(1, 4), (3, 5), (0, 6), (5, 7), (3, 8), (5, 9)]
    count1, selected1 = select_meetings(meetings1)
    print("=== 테스트 케이스 1 ===")
    print(f"전체 회의: {meetings1}")
    print(f"배정된 회의 개수: {count1}개")
    print(f"선택된 회의: {selected1}")
    print()

    # 테스트 케이스 2
    meetings2 = [
        (1, 4),
        (3, 5),
        (0, 6),
        (5, 7),
        (3, 8),
        (5, 9),
        (6, 10),
        (8, 11),
        (8, 12),
        (2, 13),
        (12, 14),
    ]
    count2, selected2 = select_meetings(meetings2)
    print("=== 테스트 케이스 2 ===")
    print(f"전체 회의: {len(meetings2)}개")
    print(f"배정된 회의 개수: {count2}개")
    print(f"선택된 회의: {selected2}")
