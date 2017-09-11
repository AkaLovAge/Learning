//
//  main.c
//  test2
//
//  Created by XuLi on 8/17/17.
//  Copyright © 2017 XuLi. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#define SQ(x)     ((x)*(x))
struct kdnode {
    double coord_x;
    double coord_y;
    double coord_z;
    int split;
    double value;
    struct kdnode* right_child;
    struct kdnode* left_child;
    struct kdnode* parent;
};

struct kdtree {
    struct kdnode* root;
    int demension;
    double bbox[4];
    //   int col;
    //   int row;
    
};
void SWAP4(double *x,double *y)
{
    double SWAP[4];
    memcpy(SWAP,x,sizeof(double*)*4);
    for(int i=0;i<4;i++)
    {
        x[i]=y[i];
        y[i]=SWAP[i];
    }
}

void SWAP5(double *x,double *y)
{
    double SWAP[5];
    memcpy(SWAP,x,sizeof(double)*5);
    for(int i=0;i<5;i++)
    {
        x[i]=y[i];
        y[i]=SWAP[i];
    }
}
int partition(double points[][5], int a, int b, int direc)
{
    int i = a;
    double X = points[b][direc];
    
    for (int j= a; j<b-1; j++)
    {
        if (X >= points[j][direc])
        {
            SWAP5(points[i], points[j]);
            i++;
        }
    }
    SWAP5(points[i], points[b]);
    return i;
}

void quicksort (double points[][5], int a, int b, int direc)
{
    if (a >= b)
    {
        return;
    }
    int q = partition(points,a,b,direc);
    quicksort(points, a, q-1, direc);
    quicksort(points, q+1, b, direc);
}

void putforward(double **points,int position, int b, int *size)
{
    double p[4] ={points[position][0],points[position][1],points[position][2],points[position][3]};
    for (int i= position; i<b; i++){
        points[i][0] = points[i+1][0];
        points[i][1] = points[i+1][1];
        points[i][2] = points[i+1][2];
        points[i][3] = points[i+1][3];
    }
    points[b][0] = p[0];
    points[b][1] = p[1];
    points[b][2] = p[2];
    points[b][3] = p[3];
    *size = *size -1;
}

void Median_Point (double forreturn[5], double Pointset[3][5],int direc)
{
    quicksort(Pointset,0,9,direc);
    forreturn[0] = Pointset[5][0];
    forreturn[1] = Pointset[5][1];
    forreturn[2] = Pointset[5][2];
    forreturn[3] = Pointset[5][3];
    forreturn[4] = Pointset[5][4];
    
}

void RandomSelect (double*midpoint, double** pointSet, int *size, int direc, int a, int b)
{
    if ((b-a+1) > 10){

        //   srand(time(NULL));
        //int *ridrepeat;
        double result[10][5];
        int ridrepeat[100];// = calloc((*size),sizeof(int));

        int n = a + rand() % (b-a+1);
        for (int i = 0; i< 10; i++)
        {
            while (ridrepeat[n] == -1)
            {
                n = a + rand() % (b-a+1);
            }
            for(int l=0; l<4; l++)
            {
                
                result[i][l] = pointSet[n][l];
              //  printf("%f,",result[i][l]);
                
            }
            result[i][4] = n;
            
            ridrepeat[n] = -1;
            n =a + rand() % (b-a+1);
        }
        Median_Point(midpoint,result,direc);
       
        putforward(pointSet,(int)midpoint[4],b,size);
        
      //  free(ridrepeat);

    }else{
        midpoint[0] = pointSet[a+(b-a)/2][0];
        midpoint[1] = pointSet[a+(b-a)/2][1];
        midpoint[2] = pointSet[a+(b-a)/2][2];
        midpoint[3] = a+(b-a)/2;
        
        putforward(pointSet, a+(b-a)/2, b, size);
   /*     printf("the mid point is:\n");
        for (int i=0; i<3; i++)
        {
            printf("%f,",midpoint[i]);
        }
        printf("\n");*/
        return;
    }
    return;
}

