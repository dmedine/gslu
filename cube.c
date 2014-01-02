#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include "shader_utils.h"
#include "gsl_utils.h"

GLuint program;
GLuint attribute_coord3d, attribute_v_color;
GLint vbo_cube_v, vbo_cube_c;
GLint ibo_cube_elements;
GLint uniform_m_transform;

int screen_width = 800;
int screen_height = 600;
static int g_foo = 0;
//gsl globals

gsl_matrix *mx_translate;//position
gsl_matrix *mx_rotate;//rotation
gsl_matrix *mx_model;//rotation x position
gsl_matrix *mx_view;
gsl_matrix *dummy1, *dummy2, *prod1, *prod2, *prod3;
gsl_matrix *mx_projection;

int init_resources(void){

  //setup the shader programs
  GLuint vs, fs;
  GLint link_ok = GL_FALSE;
 
  GLfloat cube_v[] = {
    // front
    -0.5, -0.5,  0.5,
     0.5, -0.5,  0.5,
     0.5,  0.5,  0.5,
    -0.5,  0.5,  0.5,
    // back
    -0.5, -0.5, -0.5,
     0.5, -0.5, -0.5,
     0.5,  0.5, -0.5,
    -0.5,  0.5, -0.5,
  };

  glGenBuffers(1, &vbo_cube_v);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_v);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_v), cube_v, GL_STATIC_DRAW);


  GLfloat cube_c[] = {
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
    // back colors
    1.0, 0.0, 0.0,
    0.0, 1.0, 0.0,
    0.0, 0.0, 1.0,
    1.0, 1.0, 1.0,
  };

  glGenBuffers(1, &vbo_cube_c);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_c);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cube_c), cube_c, GL_STATIC_DRAW);


  GLushort cube_elements[] = {
    //front
    0, 1, 2,
    2, 3, 0,
    //top
    2, 3, 6,
    6, 7, 2,
    //back
    7, 6, 5,
    5, 4, 7,
    // bottom
    4, 5, 1,
    1, 0, 4,
    // left
    4, 0, 3,
    3, 7, 4,
    // right
    1, 5, 6,
    6, 2, 1,
  };

  glGenBuffers(1, &ibo_cube_elements);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

  if ((vs = tut_create_shader("cube.v.glsl", GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = tut_create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);

  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(program);
    return(0);
  }
    
  const char *attribute_name = "coord3d";
  attribute_coord3d = glGetAttribLocation(program, attribute_name);
  if(attribute_coord3d == -1)
    {
      fprintf(stderr, "Error, could not bind attribute %s\n", attribute_name);
      return(0);
    }

  attribute_name = "v_color";
  attribute_v_color = glGetAttribLocation(program, attribute_name);
  if(attribute_v_color == -1)
    {
      fprintf(stderr, "Error, could not bind attribute %s\n", attribute_name);
      return 0;
    }

  const char *uniform_name = "m_transform";
  uniform_m_transform =  glGetUniformLocation(program, uniform_name);
  if(uniform_m_transform == -1)
    {
      fprintf(stderr, "Error, could not bind uniform %s\n", uniform_name);
      return 0 ;
    }

  mx_translate       = gslu_gen_id_matrix(4,4);
  mx_rotate          = gslu_gen_id_matrix(4,4);
  mx_model           = gslu_gen_id_matrix(4,4);
  mx_view            = gslu_gen_id_matrix(4,4);
  dummy1             = gslu_gen_id_matrix(4,4);
  dummy2             = gslu_gen_id_matrix(4,4);
  prod1              = gslu_gen_zero_matrix(4,4);
  prod2              = gslu_gen_zero_matrix(4,4);
  prod3              = gslu_gen_zero_matrix(4,4);
  mx_projection      = gslu_gen_id_matrix(4,4);


  return(1);

}



