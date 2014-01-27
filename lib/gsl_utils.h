///////////////////////gsl_utils.h////////////////////////
/* A set of functions for using matrices in pure C.     */

/* These utilities use the GNU scientific library       */
/* and are intended as utilities for doing matrix       */
/* manipulations for openGL 3D vecotr graphics          */
/* programs.                                            */

/* The idea is to replace the commonly used gsm library */
/* of matrix utilities which are in C++.                */
//////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_blas.h>

void gslu_show_matrix(gsl_matrix *x);

void gslu_gsl_matrix_2_float_array(float *matrix, gsl_matrix *x);

void gslu_mult_matrices(gsl_matrix *C,
			gsl_matrix *A,
			gsl_matrix *B);
//compute C = AB (no fancy scalars for now)

void gslu_mult_matrix_vector(gsl_matrix *x, double *a);

void gslu_set_id_matrix(gsl_matrix *x);
//set a matrix to identity matrix

gsl_matrix *gslu_gen_id_matrix(int row, int col);
//create an identity matrix, row x col

void gslu_set_zero_matrix(gsl_matrix *x);
//zero out a matrix

gsl_matrix *gslu_gen_zero_matrix(int row, int col);
//create a zero matrix, row x column

void gslu_translate(gsl_matrix *w, double x, double y, double z);
//make identity matrix w translate in 3d according to amounts x, y, and z

void gslu_rotate(gsl_matrix *x, double angle, char axis);
//set a rotation matrix that will rotate a 4D matrix
//by angle about axis x, y, or z
//it will do nothing if the third arg is not 'x', 'y', or 'z'

void gslu_trans_scale(gsl_matrix *x, double *translate, double *scaler);
//translate a matrix x by vector translate and scale
//it by vector scaler
//translate and vector are 3 dimensional
//this is (sorta) equivalent to the glm call
//glm::x = scale(glm::translate(glm::mat4 y(1.0), translate), scaler); 

void gslu_cross_product(double *x, double *y, double *v);
//returns the cross_product of 2 3d vectors x and y

double gslu_euclid_norm_3d(double *x);
//returns the euclidean norm of a 3d vector x

void gslu_look_at(gsl_matrix *x, 
		  gsl_matrix *dummy1,
		  gsl_matrix *dummy2,
		  double *eye, 
		  double *center, 
		  double *up);
//set a viewing matrix a la gluLookAt: 
//http://www.opengl.org/sdk/docs/man2/xhtml/gluLookAt.xml
//x is an identity matrix (will be bashed anyway)
//each ptr to double should be a 1x3 array corresponding to
// eye, center, and UP in the doc


 void gslu_perspective(gsl_matrix *x, 
		       double fovy,
		       double aspect,
		       double zNear,
		       double zFar);
//set a perspective matrix a la gluPerspective:
//http://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml
//see doc for details

void gslu_viewport_transform(gsl_matrix *w, 
			     float x, float y,
			     float width, float height,
			     float window_width, float window_height);
//transform that mimics glViewport
//the extra arguments are a matrix to store
//the resulting transform in
//the point is to have a transformation
//normaized to +-1 in order to easily draw
//frames about a particular area of the gl window
//user must provide absolute window width and height
//window size
//this code was copied from the openGL tutorial here:
//http://en.wikibooks.org/wiki/OpenGL_Programming/Scientific_OpenGL_Tutorial_03
