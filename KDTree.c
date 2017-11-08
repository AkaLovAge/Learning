#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Point.h"
#include "KDTree.h"
void KDNode_init(KDNode *knode)
{
	knode->dims = 0;
	knode->axis = 0;
	knode->left = NULL;
	knode->right = NULL;
	knode->data = NULL;
	knode->pivot = 0.0f;
}
void KDList_init(KDList *klist,KDNode *node,  double dis)
{
	klist->node = node;
	klist->dis = dis;
	klist->next = NULL;
	klist->last = NULL;
}
void KDList_insert(KDList **head, KDList **node)
{
	if (*head == NULL)
	{
//		printf("insert is NULL %f\n", (*node)->dis);
		*head = malloc(sizeof(KDList));
		memcpy(*head,*node,sizeof(KDList));
		(*head)->next = NULL;
		(*head)->last = *head;
		return;
	}
//	printf("insert node %f to",(*node)->dis);
	KDList *search = *head;
	int tmp = 0;
	while (search != NULL)
	{
//		printf("%f,%f\n",(*node)->dis , search->dis);
		if ((*node)->dis < search->dis)
		{
			(*node)->last = search->last;
			(*node)->next = search;
			if (tmp == 0)
			{
				*head = *node;
				search->last->next = NULL;
			}else{
				search->last->next = *node;
			}
			search->last = *node;
			return;
		}else{
			tmp = 1;
			search = search->next;
		}
	}
	(*node)->next= NULL;
	(*node)->last = (*head)->last;
	(*head)->last->next = *node;
	(*head)->last = *node;
	return;
}
void KDList_pop(KDList **head, KDList* popnode)
{
	if (*head == NULL)
	{
//		printf("NULL in search path \n");
		return;
	}	
	if ((*head)->last == *head)
	{
		(*head)->last = NULL;
		memcpy(popnode,*head, sizeof(KDList));
		free (*head);
		*head = NULL;
		return;
	}
	KDList *tmp = *head;
	memcpy(popnode, *head, sizeof(KDList));
	*head = (*head)->next;
	(*head)->last = tmp->last;
	free(tmp);
	tmp->last = NULL;
	return;
}
void KDList_free(KDList** head)
{
	if (*head == NULL)
		return;
	KDList *tmp = *head;
	while (tmp->next != NULL)
	{
		tmp = tmp->next;
		free(tmp->last);
		tmp->last = NULL;
	}
	free(tmp);
	return;
}
void list2array(Point* array, KDList* head)
{
	KDList* tmp =head;
	int count = 0;
	while (tmp != NULL){
		array[count].x = tmp->node->data->x;
		array[count].y = tmp->node->data->y;
		array[count].z = tmp->node->data->z;
		tmp = tmp->next;
		count ++;
	//	printf("count:%d\n",count);
	}
	KDList_free(&head);
}
int KDNode_insert (Point *point_list, int depth, int dims, int point_count, KDNode* knode)
{
	knode->dims = dims;
	knode->axis = depth % dims;
	int l_depth = 0;
	int r_depth = 0;
	int depth_flag = 0;
	if (point_count == 1)
	{
		knode->data = malloc (sizeof(Point));	
		Point_copy (knode->data,point_list);
		depth_flag = depth;
	}else{
		int piv_idx = floor(point_count / 2);
		switch(knode->axis)
		{
			case(0):
				qsort(point_list, point_count, sizeof(Point), Point_compareX);
				knode->pivot = (point_list[piv_idx].x + point_list[piv_idx+1].x) / 2;
				break;

			case(1):
				qsort(point_list, point_count, sizeof(Point), Point_compareY);
                knode->pivot = (point_list[piv_idx].y + point_list[piv_idx+1].y) / 2;
				break;

			case(2):
				qsort(point_list, point_count, sizeof(Point), Point_compareZ);
                knode->pivot = (point_list[piv_idx].z + point_list[piv_idx+1].z) / 2;
				break;

			default:
				printf("DIM ERROR: failed to recognize axis %i\n",knode->axis);
				break;
		}
		
		int new_count = point_count / 2;
		int add_off = 0;
		if (point_count % 2)
			add_off ++;
		knode->left = malloc(sizeof(KDNode));
		KDNode_init(knode->left);
		knode->right = malloc(sizeof(KDNode));
		KDNode_init(knode->right);
		l_depth = KDNode_insert(point_list, depth+1, dims, new_count,knode->left);
		r_depth = KDNode_insert(&point_list[new_count],depth+1,dims,new_count+add_off,knode->right);
		depth_flag = l_depth > r_depth ? l_depth : r_depth;
	}
	return depth_flag;
}
	
