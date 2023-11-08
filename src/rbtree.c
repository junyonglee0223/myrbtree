#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

rbtree *new_rbtree(void) {
	rbtree* p = (rbtree*)calloc(1, sizeof(rbtree));
	if (p == NULL) {
		return NULL;
	}

	node_t* nil_node = (node_t*)calloc(1, sizeof(node_t));
	if (nil_node == NULL) {
		free(p); // 이미 할당된 rbtree 구조체 메모리 해제
		return NULL;
	}
	nil_node->color = RBTREE_BLACK;
	nil_node->left = nil_node;			//nil_node가 아닌 NULL로 설정  
	nil_node->right = nil_node;
	nil_node->parent = NULL;

	p->nil = nil_node;
	p->root = p->nil;

	return p;
}

void delete_rbtree(rbtree *t) {
	postorder_delete(t, t->root);
	free(t->nil);
  	free(t);
}


//근데 사실 erase 함수를 사용해도 되지 않음??
void postorder_delete(rbtree* t, node_t* now) {
	if(now == t->nil)return;

	if (now->left != t->nil)
		postorder_delete(t, now->left);
	if (now->right != t->nil)
		postorder_delete(t, now->right);
	free(now);
	return;
}


void rotate_left(rbtree* t, node_t* node) {
    node_t* parent = node->parent;
    node_t* grand_parent = parent->parent;
    node_t* node_left = node->left;

    if (parent == t->root)
        t->root = node;
    else {
        if (grand_parent->left == parent)
            grand_parent->left = node;
        else
            grand_parent->right = node;
    }
    node->parent = grand_parent;
    parent->parent = node;
    node->left = parent;
    parent->right = node_left;
    if (node_left != t->nil) {
        node_left->parent = parent;
    }
}

void rotate_right(rbtree* t, node_t* node) {
    node_t* parent = node->parent;
    node_t* grand_parent = parent->parent;
    node_t* node_right = node->right;

    if (parent == t->root)
        t->root = node;
    else {
        if (grand_parent->right == parent)
            grand_parent->right = node;
        else
            grand_parent->left = node;
    }
    node->parent = grand_parent;
    parent->parent = node;
    node->right = parent;
    parent->left = node_right;
    if (node_right != t->nil) {
        node_right->parent = parent;
    }
}





// 노드를 삽입하고 불균형을 복구하는 함수
node_t* rbtree_insert(rbtree* t, const key_t key)
{
	// 새 노드 생성
	node_t* new_node = (node_t*)calloc(1, sizeof(node_t));
	new_node->key = key;
	new_node->color = RBTREE_RED;              // 항상 레드로 추가
	new_node->left = new_node->right = t->nil; // 추가한 노드의 자식들을 nil 노드로 설정

	// 새 노드를 삽입할 위치 탐색
	node_t* current = t->root;
	while (current != t->nil)
	{
		if (key < current->key)
		{
			if (current->left == t->nil)
			{
				current->left = new_node; // 새 노드를 왼쪽 자식으로 추가
				break;
			}
			current = current->left;
		}
		else
		{
			if (current->right == t->nil)
			{
				current->right = new_node; // 새 노드를 오른쪽 자식으로 추가
				break;
			}
			current = current->right;
		}
	}

	new_node->parent = current; // 새 노드의 부모 지정

	// root가 nil이면(트리가 비어있으면) 새 노드를 트리의 루트로 지정
	if (current == t->nil)
		t->root = new_node;

	// 불균형 복구
	rbtree_insert_fixup(t, new_node);

	return new_node;
}






// 노드 삽입 후 불균형을 복구하는 함수
void rbtree_insert_fixup(rbtree* t, node_t* node)
{
	node_t* parent = node->parent;
	node_t* grand_parent = parent->parent;
	node_t* uncle;
	int is_left = node == parent->left; // 현재 노드가 왼쪽 자식인지 여부
	int is_parent_is_left;              // 부모가 왼쪽 자식인지 여부

	// 추가된 노드가 root 노드인 경우: 색만 변경
	if (node == t->root)
	{
		node->color = RBTREE_BLACK;
		return;
	}

	// 부모가 BLACK인 경우: 변경 없음
	if (parent->color == RBTREE_BLACK)
		return;

	is_parent_is_left = grand_parent->left == parent;
	uncle = (is_parent_is_left) ? grand_parent->right : grand_parent->left;

	// [CASE 1]: 부모와 부모의 형제가 모두 RED인 경우
	if (uncle->color == RBTREE_RED)
	{
		parent->color = RBTREE_BLACK;
		uncle->color = RBTREE_BLACK;
		grand_parent->color = RBTREE_RED;
		rbtree_insert_fixup(t, grand_parent);
		return;
	}

	if (is_parent_is_left)
	{
		if (is_left)
			// [CASE 2]: 부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 왼쪽 자식인 경우
		{
			rotate_right(t, parent);
			exchange_color(parent, parent->right);
			return;
		}

		// [CASE 3]: 부모의 형제가 BLACK & 부모가 왼쪽 자식 & 현재 노드가 오른쪽 자식인 경우
		rotate_left(t, node);
		rotate_right(t, node);
		exchange_color(node, node->right);
		return;
	}

	if (is_left)
	{
		// [CASE 3]: 부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 왼쪽 자식인 경우
		rotate_right(t, node);
		rotate_left(t, node);
		exchange_color(node, node->left);
		return;
	}

	// [CASE 2]: 부모의 형제가 BLACK & 부모가 오른쪽 자식 & 현재 노드가 오른쪽 자식인 경우
	rotate_left(t, parent);
	exchange_color(parent, parent->left);
}


