#include <stdio.h>
#include <math.h>
#include <stdlib.h>

typedef struct {
    int row,col;
    double ** m;
} mat;
mat* matrix_init(int row, int col)
{
    mat* new_ma = malloc(sizeof(mat));
    new_ma->m = malloc(sizeof(double*)*row);
    int i;
    for (i=0; i<row; i++)
    {
    	new_ma->m[i] = calloc(sizeof(double),col);
    }
    new_ma->row = row;
    new_ma->col = col;
    return new_ma;
}

mat* matrix_copy(int row, int col,mat* input)
{
    if(input->row != row || input->col != col)
    {
    	printf("input matrix is not %d * %d\n",row,col);
	exit(0);
    }
    mat* new_ma;
    new_ma = matrix_init(row,col);
    int i,j;
    for (i=0; i<row; i++)
    {
    	for (j=0; j<col; j++)
	{
	    new_ma->m[i][j] = input->m[i][j];
	}
    }
    return new_ma;
}

// get the nth col of matrix, need be freed outside the function 
double* get_col(mat* input,int n)
{
    int row = input->row;
    double *tmp = malloc(sizeof(double)*row);
    int i;
    for (i=0; i<row; i++)
    {
    	tmp[i] = input->m[i][n];
    }
    return tmp;
}

// mat m[i][j] = m[j][i]
void matrix_transpose(mat * input)
{
    int i,j;
    if (input->row == input->col)
    {
    	for (i=0;i<input->row;i++)
    	{
    	    for(j=0;j<i;j++)
	    {
	    	double tmp = input->m[i][j];
	    	input->m[i][j] = input->m[j][i];
	    	input->m[j][i] = tmp;
	    }
	}
    }else{
    	printf("can not transpose matrix\n");
    }
}

//vector a = a+b
void vector_add(double *a, double *b,int n)
{
    if(sizeof(a) != sizeof(b))
    {
    	printf("two vector can not add up\n");
    }else{
    	int i;
    	for (i=0;i<n;i++)
	{
	    a[i] += b[i];
	}
    }
}

// vector a = a * s
void vector_mul(double *a, double s, int n)
{
   int i;
   for(i=0;i<n;i++)
   {
   	a[i] = a[i] * s;
   }
}

// matrix a = a+b
void matrix_add(mat* a, mat* b)
{
    if (a->row != b->row || a->col != b-> col)
    {
    	printf("two matrix can not add up.\n");
    }else{
    	int i,j;
    	for (i=0;i<a->row;i++)
	{
	    for(j=0;j<a->col;j++)
	    {
	    	a->m[i][j] = a->m[i][j]+b->m[i][j];
	    }
	}
    }
}

// mat a * mat b
mat* matrix_mul(mat* input1, mat* input2)
{
    int i,j,k;
    mat* result = matrix_init(input1->row,input2->col);
    if (input1->col == input2->row)
    {
    	for (i=0;i<input1->row;i++)
	{
	    for(j=0;j<input2->col;j++)
	    {
	    	double tmp=0;
	    	for(k=0;k<input1->col;k++)
		{
		    tmp = tmp+(input1->m[i][k])*(input2->m[k][j]);
		}
		result->m[i][j] = tmp;
	    }
	}
    }else{
    	printf("col of m1 and row of m2 is not equal.\n");
    }
    return result;
}

// generate [I,0][0,x]
mat * matrix_reflector(mat *a, int n)
{
    mat *result = matrix_init(a->row,a->col);
    int i,j,k;
    for (i=0;i<n;i++)
    {
    	result->m[i][i] = 1;
    }
    for (j=n;j<a->row;j++)
    {
    	for(k=n;k<a->col;k++)
	{
	    result->m[j][k] = a->m[j][k];
	}
    }
    return result;
}

//||input|| a double vector
double vnorm(double* input,int num)
{
     int i;
     double tmp=0;
     for(i=0;i<num;i++)
     {
     	tmp += input[i]*input[i];
     }
     return sqrt(tmp);
}

// I - 2 * v * v^T
mat *I_mul(double *v,int n)
{
    mat *result = matrix_init(n,n);
    int i,j;
    for (i=0;i<n;i++)
    {
    	for(j=0;j<n;j++)
	{
	    result->m[i][j] = -2*v[i]*v[j];
	}
    }
    for (i=0;i<n;i++)
    	result->m[i][i] = 1+result->m[i][i];
    return result;
}


void matrix_free(mat* input)
{
    if(input->row && input->col)
    {
    	int i;
    	for(i=0; i<input->row; i++)
	{
	    free(input->m[i]);
	}
	free(input->m);
	free(input);
    }else{
    	printf("can not free matrix\n");
	exit(0);
    }
}
void matrix_show(mat *m)
{
    int i,j;
    for (i=0;i<m->row;i++)
    {
    	for(j=0;j<m->col;j++)
	{
	    printf("%f,",m->m[i][j]);
	}
	printf("\n");
    }
}
void householder(mat *m, mat *R, mat *Q)
{
    int i,j,k;
    mat *q[m->row];
    mat *tmp1 = m;
    mat * tmp2;
// some where need to be parallized
// base on row cutting, column cutting or matrix cutting 

    double *e = malloc(sizeof(double)*(m->row));
    double *n_col;
    double n_norm;
    for (i=0;i<m->row && i<m->col; i++)
    {
    	
    	tmp2 = matrix_reflector(tmp1,i);

	if (tmp1 != m) matrix_free(tmp1);
	tmp1 = tmp2;
	
	n_col = get_col(tmp1,i);
	n_norm = vnorm(n_col,m->row);

	n_norm = (m->m[i][i]<0) ? -n_norm:n_norm;
	for(j =0; j<m->row;j++){e[j]=0;}
	e[i] = 1;
	
	vector_mul(e,n_norm,m->row);
	vector_add(n_col,e,m->row);

	n_norm = vnorm(n_col,m->row);
	vector_mul(n_col,(double)1/n_norm,m->row);
	
	q[i]=I_mul(n_col,m->row);
	tmp2 = matrix_mul(q[i],tmp1);
	free(n_col);
	if (tmp1!=m) matrix_free(tmp1);
	tmp1 = tmp2;
    }
    free (e);
    matrix_free(tmp1);
    mat *tmp3=m;
    mat *tmp4;
    mat* tmp5=q[0];
    for (i=0;i<m->row && i<m->col;i++)
    {
    	if (i!=0) matrix_mul(tmp5,q[i]);
    	tmp4 = matrix_mul(q[i],tmp3);
	if (tmp3!=m) matrix_free(tmp3);
	tmp3 = tmp4;
	matrix_show(tmp3);
    }
    R = tmp3;
    matrix_show(tmp5);
}
int main(int argc, char* argv[])
{
    double in[][3] = {
    { 12, -51, 4},
	{  6, 167, -68},
	{ -4, 24, -41},
	{ -1, 1, 0},
	{2,0,3},
    };
    mat* x = matrix_init(5,3);
    int i,j,k;
    for (i=0;i<5;i++)
    {
    	for(j=0;j<3;j++)
	{
	     x->m[i][j] = in[i][j];
	}
    }
    mat *R,*Q;
    householder(x,R,Q);
    matrix_free(x);
}