int KDNode_insert_node(Point *point_list, int depth, int dims, int point_count, KDNode* knode)
{
	//printf("Allocating node at depth %i, pts: %i\n", depth, point_count);
	//KDNode *knode = malloc(sizeof(KDNode));
	//printf("Successfully allocated node\n");
	//KDNode_init(knode);
	knode->dims = dims;
	knode->axis = depth % dims;
	int l_depth = 0;
	int r_depth = 0;
	int depth_flag = 0;
	//knode->left = NULL;
	//knode->right = NULL;
	//knode->data = NULL;
	if (point_count == 1)
	{
		knode->data = malloc(sizeof(Point));
		Point_copy(knode->data, point_list);
		depth_flag = depth;
	} else {
		int piv_idx = floor(point_count / 2);
		int new_count = point_count / 2 ; 
	//	printf("point_count:%d\n",point_count);
		switch(knode->axis) 
		{	
			case(0):
				qsort(point_list, point_count, sizeof(Point), Point_compareX);
			//	printf("Creating pivot\n");
				while ((new_count<point_count-1) && (point_list[new_count].x == point_list[new_count+1].x))
				{	
					new_count++;
				}
			//	printf("x new_count:%d\n",new_count);
				knode->pivot = (point_list[piv_idx].x + point_list[piv_idx+1].x) / 2;
				break;
			case(1):
				qsort(point_list, point_count, sizeof(Point), Point_compareY);
			//	printf("Creating pivot\n");
				while ((new_count<point_count-1)&&(point_list[new_count].y == point_list[new_count+1].y))
				{
					new_count++;
				}
			//	printf("y new_count:%d\n",new_count);
				knode->pivot = (point_list[piv_idx].y + point_list[piv_idx+1].y) / 2;
				break;
			case(2):
				qsort(point_list, point_count, sizeof(Point), Point_compareZ);
				while ((new_count<point_count-1)&&(point_list[new_count].z == point_list[new_count+1].z))
				{
					new_count++;
				}
			//	printf("z new_count:%d\n",new_count);
			//	printf("Creating pivot\n");
				knode->pivot = (point_list[piv_idx].z + point_list[piv_idx+1].z) / 2;
				break;
			default:
				printf("DIM ERROR: failed to recognize axis %i\n", knode->axis);
				break;
		}
		// TODO: Check how this works on even and odd subsets
 		knode->data = malloc(sizeof(Point));
		Point_copy(knode->data,&point_list[new_count]);

		//int add_off = 0;
		//if (point_count % 2 == 0) add_off++;
		knode->left = malloc(sizeof(KDNode));
		KDNode_init(knode->left);
		//printf("left \n");
		l_depth = KDNode_insert_node(point_list, depth+1, dims, new_count, knode->left);
		if ((point_count - new_count - 1) != 0 )
		{
			knode->right = malloc(sizeof(KDNode));
			KDNode_init(knode->right);
			//printf("right \n");
			r_depth = KDNode_insert_node(&point_list[new_count+1], depth+1, dims, point_count-new_count-1, knode->right);
		}
		// Set new depth flag
		depth_flag = l_depth > r_depth ? l_depth : r_depth;
	}
	return depth_flag;
}
void KDList_full_insert(KDList **head,KDList **node)
{
	if (*head == NULL)
	{
		return;
	}
	printf("start full insert\n");
	KDList *search = *head;
	int tmp = 0;
	KDList *last_node = (*head)->last;
	while (search != NULL)
	{   
		if ((*node)->dis < (search)->dis)
		{   
			(*node)->last = search->last;
			(*node)->next = search;
			if (tmp ==0)
			{
				*head = *node;
				search->last->next = NULL;
			}else{
				search->last->next = *node;
			}
			search->last = *node;
			(*head)->last = (*head)->last->last;
			free(last_node);
			(*head)->last->next = NULL;
			return;
		}else{
			tmp = 1;
			search = search->next;
		}
	}
	(*node)->next= NULL;
	(*node)->last = (*head)->last;
	(*head)->last->next = *node;
	(*head)->last = *node;

	(*head)->last = (*head)->last->last;
	free(last_node);
	(*head)->last->next = NULL;
	return;	
}
void KDNode_search_knn(KDNode* knode, Point *pt, Point *best,int k)
{
	if (knode == NULL )
	{
		return;
	}
	double thred_distance = 9999999999.0;
	KDList* search_path=NULL;
	KDList* neighbor = NULL; 
	int best_count = 0;
	
	search_path = malloc(sizeof(KDList));

	KDList* near = malloc(sizeof(KDList));
	KDList* far = malloc(sizeof(KDList));
	KDList* popnode = malloc(sizeof(KDList));
	
	double dis = Point_distance(pt, knode->data);
	KDList_init(search_path, knode, dis);
	search_path->last = search_path;
	int if_insert_popnode = 0;
	while (search_path != NULL)
	{
		KDList_pop(&search_path, popnode);
	//	printf("the pop node is:%f\n", popnode->dis);

		if (popnode->dis < thred_distance)
		{
			if (best_count < k)
			{
				KDList_insert(&neighbor,&popnode);
	//			printf("neighbor:%f\n",popnode->dis);
				if_insert_popnode = 1;
				best_count ++;
			}else{
				KDList_full_insert(&neighbor,&popnode);
				if_insert_popnode = 1;
	//			printf("neighbor:%f\n",popnode->dis);
	//			printf("the head of neigh:%f\n",neighbor->dis);
				thred_distance = neighbor->last->dis;
	//			printf("new thredhold is:%f\n",thred_distance);
			}
		}
		printf("larger than the bigest element in list\n");
		switch(popnode->node->axis)
		{
			case(0):
				if (pt->x <= popnode->node->data->x)
				{
					if (popnode->node->left)
					{
						KDList_init(near, popnode->node->left, Point_distance(pt, popnode->node->left->data));
					}else{
						free(near);
						near = NULL;
					}
					if (popnode->node->right)
					{
						KDList_init(far, popnode->node->right, Point_distance(pt, popnode->node->right->data));
					}else{
						free(far);
						far = NULL;
					}
				}else{
					if (popnode->node->left)
					{
						KDList_init(far, popnode->node->left, Point_distance(pt, popnode->node->left->data));
					}else{
						free(far);
						far = NULL;
					}
					if (popnode->node->right)
					{
						KDList_init(near, popnode->node->right, Point_distance(pt, popnode->node->right->data));
					}else{
						free(near);
						near = NULL;
					}
				}
				break;
			case(1):
				if (pt->y <= popnode->node->data->y)
				{
                    if (popnode->node->left)
                    {
                        KDList_init(near, popnode->node->left, Point_distance(pt, popnode->node->left->data));
                    }else{
                        free(near);
                        near = NULL;
                    }
                    if (popnode->node->right)
                    {
                        KDList_init(far, popnode->node->right, Point_distance(pt, popnode->node->right->data));
                    }else{
                        free(far);
                        far = NULL;
                    }
                }else{
                    if (popnode->node->left)
                    {
                        KDList_init(far, popnode->node->left, Point_distance(pt, popnode->node->left->data));
                    }else{
                        free(far);
                        far = NULL;
                    }
                    if (popnode->node->right)
                    {
                        KDList_init(near, popnode->node->right, Point_distance(pt, popnode->node->right->data));
                    }else{
                        free(near);
                        near = NULL;
                    }
                }
				break;
			case(2):
				if (pt->z <= popnode->node->data->z)
				{
                    if (popnode->node->left)
                    {
                        KDList_init(near, popnode->node->left, Point_distance(pt, popnode->node->left->data));
                    }else{
                        free(near);
                        near = NULL;
                    }
                    if (popnode->node->right)
                    {
                        KDList_init(far, popnode->node->right, Point_distance(pt, popnode->node->right->data));
                    }else{
                        free(far);
                        far = NULL;
                    }
                }else{
                    if (popnode->node->left)
                    {
                        KDList_init(far, popnode->node->left, Point_distance(pt, popnode->node->left->data));
                    }else{
                        free(far);
                        far = NULL;
                    }
                    if (popnode->node->right)
                    {
                        KDList_init(near, popnode->node->right, Point_distance(pt, popnode->node->right->data));
                    }else{
                        free(near);
                        near = NULL;
                    }
                }
				break;
			default:
				printf("can not recognize the axis\n");
				break;
		}
	//	printf("find the near and far\n");
		if(near)
		{
	//		printf("near is:%f\n",near->dis);
			KDList_insert(&search_path, &near);
	//		printf("insert near to the list\n");
			near = NULL;
			near = malloc(sizeof(KDList));
		}else{
	//		printf("no near\n");
			near = malloc(sizeof(KDList));
		}
		if(far)
		{		
			if (far->dis < thred_distance)
			{
				printf("far is:%f\n",far->dis);
				KDList_insert(&search_path, &far);
				far = NULL;	
				far = malloc(sizeof(KDList));	
			}
		}else{	
	//		printf("no far\n");
			far = malloc(sizeof(KDList));
		}
		if (if_insert_popnode ==1)
		{		
			popnode = NULL;
			popnode = malloc(sizeof(KDList));	
		}
		if_insert_popnode = 0;
	}
	
	free(popnode);
	list2array(best,neighbor);
	return;
}