// key에 해당하는 노드를 반환하는 함수
node_t* rbtree_find(const rbtree* t, const key_t key)
{
    node_t* current = t->root;
    while (current != t->nil)
    {
        if (key == current->key)
            return current;
        else
            current = (key < current->key) ? current->left : current->right;
    }
    return NULL; // 해당 key값을 가진 노드가 없을 경우 NULL 반환
}



node_t *rbtree_min(const rbtree *t) {
	node_t* tmp = t->root;
	while (tmp->left != t->nil) {
		tmp = tmp->left;
	}
	return tmp;
}

node_t *rbtree_max(const rbtree *t) {
	node_t* tmp = t->root;
	while (tmp->right != t->nil) {
		tmp = tmp->right;
	}
	return tmp;
}




void trans_node(rbtree* t, node_t* u, node_t* v) {
	// u가 트리의 루트인 경우, t의 루트를 v로 변경합니다.
	if (u->parent == NULL) {
		t->root = v;
	}
	// u가 부모의 왼쪽 자식인 경우, 부모의 왼쪽 포인터를 v로 변경합니다.
	else if (u == u->parent->left) {
		u->parent->left = v;
	}
	// u가 부모의 오른쪽 자식인 경우, 부모의 오른쪽 포인터를 v로 변경합니다.
	else {
		u->parent->right = v;
	}
	// v가 NULL이 아닌 경우에만 v의 부모 포인터를 업데이트합니다.
	if (v != NULL) {
		v->parent = u->parent;
	}
}




void refix_erase(rbtree* t, node_t* parent, int is_left) {
	
	
	node_t* extra_black = is_left ? parent->left : parent->right;
	if (extra_black->color == RBTREE_RED) {
		extra_black->color = RBTREE_BLACK;
		return;
	}

	node_t* sibling = is_left ? parent->right : parent->left;
	node_t* sibling_left = sibling->left;
	node_t* sibling_right = sibling->right;

	if (sibling->color == RBTREE_RED) {
		if (is_left)
			rotate_left(t, sibling);
		else
			rotate_right(t, sibling);
		exchange_color(sibling, parent);
		refix_erase(t, parent, is_left);
		return;
	}

	node_t* near = is_left ? sibling_left : sibling_right;
	node_t* distant = is_left ? sibling_right : sibling_left;

	if (is_left && near->color == RBTREE_RED && distant->color == RBTREE_BLACK) {
		rotate_right(t, near);
		exchange_color(sibling, near);
		refix_erase(t, parent, is_left);
		return;
	}

	if (is_left && distant->color == RBTREE_RED) {
		rotate_left(t, sibling);
		exchange_color(sibling, parent);
		distant->color = RBTREE_BLACK;
		return;
	}

	if (near->color == RBTREE_RED && distant->color == RBTREE_BLACK) {
		rotate_left(t, near);
		exchange_color(sibling, near);
		refix_erase(t, parent, is_left);
		return;
	}

	if (distant->color == RBTREE_RED) {
		rotate_right(t, sibling);
		exchange_color(sibling, parent);
		distant->color = RBTREE_BLACK;
		return;
	}

	sibling->color = RBTREE_RED;
	
	
	if (parent != t->root)
		refix_erase(t, parent->parent, parent->parent->left == parent);
	
}


int rbtree_erase(rbtree* t, node_t* p) {
	node_t* remove;
	node_t* remove_parent, * replace_node;
	int is_remove_black, is_remove_left;

	if (p->left != t->nil && p->right != t->nil) {
		remove = get_next_node(t, p);
		replace_node = remove->right;
		p->key = remove->key;
	}
	else {
		remove = p;
		replace_node = (remove->right != t->nil) ? remove->right : remove->left;
	}
	remove_parent = remove->parent;

	if (remove == t->root) {
		t->root = replace_node;
		t->root->color = RBTREE_BLACK;
		free(remove);
		return 0;
	}

	is_remove_black = remove->color;
	is_remove_left = remove_parent->left == remove;

	if (is_remove_left)
		remove_parent->left = replace_node;
	else
		remove_parent->right = replace_node;

	replace_node->parent = remove_parent;
	free(remove);

	if (is_remove_black)
		refix_erase(t, remove_parent, is_remove_left);

	
	return 0;
}


void exchange_color(node_t* a, node_t* b)
{
	int tmp = a->color;
	a->color = b->color;
	b->color = (tmp == RBTREE_BLACK) ? RBTREE_BLACK : RBTREE_RED;
}


node_t* get_next_node(const rbtree* t, node_t* p)
{
	node_t* current = p->right;
	if (current == t->nil) // 오른쪽 자식이 없으면
	{
		current = p;
		while (1)
		{
			if (current->parent->right == current) // current가 오른쪽 자식인 경우
				current = current->parent;           // 부모 노드로 이동 후 이어서 탐색
			else
				return current->parent; // current가 왼쪽 자식인 경우 부모 리턴
		}
	}
	while (current->left != t->nil) // 왼쪽 자식이 있으면
		current = current->left;      // 왼쪽 끝으로 이동
	return current;
}

int rbtree_to_array(const rbtree* t, key_t* arr, const size_t n) {
    node_t* current = rbtree_min(t);
    arr[0] = current->key;
    for (int i = 1; i < n; i++) {
        if (current == t->nil)
            break;
        current = get_next_node(t, current);
        if (current == t->nil)
            break;
        arr[i] = current->key;
    }
    return 0;
}



