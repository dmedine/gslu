///////////////////////gsl_utils.c////////////////////////
/* A set of functions for using matrices in pure C.     */

/* These utilities use the GNU scientific library       */
/* and are intended as utilities for doing matrix       */
/* manipulations for openGL 3D vecotr graphics          */
/* programs.                                            */

/* The idea is to replace the commonly used gsm library */
/* of matrix utilities which are in C++.                */
//////////////////////////////////////////////////////////

#include "gsl_utils.h"


static int g_foo = 0;
static void show_matrix(gsl_matrix *x){

 int i, j;
  for(i=0; i<x->size1; i++)
    for(j=0; j<x->size2; j++)
      printf("row %d, col %d : %f\n", i, j, gsl_matrix_get(x, i, j));


}

void gslu_show_matrix(gsl_matrix *x){

 int i, j;
  for(i=0; i<x->size1; i++)
    for(j=0; j<x->size2; j++)
      printf("row %d, col %d : %f\n", i, j, gsl_matrix_get(x, i, j));


}


void gslu_gsl_matrix_2_float_array(float *matrix, gsl_matrix *x){

  int i, j;
  //gsl is row major, but openGL is collumn major
  //thus we switch the ordering along these lines
  for(i=0; i<4; i++)
      for(j=0; j<4; j++)
  	  matrix[i*4 + j] = (float)gsl_matrix_get(x, j, i);

  /* if(g_foo==1) */
  /*   { */
  /*     printf("mtx2float\n"); */
  /*     show_matrix(x); */
  /*     for(i=0; i<16; i++) */
  /* 	printf("%d, %f\n", i, matrix[i]); */
  /*   } */
 
}

void gslu_mult_matrices(gsl_matrix *C,
			gsl_matrix *A,
			gsl_matrix *B){
  
  gsl_blas_dgemm(CblasNoTrans, 
		 CblasNoTrans, 
		 1.0, 
		 A, 
		 B, 
		 0.0, C);

}

void gslu_set_id_matrix(gsl_matrix *x){

  int i, j;
  for(i=0; i<x->size1; i++)
    for(j=0; j<x->size2; j++)
      if(i==j)gsl_matrix_set(x, i, j, 1.0);
      else gsl_matrix_set(x, i, j, 0.0);

}

gsl_matrix *gslu_gen_id_matrix(int row, int col){

  gsl_matrix *x = gsl_matrix_alloc(row, col);
  gslu_set_id_matrix(x);
  return(x);

}

void gslu_set_zero_matrix(gsl_matrix *x){

  int i, j;
  for(i=0; i<x->size1; i++)
    for(j=0; j<x->size2; j++)
      gsl_matrix_set(x, i, j, 0.0);

}

gsl_matrix *gslu_gen_zero_matrix(int row, int col){

  gsl_matrix *x = gsl_matrix_alloc(row, col);
  gslu_set_zero_matrix(x);
  return(x);

}

 void gslu_translate(gsl_matrix *w, double x, double y, double z){

   int i;

  if(w->size1 != 4|| w->size2 != 4)
    {
      printf("gsl_utils error: gslu_translate:\n matrix is incorrect size\n");
      return;
    }

  gsl_matrix_set(w, 0, 3, x);
  gsl_matrix_set(w, 1, 3, y);
  gsl_matrix_set(w, 2, 3, z);

  /* if(g_foo == 0) */
  /*   { */
  /*     printf("translate:\n"); */
  /*     show_matrix(w); */
  /*     printf("---------\n"); */
  /*   } */
 }

void gslu_rotate(gsl_matrix *x, double angle, char axis){

  double cos_a, sin_a, minus_sin_a;
  cos_a = cosf(angle);
  sin_a = sinf(angle);
  minus_sin_a = -1.0*sin_a;

  if(x->size1 != 4|| x->size2 != 4)
    {
      printf("gsl_utils error: gslu_rotate:\n matrix is incorrect size\n");
      return;
    }


  if(axis == 'x')
    {
      gsl_matrix_set(x, 1, 1, cos_a);
      gsl_matrix_set(x, 2, 1, minus_sin_a);
      gsl_matrix_set(x, 1, 2, sin_a);
      gsl_matrix_set(x, 2, 2, cos_a);
    }

  else if(axis == 'y')
    {
      gsl_matrix_set(x, 2, 2, cos_a);
      gsl_matrix_set(x, 2, 0, minus_sin_a);
      gsl_matrix_set(x, 0, 2, sin_a);
      gsl_matrix_set(x, 0, 0, cos_a);
    }

  else if(axis == 'z')
    {
      gsl_matrix_set(x, 0, 0, cos_a);
      gsl_matrix_set(x, 0, 1, minus_sin_a);
      gsl_matrix_set(x, 1, 0, sin_a);
      gsl_matrix_set(x, 1, 1, cos_a);
    }

  else 
    printf("gsl_utils error: gslu_rotate\n incorrect axis selection : %c\n", axis);

}

