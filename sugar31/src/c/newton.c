#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include  "mesh.h"
#include  "modelmgr.h"
#include  "assemble.h"
#include  "superlu.h"


/*
 * compute  y = alph*x + y; 
 *  n - the dimension of vectors x, y
 */
void daxpy(int n, double alph, double *x, double *y)
{
    int k;
    for (k = 0; k < n; ++k)
        y[k] += alph * x[k];
    return;
}


/*
 * compute  || x ||_1 
 *  n - the dimension of vectors x 
 */
double norm1(int n, double *x)
{
    int k;

    double nrm = 0;

    for (k = 0; k < n; ++k)
        nrm = (fabs(x[k]) > nrm) ? fabs(x[k]) : nrm;

    return nrm;
}


int newton(mesh_t mesh, int n, double *x, double *R,
           double (*norm) (int n, double *x),
           double rtol, double atol, int maxiter)
/*  
* this function is to find the solution of mesh by newton's method
*
* inputs:
*   n - the dimension of equations;
*   x -  an initial guess of the solution
*   R - the residual vector ;
*   norm - is a function with arguments (x, n) for 
*                 evaluating the norm of vector x;
*              if it's NULL, we'll use the default 1-norm; 
*   atol - the absolute accuracy tolerance ;
*   rtol - the relative accuracy tolerance ;
*  maxiter - the maximum number of iterations allowed ;
*  
* outputs:
*  mesh, n, rtol,atol, and maxiter are returned without being changed ;
*  x is the solution vector;
*  R is the residual vector;
*  the returned value of function is the number of iterations
*/
{
#ifdef HAS_SUPERLU
#define  max(a,b)  ( ( a < b )?  (b):(a) )
#define  NORM(n,y)  ( (norm== NULL) ?  (norm1(n,y)) : (norm(n,y)) )
    int k;
    double normR, normX;
    double *dx = calloc(n, sizeof(double));
    superlu_t dR;
    assembler_t semb = mesh_assembler(mesh);

    assemble_R(semb, x, NULL, NULL, R, n);
    k = 0;

    normR = NORM(n,R);
    normX = NORM(n,x);

    while (normR >= max(rtol * normX, atol) && k <= maxiter) {
        memcpy(dx, R, n * sizeof(double));
        dR = assemble_dR_csc(semb, 1, x, 0, NULL, 0, NULL, n);
        superlu_solve(dR, dx);
        superlu_destroy(dR);
        daxpy(n, -1, dx, x);
        assemble_R(semb, x, NULL, NULL, R, n);
        normR = NORM(n,R);
        normX = NORM(n,x);
        k += 1;
    }
    printf("After %d iterations: normR = %g; normX = %g\n", k, normR, normX);
    free(dx);

    return k;
#endif

}
