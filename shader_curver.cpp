#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>
#include <cmath>
#include <time.h>
#include <cstdio>
#include <stdlib.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifdef __linux__
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#else
#include <GL/glew.h>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <OpenGL/glext.h>
#endif

#include "vertex.cpp"
#include "file_functions.cpp"

using namespace std;
using namespace vertex_math;

#define LINE_SIZE (256)

vector<glm::vec3> vertices;
vector<triangle> triangles;
vector<glm::vec3> normals;

int t;

GLuint curves_program;
GLuint curves_vertex;
GLuint curves_fragment;
GLuint curves_tesscontrol;
GLuint curves_tesseval;

glm::mat4 matrix_mv;
glm::mat4 matrix_mvp;
glm::vec3 light_position;
glm::vec3 light_color;
GLuint shader_mv;
GLuint shader_mvp;
GLuint shader_lpos;
GLuint shader_lcolor;
GLuint shader_normal;

/**/
vector<char> fragment_source;
vector<char> vertex_source;
vector<char> tesscontrol_source;
vector<char> tesseval_source;
const char *vertex_source_pointer;
const char *fragment_source_pointer;
const char *tesscontrol_source_pointer;
const char *tesseval_source_pointer;
/**/

void curve_object(int count) {
  if(count <= 0) return;
  glm::vec3 v1, v2, v3, n1, n2, n3, b1, b2, b3, bn1, bn2, bn3;
  int i1, i2, i3, in1, in2, in3, o1, o2, o3, on1, on2, on3;
  triangle t1, t2, t3, t4;
  vector<triangle> new_triangles;

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    o1 = triangles.at(i).i1;
    o2 = triangles.at(i).i2;
    o3 = triangles.at(i).i3;
    on1 = triangles.at(i).n1;
    on2 = triangles.at(i).n2;
    on3 = triangles.at(i).n3;

    v1 = vertices.at(o1);
    v2 = vertices.at(o2);
    v3 = vertices.at(o3);
    n1 = normals.at(on1);
    n2 = normals.at(on2);
    n3 = normals.at(on3);

    bezier(v1, v2, n1, n2, b1, bn1);
    bezier(v2, v3, n2, n3, b2, bn2);
    bezier(v3, v1, n3, n1, b3, bn3);

    i1 = vertices.size();
    i2 = i1 + 1;
    i3 = i2 + 1;
    in1 = normals.size();
    in2 = in1 + 1;
    in3 = in2 + 1;

    vertices.push_back(b1);
    vertices.push_back(b2);
    vertices.push_back(b3);
    normals.push_back(bn1);
    normals.push_back(bn2);
    normals.push_back(bn3);

    t1 = createTriangle(o1, i1, i3, on1, in1, in3);
    t2 = createTriangle(o2, i2, i1, on2, in2, in1);
    t3 = createTriangle(o3, i3, i2, on3, in3, in2);
    t4 = createTriangle(i1, i2, i3, in1, in2, in3);

    new_triangles.push_back(t1);
    new_triangles.push_back(t2);
    new_triangles.push_back(t3);
    new_triangles.push_back(t4);
  }

  triangles = new_triangles;
  curve_object(--count);
}

void draw_obj(void)
{
  glm::vec3 v1, v2, v3, n1, n2, n3;
  glm::vec3 n;


  glPushMatrix();
  glRotatef((clock() - t) / 5e4, 0.0, 1.0, 0.0);
  glm::mat4 rotated = glm::rotate(matrix_mv, (clock() - t) / 5e4f, glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 projection = matrix_mvp * rotated;

  glUniformMatrix4fv(shader_mv, 1, GL_FALSE, &rotated[0][0]);
  glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, &projection[0][0]);
  glUniform3f(shader_lpos, light_position.x, light_position.y, light_position.z);
  glUniform3f(shader_lcolor, light_color.x, light_color.y, light_color.z);

  glPatchParameteri(GL_PATCH_VERTICES, 3);

  glBegin(GL_PATCHES);

  //cout << "drawing " << triangles.size() << " tris\n";

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    v1 = vertices.at(triangles.at(i).i1);
    v2 = vertices.at(triangles.at(i).i2);
    v3 = vertices.at(triangles.at(i).i3);
    n1 = normals.at(triangles.at(i).n1);
    n2 = normals.at(triangles.at(i).n2);
    n3 = normals.at(triangles.at(i).n3);

    //n = (n1 + n2 + n3) * 0.3333f;

    // Draw this triangle.
    /**
    glNormal3f(n.x, n.y, n.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    /**/
    //glNormal3fv(&n[0]);
    glVertexAttrib3fv(shader_normal, &n1[0]);
    glVertex3fv(&v1[0]);
    glVertexAttrib3fv(shader_normal, &n2[0]);
    glVertex3fv(&v2[0]);
    glVertexAttrib3fv(shader_normal, &n3[0]);
    glVertex3fv(&v3[0]);
    /**
    glNormal3f(n1.x, n1.y, n1.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glNormal3f(n2.x, n2.y, n2.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glNormal3f(n3.x, n3.y, n3.z);
    glVertex3f(v3.x, v3.y, v3.z);
    /**/
  }

  glEnd();
  glPopMatrix();
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_obj();
  glutSwapBuffers();
  glutPostRedisplay();
}

