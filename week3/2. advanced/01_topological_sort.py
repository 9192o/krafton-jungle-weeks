"""
[위상 정렬 - Topological Sort]

문제 설명:
- 방향 그래프에서 순서를 정합니다.
- 선행 작업이 먼저 오도록 정렬합니다.
- 예: 과목 선수과목, 작업 순서

입력:
- graph: 방향 그래프
- vertices: 정점 개수

출력:
- 위상 정렬 순서

예제:
과목:
0(기초) → 1(중급) → 3(고급)
0(기초) → 2(응용)

위상 정렬: [0, 1, 2, 3] 또는 [0, 2, 1, 3]

힌트:
- 진입 차수(in-degree) 사용
- 진입 차수가 0인 정점부터 시작
- 큐 사용
"""

from collections import deque


def topological_sort(vertices, edges):
    """
    위상 정렬 (Kahn's Algorithm)

    Args:
        vertices: 정점 개수
        edges: (출발, 도착) 간선 리스트

    Returns:
        위상 정렬 순서
    """
    # indegree와 outdegree들을 저장하기 위함.
    not_inbound = [i for i in range(vertices)]
    outbound = {}
    result = []

    # (출발, 도착) 간선 리스트들 중에
    for s, e in edges:
        # inbound가 있는 애들은 제외하고 없는 애들만 남긴다.
        if e in not_inbound:
            not_inbound.remove(e)
        # outbound들은 따로 매핑해준다. {s: [e, ]}
        if outbound.get(s) is None:
            outbound[s] = [
                e,
            ]
        else:
            outbound[s].extend([e])

    # outbound가 없는 애들을 먼저 Queue에 push.
    q = deque(not_inbound)

    # Queue가 빌때까지
    while len(q):
        node = q.popleft()
        # 하나 뽑아서 노드를 넣는다.
        result.append(node)

        # 그리고 그 노드들의 outbound를, result에 없으면 넣고 있으면 넣지 않는다.
        if outbound.get(node) is not None:
            for n in outbound[node]:
                if n not in result:
                    q.extend([n])
    return result


# 테스트 케이스
if __name__ == "__main__":
    # 과목 선수과목 예제
    vertices = 4
    edges = [
        (0, 1),  # 0 → 1
        (0, 2),  # 0 → 2
        (1, 3),  # 1 → 3
    ]

    print("=== 위상 정렬 ===")
    print("과목 관계:")
    print("  0(기초) → 1(중급) → 3(고급)")
    print("  0(기초) → 2(응용)")
    print()

    result = topological_sort(vertices, edges)
    print(f"수강 순서: {result}")
