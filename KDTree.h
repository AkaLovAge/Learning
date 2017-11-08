#ifndef KDTREE_H
#define KDTREE_H
#include <stdio.h>
#include <stdlib.h>
#include "Point.h"

typedef struct KDNode {
	int axis;
	int dims;
	double pivot;
	struct KDNode *left;
	struct KDNode *right;
	Point *data;
} KDNode;

typedef struct KDTree {
	int dims;
	int depth;
	KDNode *root;
} KDTree;
typedef struct KDList{
	KDNode *node;
	double dis;
	struct KDList *next;
	struct KDList *last;
} KDList;
// Return the tree depth

void KDNode_init(KDNode *knode);

void KDList_init(KDList *klist, KDNode *node, double dis);

void KDList_insert(KDList **head, KDList **node);

void KDList_pop(KDList **head, KDList *popnode);

void KDList_free(KDList **head);

void KDList_full_insert(KDList **neightbor, KDList **popnode);

void list2array(Point *array, KDList* head);

int KDNode_insert(Point * point_list, int depth, int dims, int point_count, KDNode *knode);

int KDNode_insert_node(Point * point_list, int depth, int dims, int point_count, KDNode *knode);

Point* KDNode_search(KDNode* knode, Point *pt, Point *best);

void KDNode_delete(KDNode* knode);

void KDNode_vis(KDNode* knode, int *k);

void KDNode_search_n(KDNode* knode, Point *pt, Point *best);

void KDNode_search_knn(KDNode* kndoe, Point* pt, Point* best, int k);
#endif