void onDisplay(){

  int size;
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glEnable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glUseProgram(program);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_v);
  glEnableVertexAttribArray(attribute_coord3d);
 
  /* Describe our vertices array to OpenGL (it can't guess its format automatically) */
  glVertexAttribPointer(
    attribute_coord3d,           // attribute
    3,                           // number of elements per vertex, here (x,y, and z)
    GL_FLOAT,                    // the type of each element
    GL_FALSE,                    // take our values as-is
    0,   // that much data between each position
    0);                          // initial offset
  

  glEnableVertexAttribArray(attribute_v_color);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_c);
  glVertexAttribPointer(
    attribute_v_color, // attribute
    3,                 // number of elements per vertex, here (x,y, and z)
    GL_FLOAT,          // the type of each element
    GL_FALSE,          // take our values as-is
    0,                 // that much data between each position
    0);                // initial offset
			

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_cube_elements);
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_v_color);

  glutSwapBuffers();

}

void tut_idle(){

  GLfloat mx[16];
  float move = sinf(glutGet(GLUT_ELAPSED_TIME)/1000.0 * (2*M_PI) / 10); //between -1 and 1 each 5 seconds
  float angle = glutGet(GLUT_ELAPSED_TIME)/ 1000.0 * (M_PI/4); //45 degrees everey second
  float up_x_coord = sinf(glutGet(GLUT_ELAPSED_TIME)/ 1000.0 * (2*M_PI) / 5); //45 degrees everey second

  float angle2 = (50.0/180.0) * M_PI; 

  double l_eye[]    = {1.0,  move,  0.0};
  double l_center[] = {0.0,  0.0, -4.0};
  double l_up[]     = {0.0,  1.0,  0.0};

  gslu_look_at(mx_view, dummy1, dummy2,
	       l_eye, l_center, l_up);

  gslu_perspective(mx_projection, 
		   M_PI/4.0, 
		   4.0/3.0, 
		   0.1, 
		   10.0);

  gslu_translate(mx_translate, 0.0, 0.0, (move-1)* 5);
  gslu_rotate(mx_rotate, angle, 'y');

  gslu_mult_matrices(prod1, 
		     mx_translate, 
		     mx_rotate);

  gslu_mult_matrices(prod2, 
		     mx_view, 
		     prod1);

  gslu_mult_matrices(prod3, 
		     mx_projection, 
		     prod2);

  gslu_gsl_matrix_2_float_array(mx, prod3);

  int i;
  if(g_foo == 0)
    for(i=0; i<16; i++)
      printf("%d %f\n", i, mx[i]);
  g_foo = 1;

  glUseProgram(program);
  glUniformMatrix4fv(uniform_m_transform, 1, GL_FALSE, mx);
  glutPostRedisplay();

}

void onReshape(int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, screen_width, screen_height);
}

void free_resources(){

  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_v);
  glDeleteBuffers(1, &vbo_cube_c);
  glDeleteBuffers(1, &ibo_cube_elements);

  //free our gsl objects
  gsl_matrix_free(mx_translate);
  gsl_matrix_free(mx_rotate);
  gsl_matrix_free(mx_model);
  gsl_matrix_free(mx_projection);
  gsl_matrix_free(mx_view);
  gsl_matrix_free(dummy1);
  gsl_matrix_free(dummy2);
  gsl_matrix_free(prod1);
  gsl_matrix_free(prod2);
  gsl_matrix_free(prod3);


}


int main(int argc, char **argv){

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);

  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow("Cube");

  GLenum glew_status = glewInit();
  if(glew_status != GLEW_OK)
    {
      fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
      return(EXIT_FAILURE);
    }

  if(1 == init_resources())
    {
      glutDisplayFunc(onDisplay);
      //glutReshapeFunc(onReshape);
      glutIdleFunc(tut_idle);
      glutReshapeFunc(onReshape); 
      glEnable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      //glDepthFunc(GL_LESS);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glutMainLoop();

    }


  free_resources();
  return(EXIT_SUCCESS);
}
