//////////////////////////////////////////////////////////////////////////////////

/* CE1007/CZ1007 Data Structures
Lab Test: Section F - Binary Search Trees Questions
Purpose: Implementing the required functions for Question 5
		 Implementing 'remove node' operation for BST*/
//////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////////////

typedef struct _bstnode
{
	int item;
	struct _bstnode *left;
	struct _bstnode *right;
} BSTNode; // You should not change the definition of BSTNode

typedef struct _stackNode
{
	BSTNode *data;
	struct _stackNode *next;
} StackNode; // You should not change the definition of StackNode

typedef struct _stack
{
	StackNode *top;
} Stack; // You should not change the definition of Stack

///////////////////////// function prototypes ////////////////////////////////////

// You should not change the prototypes of these functions
void postOrderIterativeS2(BSTNode *root);

void insertBSTNode(BSTNode **node, int value);

void push(Stack *stack, BSTNode *node);
BSTNode *pop(Stack *s);
BSTNode *peek(Stack *s);
int isEmpty(Stack *s);
void removeAll(BSTNode **node);
BSTNode *removeNodeFromTree(BSTNode *root, int value);
int findSmallestValue(BSTNode *root);
///////////////////////////// main() /////////////////////////////////////////////

int main()
{
	int c, i;
	c = 1;

	// Initialize the Binary Search Tree as an empty Binary Search Tree
	BSTNode *root;
	root = NULL;

	printf("1: Insert an integer into the binary search tree;\n");
	printf("2: Print the post-order traversal of the binary search tree;\n");
	printf("0: Quit;\n");

	while (c != 0)
	{
		printf("Please input your choice(1/2/0): ");
		scanf("%d", &c);

		switch (c)
		{
		case 1:
			printf("Input an integer that you want to insert into the Binary Search Tree: ");
			scanf("%d", &i);
			insertBSTNode(&root, i);
			break;
		case 2:
			printf("The resulting post-order traversal of the binary search tree is: ");
			postOrderIterativeS2(root); // You need to code this function
			printf("\n");
			break;
		case 0:
			removeAll(&root);
			break;
		default:
			printf("Choice unknown;\n");
			break;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////////////

void postOrderIterativeS2(BSTNode *root)
{
	/* add your code here */
	// Initialize Stack 1, 2
	Stack s1, s2;

	s1.top = NULL;
	s2.top = NULL;

	push(&s1, root);
	while (!isEmpty(&s1))
	{
		// 1번 스택에서 Pop
		BSTNode *temp = pop(&s1);

		// 1번 스택에 양쪽 노드를 Push
		if (temp->left)
			push(&s1, temp->left);
		if (temp->right)
			push(&s1, temp->right);

		// 2번 스택에 Pop 한 노드 Push
		push(&s2, temp);
	}

	// 2번 스택에 다 저장되어있으니, 그대로 출력
	while (!isEmpty(&s2))
		printf("%d ", pop(&s2)->item);
}

/* Given a binary search tree and a key, this function
   deletes the key and returns the new root. Make recursive function. */
BSTNode *removeNodeFromTree(BSTNode *root, int value)
{
	/* add your code here */
	BSTNode *cur = root;

	// 0. 삭제하려는 노드를 탐색
	if (cur == NULL)
		return NULL;

	if (cur->item == value)
	{
		// 1. 삭제하려는 노드의 자식 개수가 0개일 경우
		if (cur->left == NULL && cur->right == NULL)
		{
			free(cur);
			return NULL;
		}
		// 2. 삭제하려는 노드의 자식 개수가 1개일 경우
		else if ((cur->left != NULL && cur->right == NULL) || (cur->left == NULL && cur->right != NULL))
		{
			BSTNode *temp = cur->left != NULL ? cur->left : cur->right;
			free(cur);
			return temp;
		}

		// 3. 삭제하려는 노드의 자식 개수가 2개일 경우
		else
		{
			// 가장 작은 값을 가진 노드를 탐색
			int smallest = findSmallestValue(cur->right);
			// 현재 노드를 가장 작은 값으로 교체
			cur->item = smallest;
			// 오른쪽으로 넘어감.
			cur->right = removeNodeFromTree(cur->right, smallest);
			return cur;
		}
	}
	if (value < cur->item)
	{
		cur->left = removeNodeFromTree(cur->left, value);
		return cur;
	}
	cur->right = removeNodeFromTree(cur->right, value);
	return cur;
}

int findSmallestValue(BSTNode *root)
{
	// 최솟값을 반환하는 헬퍼
	// 어차피 왼쪽으로 가야함 (BT)
	return root->left == NULL ? root->item : findSmallestValue(root->left);
}
///////////////////////////////////////////////////////////////////////////////

void insertBSTNode(BSTNode **node, int value)
{
	if (*node == NULL)
	{
		*node = malloc(sizeof(BSTNode));

		if (*node != NULL)
		{
			(*node)->item = value;
			(*node)->left = NULL;
			(*node)->right = NULL;
		}
	}
	else
	{
		if (value < (*node)->item)
		{
			insertBSTNode(&((*node)->left), value);
		}
		else if (value > (*node)->item)
		{
			insertBSTNode(&((*node)->right), value);
		}
		else
			return;
	}
}

//////////////////////////////////////////////////////////////////////////////////

void push(Stack *stack, BSTNode *node)
{
	StackNode *temp;

	temp = malloc(sizeof(StackNode));

	if (temp == NULL)
		return;
	temp->data = node;

	if (stack->top == NULL)
	{
		stack->top = temp;
		temp->next = NULL;
	}
	else
	{
		temp->next = stack->top;
		stack->top = temp;
	}
}

BSTNode *pop(Stack *s)
{
	StackNode *temp, *t;
	BSTNode *ptr;
	ptr = NULL;

	t = s->top;
	if (t != NULL)
	{
		temp = t->next;
		ptr = t->data;

		s->top = temp;
		free(t);
		t = NULL;
	}

	return ptr;
}

BSTNode *peek(Stack *s)
{
	StackNode *temp;
	temp = s->top;
	if (temp != NULL)
		return temp->data;
	else
		return NULL;
}

int isEmpty(Stack *s)
{
	if (s->top == NULL)
		return 1;
	else
		return 0;
}

void removeAll(BSTNode **node)
{
	if (*node != NULL)
	{
		removeAll(&((*node)->left));
		removeAll(&((*node)->right));
		free(*node);
		*node = NULL;
	}
}
