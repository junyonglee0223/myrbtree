#ifndef _RBTREE_H_
#define _RBTREE_H_

#include <stddef.h>

typedef enum { RBTREE_RED, RBTREE_BLACK } color_t;

typedef int key_t;

typedef struct node_t {
  color_t color;
  key_t key;
  struct node_t *parent, *left, *right;
} node_t;

typedef struct {
  node_t *root;
  node_t *nil;  // for sentinel
} rbtree;

rbtree *new_rbtree(void);
void delete_rbtree(rbtree *);

node_t *rbtree_insert(rbtree *, const key_t);
node_t *rbtree_find(const rbtree *, const key_t);
node_t *rbtree_min(const rbtree *);
node_t *rbtree_max(const rbtree *);
int rbtree_erase(rbtree *, node_t *);

int rbtree_to_array(const rbtree *, key_t *, const size_t);
void rotate_left(rbtree*, node_t*);
void rotate_right(rbtree*, node_t*);
void refix_insert(rbtree*, node_t*);

void rbtree_insert_fixup(rbtree*, node_t* );
void refix_erase(rbtree*, node_t*, int);           //test erase refix
//void refix_erase(rbtree*, node_t*);
void trans_node(rbtree*, node_t*, node_t*);
void inorder_travel(rbtree*, node_t*);						//test용 메소드
void postorder_delete(rbtree*, node_t*);					//delete용 메소드



void exchange_color(node_t*, node_t*);
node_t* get_next_node(const rbtree*, node_t*);
#endif  // _RBTREE_H_