void KDNode_search_n(KDNode* knode, Point *pt, Point *best)
{
	if (knode == NULL)
	{
	//	printf("The knode is NULL in search function");
		return;
	}
	KDNode search_path[2048]; 
	KDNode* p_search = malloc(sizeof(KDNode));
	memcpy(p_search, knode, sizeof(KDNode));
	int path_index = 0;

	while (p_search != NULL)
	{
		search_path[path_index] = *p_search;
		printf("p_search:%f,%f,%f, axis:%d\n",p_search->data->x,p_search->data->y,p_search->data->z,p_search->axis);
	//	printf("axis:%d\n",p_search->axis);
	//	printf("left:%f,right:%f\n",p_search->left->data->x,p_search->right->data->x);
			
	//	printf("path:%f,%f,%f\n",search_path[path_index].data->x,search_path[path_index].data->y,search_path[path_index].data->z);
		path_index++;
		switch(p_search->axis)
		{
			case (0):
				p_search = (pt->x <= p_search->data->x) ? p_search->left : p_search->right;
				break;
			case (1):
				p_search = (pt->y <= p_search->data->y) ? p_search->left : p_search->right;
				break;
			case (2):
				p_search = (pt->z <= p_search->data->z) ? p_search->left : p_search->right;
				break;
			default:
				printf("can not recognize the axis\n");
				break;
		}	
	}	
	printf("path number:%d\n",path_index);
	free(p_search);
	path_index--;
	double radius = Point_distance (pt, best);
//	printf("path number:%d\n",path_index);
	if (radius > Point_distance (pt,search_path[path_index].data))
	{
		best->x = search_path[path_index].data->x;
		best->y = search_path[path_index].data->y;
		best->z = search_path[path_index].data->z;
		radius = Point_distance (pt,search_path[path_index].data);
	}	
//	memcpy(best[best_index],search_path[path_index],sizeof(Point));
//	best_index ++;
	path_index--;
	while (path_index > 0)
	{
	//	printf("the index in search loop is:%d\n",path_index);
		if (radius > Point_distance (pt,search_path[path_index].data))
		{
			best->x = search_path[path_index].data->x;
			best->y = search_path[path_index].data->y;
			best->z = search_path[path_index].data->z;                      
			radius = Point_distance (pt,search_path[path_index].data);
		}
//		printf("radius:%f\n",radius);
		if ((search_path[path_index].data->x > (pt->x - radius) && search_path[path_index].data->x < (pt->x + radius)) || (search_path[path_index].data->y > (pt->y-radius) && search_path[path_index].data->y < (pt->y+radius)) ||(search_path[path_index].data->z > (pt->z-radius) && search_path[path_index].data->z < (pt->z+radius)) )
		{
	//		printf("recursion:\n");
			switch(search_path[path_index].axis)
        	{
            	case (0):
                	if (pt->x <= knode->data->x)
						{	  
							KDNode_search_n(search_path[path_index].right, pt , best);
						}else{
							KDNode_search_n(search_path[path_index].left, pt, best);
						}
                	break;
            	case (1):
                	if (pt->y <= knode->data->y)
					{
						KDNode_search_n(search_path[path_index].right, pt , best);
					}else{
						KDNode_search_n(search_path[path_index].left, pt, best);
					}
               		break;
            	case (2):
                	if (pt->z <= knode->data->z)
				 	{
						KDNode_search_n(search_path[path_index].right, pt , best);
					}else{      
						KDNode_search_n(search_path[path_index].left, pt, best);
					}
                	break;
            	default:
                	printf("can not recognize the axis\n");
					break;
			}
			//path_index--;
		//	printf("path number:%d\n",path_index);		
		}	
		path_index--;
	}
	return;	
}