void init_scene(void)
{
  //glUseProgram(0);

  /**/
  light_position = glm::vec3(1.0f, 4.0f, 2.0f);
  light_color = glm::vec3(1.0f, 0.0f, 0.0f);

  // Configure a light.
  GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  // Red diffuse light.
  GLfloat light_position[] = {1.0, 4.0, 2.0, 0.0};  // Infinite light location.
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  /**/

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  matrix_mvp = glm::perspective(40.0f, 1.0f, 1.0f, 10.0f);
  matrix_mv = glm::lookAt(glm::vec3(0.0f, 2.0f, 5.0f),
    glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

  // Configure viewing.  (Assumes object is within [-1,1] in all axes.)
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 2.0, 5.0,  // Set eye position, target position, and up direction.
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0);

  // Rotate object.
  glRotatef(30, 0.0, 1.0, 0.0);


}

static void show_info_log(GLuint object,
  PFNGLGETSHADERIVPROC glGet__iv,
  PFNGLGETSHADERINFOLOGPROC glGet__InfoLog) {
  GLint log_length;
  char *log;

  glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
  log = (char*)malloc(log_length);
  glGet__InfoLog(object, log_length, NULL, log);
  fprintf(stderr, "%s", log);
  free(log);
}

static void verify_shader(GLuint shader, char *name) {
  GLint shader_ok;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
  if (!shader_ok) {
    fprintf(stderr, "Failed to compile %s:\n", name);
    show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
    //glDeleteShader(shader);
  }
}

void initShaders() {

  /**/
  curves_program = glCreateProgram();
  /**/
  curves_vertex = glCreateShader(GL_VERTEX_SHADER);
  curves_fragment = glCreateShader(GL_FRAGMENT_SHADER);
  curves_tesscontrol = glCreateShader(GL_TESS_CONTROL_SHADER);
  curves_tesseval = glCreateShader(GL_TESS_EVALUATION_SHADER);

  /*GLint length;
  GLchar *source = file_contents("vertex.glsl", &length);
  cout << source << "\n";*/

  vertex_source = readFileToCharVector("vertex.glsl");
  fragment_source = readFileToCharVector("fragment.glsl");
  tesscontrol_source = readFileToCharVector("tesscontrol.glsl");
  tesseval_source = readFileToCharVector("tesseval.glsl");
  vertex_source_pointer = &vertex_source[0];
  fragment_source_pointer = &fragment_source[0];
  tesscontrol_source_pointer = &tesscontrol_source[0];
  tesseval_source_pointer = &tesseval_source[0];

  /*cout << vertex_source_pointer << "\n";
  cout << fragment_source_pointer << "\n";*/

  glShaderSource(curves_vertex, 1, &vertex_source_pointer, NULL);
  glShaderSource(curves_fragment, 1, &fragment_source_pointer, NULL);
  glShaderSource(curves_tesscontrol, 1, &tesscontrol_source_pointer, NULL);
  glShaderSource(curves_tesseval, 1, &tesseval_source_pointer, NULL);

  glCompileShader(curves_vertex);
  verify_shader(curves_vertex, "vertex");
  
  glCompileShader(curves_fragment);
  verify_shader(curves_fragment, "fragment");

  glCompileShader(curves_tesscontrol);
  verify_shader(curves_tesscontrol, "tesscontrol");

  glCompileShader(curves_tesseval);
  verify_shader(curves_tesseval, "tesseval");

  glAttachShader(curves_program, curves_vertex);
  glAttachShader(curves_program, curves_fragment);
  glAttachShader(curves_program, curves_tesscontrol);
  glAttachShader(curves_program, curves_tesseval);
  glLinkProgram(curves_program);

  GLint prog_link_success;
  glGetObjectParameterivARB(curves_program, GL_OBJECT_LINK_STATUS_ARB, &prog_link_success);
  if (!prog_link_success) {
    fprintf(stderr, "The shaders could not be linked\n");
    exit(1);
  }

  GLint MaxPatchVertices = 0;
  glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
  printf("Max supported patch vertices %d\n", MaxPatchVertices);  
  glPatchParameteri(GL_PATCH_VERTICES, 3);

  /**/

  glUseProgram(curves_program);

  shader_mv = glGetUniformLocation(curves_program, "MVMatrix");
  shader_mvp = glGetUniformLocation(curves_program, "MVPMatrix");
  shader_lcolor = glGetUniformLocation(curves_program, "LightColor");
  shader_lpos = glGetUniformLocation(curves_program, "LightPosition");
  shader_normal = glGetAttribLocation(curves_program, "Normal");

}

int main(int argc, char **argv)
{
  // Check for proper arguments.
  if (argc < 3) {
    cout << "usage: " << argv[0] << " <obj_filename> <iterations>" << endl;
    exit(0);
  }

  // Read obj file given as argument.
  read_obj_file(argv[1], vertices, normals, triangles);

  cout << "t" << triangles.size() << "v" << vertices.size() << "n" << normals.size() << "\n";

  // Curve it!
  curve_object(stoi(argv[2]));
  
  // Set up glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Shader Curver");

  /**/
  GLenum err = glewInit();
  if (err != GLEW_OK)
    exit(1); // or handle the error in a nicer way
  if (!GLEW_VERSION_2_1)  // check that the machine supports the 2.1 API.
    exit(1); // or handle the error in a nicer way

  initShaders();
  /**/

  glutDisplayFunc(display);

  // Initialize scene.
  init_scene();

  t = clock();

  // Hand control over to glut's main loop.
  glutMainLoop();
}
