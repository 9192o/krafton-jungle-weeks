//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section A - Linked List Questions
Purpose: Implementing the required functions for Question 1 */

//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _listnode
{
	int item;
	struct _listnode *next;
} ListNode; // You should not change the definition of ListNode

typedef struct _linkedlist
{
	int size;
	ListNode *head;
} LinkedList; // You should not change the definition of LinkedList

///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototype of this function
int insertSortedLL(LinkedList *ll, int item);

void printList(LinkedList *ll);
void removeAllItems(LinkedList *ll);
ListNode *findNode(LinkedList *ll, int index);
int insertNode(LinkedList *ll, int index, int value);
int removeNode(LinkedList *ll, int index);

//////////////////////////// main() //////////////////////////////////////////////

int main()
{
	LinkedList ll;
	int c, i, j;
	c = 1;

	// Initialize the linked list 1 as an empty linked list
	ll.head = NULL;
	ll.size = 0;

	printf("1: Insert an integer to the sorted linked list:\n");
	printf("2: Print the index of the most recent input value:\n");
	printf("3: Print sorted linked list:\n");
	printf("0: Quit:");

	while (c != 0)
	{
		printf("\nPlease input your choice(1/2/3/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to add to the linked list: ");
			scanf("%d", &i);
			j = insertSortedLL(&ll, i);
			printf("The resulting linked list is: ");
			printList(&ll);
			break;
		case 2:
			printf("The value %d was added at index %d\n", i, j);
			break;
		case 3:
			printf("The resulting sorted linked list is: ");
			printList(&ll);
			removeAllItems(&ll);
			break;
		case 0:
			removeAllItems(&ll);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

int insertSortedLL(LinkedList *ll, int item)
{
	// 조건:
	// 1. 새 항목이 추가된 인덱스 위치를 반환해야 한다.
	// 2. 함수 실행이 실패한 경우 -1 반환해야 한다.

	// 만약 현재 Linked List가 다음과 같고,
	// [2, 3, 5, 7, 9]
	// 여기서 8을 추가해야 한다면 결과는 다음과 같다.
	// [2, 3, 5, 7, 8, 9]
	// 또한 반환값은 삽입된 인덱스, 4를 출력해야한다.

	// 만약 현재 Linked List가 다음과 같고,
	// [5, 7, 9, 11, 15]
	// 여기서 7을 추가한다면 결과는 다음과 같다.
	// [5, 7, 9, 11, 15]
	// 반환값은 -1

	/* add your code here */
	ListNode *cur, *temp;

	// 1. List 자체가 비어있을 때
	if (ll == NULL)
	{
		return -1;
	}

	// List head가 비어있을 때 (첫 번째 Push)
	int index = 0;
	if (ll->head == NULL)
	{
		ListNode *node = malloc(sizeof(ListNode));
		node->item = item;
		node->next = NULL;
		ll->head = node;
		ll->size++;
		return index;
	}

	// List가 비어있지 않을 때 (정렬 Push)
	// Current Node의 item과, 현재 인수로 들어온 item과 비교하여
	// 노드를 삽입한다.

	// TODO: 아래에 함수 포인터 findNode가 있던데
	if (ll->head != NULL)
	{
		cur = ll->head;
		while (cur != NULL)
		{
			// Linked List 안에 중복 값이 있으면 return -1
			if (cur->item == item)
				return -1;

			// 조건은 세 가지.
			// 1. head가 더 클때. 그러니까 인덱스 0에 해당하는 노드가 더 클때.
			if (cur->item > item && index == 0)
			{
				ListNode *node = malloc(sizeof(ListNode));
				node->item = item;
				node->next = cur;
				ll->head = node;
				return index;
			}

			// 2. cur->next가 없고, cur->item < item
			if (cur->next == NULL && cur->item < item)
			{
				ListNode *node = malloc(sizeof(ListNode));
				node->item = item;
				node->next = NULL;

				cur->next = node;
				return index;
			}

			// 3. cur->next가 있고, cur->item < item < cur->next->item
			if (cur->item < item && cur->next->item > item)
			{
				ListNode *node = malloc(sizeof(ListNode));

				temp = cur->next;
				node->item = item;
				node->next = temp;

				cur->next = node;
				return index;
			}
			cur = cur->next;
			index++;
		}
	}
	return -1;
}

///////////////////////////////////////////////////////////////////////////////////

void printList(LinkedList *ll)
{

	ListNode *cur;
	if (ll == NULL)
		return;
	cur = ll->head;

	if (cur == NULL)
		printf("Empty");
	while (cur != NULL)
	{
		printf("%d ", cur->item);
		cur = cur->next;
	}
	printf("\n");
}

void removeAllItems(LinkedList *ll)
{
	ListNode *cur = ll->head;
	ListNode *tmp;

	while (cur != NULL)
	{
		tmp = cur->next;
		free(cur);
		cur = tmp;
	}
	ll->head = NULL;
	ll->size = 0;
}

ListNode *findNode(LinkedList *ll, int index)
{

	ListNode *temp;

	if (ll == NULL || index < 0 || index >= ll->size)
		return NULL;

	temp = ll->head;

	if (temp == NULL || index < 0)
		return NULL;

	while (index > 0)
	{
		temp = temp->next;
		if (temp == NULL)
			return NULL;
		index--;
	}

	return temp;
}

int insertNode(LinkedList *ll, int index, int value)
{

	ListNode *pre, *cur;

	if (ll == NULL || index < 0 || index > ll->size + 1)
		return -1;

	// If empty list or inserting first node, need to update head pointer
	if (ll->head == NULL || index == 0)
	{
		cur = ll->head;
		ll->head = malloc(sizeof(ListNode));
		ll->head->item = value;
		ll->head->next = cur;
		ll->size++;
		return 0;
	}

	// Find the nodes before and at the target position
	// Create a new node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL)
	{
		cur = pre->next;
		pre->next = malloc(sizeof(ListNode));
		pre->next->item = value;
		pre->next->next = cur;
		ll->size++;
		return 0;
	}

	return -1;
}

int removeNode(LinkedList *ll, int index)
{

	ListNode *pre, *cur;

	// Highest index we can remove is size-1
	if (ll == NULL || index < 0 || index >= ll->size)
		return -1;

	// If removing first node, need to update head pointer
	if (index == 0)
	{
		cur = ll->head->next;
		free(ll->head);
		ll->head = cur;
		ll->size--;

		return 0;
	}

	// Find the nodes before and after the target position
	// Free the target node and reconnect the links
	if ((pre = findNode(ll, index - 1)) != NULL)
	{

		if (pre->next == NULL)
			return -1;

		cur = pre->next;
		pre->next = cur->next;
		free(cur);
		ll->size--;
		return 0;
	}

	return -1;
}