Point* KDNode_search(KDNode* knode, Point *pt, Point *best)
{
	if (knode->data != NULL)
	{
		double d_n, d_best;
		d_n = Point_distance(pt, knode->data);
		d_best = Point_distance(pt, best);
		if (d_n < d_best)
			return knode->data;
		else
			return best;
	} else { // Search nodes
		Point *p1, *p2;
		p1 = KDNode_search(knode->left, pt, best);
		p2 = KDNode_search(knode->right, pt, best);
		double d1, d2, d_best;
		d1 = Point_distance(pt, p1);
		d2 = Point_distance(pt, p2);
		d_best = Point_distance(pt, best);
		if (d_best < d1 && d_best< d2) 
			return best;
		else if( d1 < d2 && d2 < d_best) 
			return p2;
		else 
			return p1;
	}
}

void KDNode_delete(KDNode *knode) 
{
	if (knode != NULL)
	{
		knode->axis = 0;
		knode->dims = 0;
		knode->pivot = 0.0f;
		if (knode->data != NULL)
		{
			free(knode->data);
			knode->data = NULL;
		}
		KDNode_delete(knode->left);
		KDNode_delete(knode->right);
		free(knode);
		knode = NULL;
	}
}
	
void KDNode_vis(KDNode *knode,int *k)
{
	if (knode != NULL)
	{
	//	printf("the node is:%.3f,%.3f,%.3f",(knode->data)->x,(knode->data)->y,(knode->data)->z);
	//	if (knode->left)	
	//		printf(" left child is:%.3f,%.3f,%.3f",(knode->left->data)->x,(knode->left->data)->y,(knode->left->data)->z);
	//	if (knode->right)
	//		printf(" right child is:%.3f,%.3f,%.3f\n",(knode->right->data)->x,(knode->right->data)->y,(knode->right->data)->z);
		*k = (*k)+1;
		KDNode_vis(knode->left,k);
		KDNode_vis(knode->right,k);
		return;	
	}else{
		return;
	}		
}