int reorder(double **points, double value, int direc, int a, int b)
{
    int left =a;
    // printf("reorder begin! direc:%d, value:%f\n",direc,value);
    for (int right = a; right<= b; right++)
    {
        if (points[right][direc] <= value)
        {
            SWAP4(points[left], points[right]);
            left = left+1;
        }
    }
  /*  printf("reordering : %f, %d,%d,%d\n",value, a,b,left);
    for (int right = a; right<= b+1; right++)
    {
        printf("%f,",points[right][direc]);
    }
    printf("\n");*/
    return (left-1);
}
void node_create(struct kdnode *output, double x, double y, double z, int split,double value, struct kdnode* parent)
{
    (*output).coord_x = x;
    (*output).coord_y = y;
    (*output).coord_z = z;
    (*output).left_child = NULL;
    (*output).right_child = NULL;
    (*output).split = split;
    (*output).value = value;
    (*output).parent = parent;
    
}

void build_kdtree(struct kdnode* input_node, double** points, int countdirec, int* size,int a, int b, int* record)
{
/*    for (int i=0; i< 100; i++)
    {
        printf("%d,",i);
        for (int j=0; j<3; j++)
        {
            printf("%f,",points[i][j]);
        }
        printf("\n");
    }
    printf("\n");*/
 
   if (b<=a)
    {
        struct kdnode node;
        node_create(&node, points[a][0], points[a][1], points[a][2], countdirec, points[a][3], input_node);
        int direct = countdirec;
   //     printf("the last , %d\n",direct);
        printf("a point is:%d,%d\n",a,b);

        *record = *record + 1;
        switch (direct) {
            case 0:
                if (input_node->coord_x>=points[a][0])
                {
                    (*input_node).left_child = (struct kdnode*) malloc(sizeof(struct kdnode));
                  //  (*input_node).right_child = NULL;
                    memcpy((*input_node).left_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("left child:%f,%f,%f\n",points[a][0], points[a][1], points[a][2]);
         //           printf("%d\n",direct);
                }else{
                    (*input_node).right_child = (struct kdnode*) malloc(sizeof(struct kdnode));
                  //  (*input_node).left_child = NULL;
                    memcpy((*input_node).right_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("right child:%f,%f,%f\n",points[a][0], points[a][1],points[a][2]);
         //           printf("%d\n",direct);
                }
                break;
            case 1:
                if (input_node->coord_y>=points[a][1])
                {
                    (*input_node).left_child = (struct kdnode*) malloc(sizeof(struct kdnode));
                 //   (*input_node).right_child = NULL;
                    memcpy((*input_node).left_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("left child:%f,%f,%f\n",points[a][0], points[a][1], points[a][2]);
                    printf("%d\n",direct);
                }else{
                    (*input_node).right_child = (struct kdnode*) malloc(sizeof(struct kdnode));
                 //   (*input_node).left_child = NULL;
                    memcpy((*input_node).right_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("right child:%f,%f,%f\n",points[a][0], points[a][1], points[a][2]);
         //           printf("%d\n",direct);
                }
                break;
            case 2:
                if (input_node->coord_z>=points[a][2])
                {
                    (*input_node).left_child = (struct kdnode*) malloc(sizeof(struct kdnode));
                 
                    memcpy((*input_node).left_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("left child:%f,%f,%f\n",points[a][0], points[a][1], points[a][2]);
          //          printf("%d\n",direct);
                }else{
                    (*input_node).right_child = (struct kdnode*) malloc(sizeof(struct kdnode));
            
                    memcpy((*input_node).right_child, &node,sizeof(struct kdnode));
                    printf("node:%f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
                    printf("right child:%f,%f,%f\n",points[a][0], points[a][1], points[a][2]);
           //         printf("%d\n",direct);
                }
                break;
                
            default:
                printf("error occurs when creating the leaf!\n");
                exit(0);
                break;
        }
        return;
    }
    
 /*   if ((b-a+1) == 2)
     {
     struct kdnode node1;
     struct kdnode node2;
     node_create(&node1, points[0][0], points[0][1], countdirec, points[0][2], NULL, NULL, input_node);
     
     node_create(&node1, points[1][0], points[1][1], countdirec, points[1][2], NULL, NULL, input_node);
     
     if (points[1][countdirec] > points[0][countdirec])
     {
     input_node->left_child = &node1;
     input_node->right_child = &node2;
     }else{
     input_node->right_child = &node2;
     input_node->right_child = &node1;
     }
     return;
     }*/
    
    int direct = countdirec;
    direct= (direct+1) % 3;
    double value = points[b+1][direct];
    printf("the value is: %f\n",value);
    int split_index =0;
    split_index = reorder(points, value, direct,a,b);
    
    struct kdnode leftnode;
    struct kdnode rightnode;
    double left_midpoint[5]={0,0,0,0,0};
    double right_midpoint[5]={0,0,0,0,0};
    bool hasleft = true;
    bool hasright = true;
    if (split_index < a){
        hasleft = false;
    }
    if (split_index >= b)
    {
        hasright = false;
    }
    printf("a point is:%d,%d,%d\n",a,split_index,b);
    if (hasleft)
    {
        if (split_index == a)
        {
       //     printf("node %f,%f,%f\n",input_node->coord_x,input_node->coord_y,input_node->coord_z);
       //     printf("left child:%f,%f,%f\n",points[a][0],points[a][1],points[a][2]);
       //     printf("a:%d,b:%d\n",a,split_index);
         build_kdtree(input_node, points, direct, size,a,split_index,record);
        }else{
      //      printf("a :%d,b: %d\n",a,b);
          //  printf("find randon from %d to %d",a,split_index);
            printf("left normal\n");
            RandomSelect(left_midpoint, points, size, direct,a,split_index);
            printf("node %f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
            printf("left child:%f, %f, %f\n",left_midpoint[0],left_midpoint[1],left_midpoint[2]);
     //       printf("%d\n",direct);
      //      printf("a:%d,b:%d\n",a,split_index-1);
            node_create(&leftnode, left_midpoint[0], left_midpoint[1], left_midpoint[2], direct, left_midpoint[3], input_node);
            (*input_node).left_child = (struct kdnode*) malloc(sizeof(struct kdnode));
            memcpy((*input_node).left_child, &leftnode,sizeof(struct kdnode));
            *record = *record + 1;
            build_kdtree((*input_node).left_child, points, direct, size,a,split_index-1,record);
        }
    }
 //   printf("recording:%d\n\n",*record);
    printf("a point is:%d,%d,%d\n",a,split_index,b);

    if (hasright)
    {
        if (b == split_index+1)
        {
       //     printf("node %f,%f,%f\n",input_node->coord_x,input_node->coord_y,input_node->coord_z);
            
       //     printf("right child:%f,%f,%f\n",points[b][0],points[b][1],points[b][2]);
       //     printf("a:%d,b:%d\n",split_index+1,b);
            build_kdtree(input_node, points, direct, size,split_index+1,b,record);
        }else{
        //    printf("a point is:%d,%d,%d\n",a,split_index,b);
         //   printf("find randon from %d to %d",split_index+1,b);
            printf("the right normal\n");
            RandomSelect(right_midpoint, points, size, direct,split_index+1,b);
            printf("node %f,%f,%f,",input_node->coord_x,input_node->coord_y,input_node->coord_z);
            printf("right child:%f, %f,%f\n",right_midpoint[0],right_midpoint[1],right_midpoint[2]);
      //      printf("%d\n",direct);
      //      printf("a:%d,b:%d\n",split_index+1,b-1);
            node_create(&rightnode, right_midpoint[0], right_midpoint[1],right_midpoint[2],direct, right_midpoint[3], input_node);
            (*input_node).right_child = (struct kdnode*) malloc(sizeof(struct kdnode));
            memcpy((*input_node).right_child, &rightnode,sizeof(struct kdnode));
            *record = *record + 1;
            build_kdtree((*input_node).right_child, points, direct, size,split_index+1,b-1,record);
        }
    }
    return;
}

void kd_search(struct kdnode *node, int* a)
{
    if (node)
    {
        printf("the node is:%f,%f,%f\n",(node->coord_x-1010000),(node->coord_y-1249900),(node->coord_z-700));
      //  printf("%d\n",*a);
        *a = *a + 1;
        kd_search(node->left_child, a);
        kd_search(node->right_child,a);
    }else{
        return;
    }
}
void kd_delete(struct kdnode* node)
{
    
    if (node !=NULL )
    {
        if (node->left_child != NULL)
        {
            kd_delete(node->left_child);
            free(node);
        }
        if (node->right_child != NULL)
        {
            kd_delete(node->right_child);
            free(node);
        }
    }else return;
}

struct kdtree kdtree_create (int k, double** pointSet)
{
    if (! (k > 0))
    {
        printf ("K must be larger than 0");
        exit(0);
    }
    
    struct kdtree tree;
    /*  double tem_minx = 10000.0;
     double tem_maxx = 0.0;
     double tem_miny = 10000.0;
     double tem_maxy = 0.0;
     
     for (int i=0 ; i<col ; i++)
     {
     if (tem_maxy < pointSet[i][1])
     {
     tem_maxy = pointSet[i][1];
     }
     if (tem_miny > pointSet[(row-1)*col+i][1])
     {
     tem_miny = pointSet[(row-1)*col+i][1];
     }
     }
     
     for (int i=0 ; i<row ; i++)
     {
     if (tem_minx > pointSet[i*col][0])
     {
     tem_minx = pointSet[i*col][0];
     }
     if (tem_maxx < pointSet[(i+1)*col-1][0])
     {
     tem_maxx = pointSet[(i+1)*col-1][0];
     }
     }
     
     tree.boundary[0] = tem_minx;
     tree.boundary[1] = tem_miny;
     tree.boundary[2] = tem_maxx;
     tree.boundary[3] = tem_maxy;
     */
    tree.demension = k;
    //  tree.col = col;
    //  tree.row = row;
    
    int remain_size = 100;
    int b = 99;
    double midpoint[5]={0,0,0,0,0};
    int recording = 1;
    RandomSelect(midpoint, pointSet, &remain_size,0,0,b);
    b--;
    struct kdnode root;
    
    root.coord_x = midpoint[0];
    root.coord_y = midpoint[1];
    root.coord_z = midpoint[2];
    root.value = midpoint[3];

    int count_direc=2;
    build_kdtree(&root, pointSet, count_direc, &remain_size,0,b,&recording);
    tree.root = &root;
    int num = 1;
    kd_search(&root,&num);
    printf("%d\n",num);
    return tree;
}

void kd_free(struct kdtree* tree)
{
    
}

void kd_clean(struct kdtree* tree)
{
    
}

void kd_insert(struct kdtree* tree, double x, double y, void* value)
{
    
}



void kd_nearest(struct kdtree* tree, double x, double y)
{
    
}
void txt_point_reader(const char *filename, double**points)
{
    FILE *fp;
    size_t len = 0;
    ssize_t read1;
    char *line1 = NULL;
    fp = fopen(filename,"r");
    printf("open file %s\n",filename);
    int i = 0;
    if (fp == NULL)
    {
        printf("Can not find file\n");
        exit (EXIT_FAILURE);
    }
    const char s[2] = ",";
    double token1[4];
    read1 = getline(&line1, &len, fp);
    read1 = getline(&line1, &len, fp);
    token1[0] = atof(strtok(line1,s));
    token1[1] = atof(strtok(NULL,s));
    token1[2] = atof(strtok(NULL,s));
    token1[3] = atof(strtok(NULL,s));
    points[i][0] = token1[0];
    points[i][1] = token1[1];
    points[i][2] = token1[2];
    points[i][3] = token1[3];
    i++;
    while (i < 100)
    {
        read1 = getline(&line1, &len, fp);
        token1[0] = atof(strtok(line1,s));
        token1[1] = atof(strtok(NULL,s));
        token1[2] = atof(strtok(NULL,s));
        token1[3] = atof(strtok(NULL,s));
        
        if ((token1[0] != points[i-1][0]) & (token1[1] != points[i-1][0]) & (token1[2] != points[i-1][0]))
        {
            points[i][0] = token1[0];
            points[i][1] = token1[1];
            points[i][2] = token1[2];
            points[i][3] = token1[3];
            i++;
        }
    }
    fclose(fp);
    if (line1)
        free(line1);
    return;
}
int main(int argc, const char * argv[]) {
    
    double ** pointtest;
    pointtest = (double**)malloc(100*sizeof(double*));
    
    for (int i=0; i<100; i++)
    {
        pointtest[i] = (double*)malloc(4*sizeof(double));
    }
    txt_point_reader("/Users/lixu/workplace/Lidar/kdtree/test2/test2/test.txt",pointtest);
    
    for (int i=0; i< 100; i++)
    {
        for (int j=0; j<4; j++)
        {
            printf("%f,",pointtest[i][j]);
        }
        printf("\n");
    }
    kdtree_create(3, pointtest);
    //free the memory
    for (int i =0; i<100;i++)
        free(pointtest[i]);
    free(pointtest);
    
    return 0;
}
