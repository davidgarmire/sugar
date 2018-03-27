#ifndef NEWTON_H
#define NEWTON_H

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
int newton(mesh_t mesh, int n, double *x, double *R,
           double (*norm) (int n, double *x),
           double rtol, double atol, int maxiter);

#endif /* NEWTON_H */