static double inner_cp(double a, double b, double c, double d){

  double x;
  double val1, val2;
  val1 = a*d;
  val2 = b*c;
  x = val1 - val2; 

  return(x);
}

void gslu_cross_product(double *x, double *y, double *v){

  v[0] = inner_cp (x[1], x[2], y[1], y[2]);
  v[1] = inner_cp (x[2], x[0], y[2], y[0]);
  v[2] = inner_cp (x[0], x[1], y[0], y[1]);

}

double gslu_euclid_norm_3d(double *x){

  int i;
  double en = 0;

  for(i=0; i<3; i++)
    en += (x[i] * x[i]);
  en = sqrt(en);

  return(en);

}

void gslu_look_at(gsl_matrix *x,
		  gsl_matrix *dummy1,//needed for final step
		  gsl_matrix *dummy2,//needed for final step
		  double *eye,
		  double *center,
		  double *up){
  
  int i;
  double F[3];
  double s[3], u[3];

  if(x->size1 != 4|| x->size2 != 4)
    {
      printf("gsl_utils error: gslu_look_at:\n matrix is incorrect size\n");
      return;
    }

  if(dummy1->size1 != 4|| dummy1->size2 != 4)
    {
      printf("gsl_utils error: gslu_look_at:\n dummmy1 (arg2) matrix is incorrect size\n");
      return;
    }

  if(dummy2->size1 != 4|| dummy2->size2 != 4)
    {
      printf("gsl_utils error: gslu_look_at:\n dummmy2 (arg3) matrix is incorrect size\n");
      return;
    }

  gslu_set_zero_matrix(x);
  gslu_set_zero_matrix(dummy1);
  gslu_set_id_matrix(dummy2);
  
  //find F
  for(i=0; i<3; i++)
    F[i] = center[i] - eye[i];

  //normalize F
  double norm = gslu_euclid_norm_3d(F);
  for(i=0; i<3; i++)
    F[i]/=norm;

  //normalize UP
  norm = gslu_euclid_norm_3d(up);
  for(i=0; i<3; i++)
    up[i]/=norm;
  
  //find s
  gslu_cross_product(F, up, s);
  norm = gslu_euclid_norm_3d(s);
  for(i=0; i<3; i++)
    s[i]/=norm;

  //find u
  gslu_cross_product(s, F, u);

  //pack the results into the matrix
  gsl_matrix_set(dummy1, 0, 0, s[0]);
  gsl_matrix_set(dummy1, 0, 1, s[1]);
  gsl_matrix_set(dummy1, 0, 2, s[2]);

  gsl_matrix_set(dummy1, 1, 0, u[0]);
  gsl_matrix_set(dummy1, 1, 1, u[1]);
  gsl_matrix_set(dummy1, 1, 2, u[2]);

  gsl_matrix_set(dummy1, 2, 0, -1.0 * F[0]);
  gsl_matrix_set(dummy1, 2, 1, -1.0 * F[1]);
  gsl_matrix_set(dummy1, 2, 2, -1.0 * F[2]);

  gsl_matrix_set(dummy1, 3, 3, 1.0);

  //final translation step
  gsl_matrix_set(dummy2, 0, 3, -1.0 * eye[0]);
  gsl_matrix_set(dummy2, 1, 3, -1.0 * eye[1]);
  gsl_matrix_set(dummy2, 2, 3, -1.0 * eye[2]);

  gslu_mult_matrices(x, dummy1, dummy2);

}

void gslu_perspective(gsl_matrix *x, 
		      double fovy,
		      double aspect,
		      double zNear,
		      double zFar){

  double f;
  double two_two, two_three;

  if(x->size1 != 4|| x->size2 != 4)
    {
      printf("gsl_utils error: gslu_perspective:\n matrix is incorrect size\n");
      return;
    }

  //zero out the matrix
  gslu_set_zero_matrix(x);

  f = 1/(tanf( (fovy * .5) ));
  two_two = (zNear + zFar) / (zNear - zFar);
  two_three = (2 * zNear * zFar) / (zNear - zFar);
  
  //pack the matrix
  gsl_matrix_set(x, 0, 0, (f/aspect));
  gsl_matrix_set(x, 1, 1, f);
  gsl_matrix_set(x, 2, 2, two_two);
  gsl_matrix_set(x, 2, 3, two_three);
  gsl_matrix_set(x, 3, 2, -1);

  /* if(g_foo == 0) */
  /*   show_matrix(x); */
  /* g_foo = 1; */
}
