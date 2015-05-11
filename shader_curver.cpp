#include <vector>
#include <iostream>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"

#ifdef __linux__
#include <GL/glew.h>
#include <GL/glut.h>
#else
#include <GL/glew.h>
#include <GLUT/glut.h>
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

GLuint tess_inner;
GLuint tess_outer;
GLuint tangent_length;
int t_inner;
int t_outer;
float tan_length;
bool rotate;
float angle;
glm::vec3 position;
glm::vec3 direction;

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

  if (rotate) {
    angle += (clock() - t) / 1e6f;
  }
  t = clock(); 

  // rotate modelview (similar to glRotate)
  glm::mat4 rotated = glm::rotate(matrix_mv, angle, glm::vec3(0.0f, 1.0f, 0.0f));
  rotated = glm::rotate(rotated, angle / 10, glm::vec3(1.0f, 0.0f, 0.0f));
  // calculate projection matrix
  glm::mat4 projection = matrix_mvp * rotated;
  // pass matrices to the shaders
  glUniformMatrix4fv(shader_mv, 1, GL_FALSE, &rotated[0][0]);
  glUniformMatrix4fv(shader_mvp, 1, GL_FALSE, &projection[0][0]);
  
  // pass light to the shaders
  glUniform3f(shader_lpos, light_position.x, light_position.y, light_position.z);
  glUniform3f(shader_lcolor, light_color.x, light_color.y, light_color.z);

  // pass tesselation info to the shaders
  glUniform1f(tess_inner, t_inner);
  glUniform1f(tess_outer, t_outer);
  glUniform1f(tangent_length, tan_length);

  // using triangle patches (triangles with tesselation)
  glPatchParameteri(GL_PATCH_VERTICES, 3);
  glBegin(GL_PATCHES);

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    v1 = vertices.at(triangles.at(i).i1);
    v2 = vertices.at(triangles.at(i).i2);
    v3 = vertices.at(triangles.at(i).i3);
    n1 = normals.at(triangles.at(i).n1);
    n2 = normals.at(triangles.at(i).n2);
    n3 = normals.at(triangles.at(i).n3);

    // normals are not a fixed part of the GLSL 1.4+ pipeline
    // we pass them as atrributes, and use them as normals in our shaders
    glVertexAttrib3fv(shader_normal, &n1[0]);
    glVertex3fv(&v1[0]);
    glVertexAttrib3fv(shader_normal, &n2[0]);
    glVertex3fv(&v2[0]);
    glVertexAttrib3fv(shader_normal, &n3[0]);
    glVertex3fv(&v3[0]);
  }

  glEnd();
}

void display(void)
{
  // clear, draw, flip and tell glut to repaint again
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_obj();
  glutSwapBuffers();
  glutPostRedisplay();
}

void init_scene(void)
{
  /* Light is just vectors now, we manually pass them to the shaders
      and calculate the values as diffuse light */
  light_position = glm::vec3(1.0f, 4.0f, 2.0f);
  light_color = glm::vec3(1.0f, 0.0f, 0.0f);
  /**/

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  // setup the projection and model view matrices, using GLM's glut helper functions
  matrix_mvp = glm::perspective(0.7f, 1.0f, 1.0f, 10.0f);
  matrix_mv = glm::lookAt(position,
    position + direction,
    glm::vec3(0.0f, 1.0f, 0.0f));
}

// Code to verify GLSL version and shaders:
// show_info_log and verify_shaders are based on code copied from
// http://voxels.blogspot.dk/2011/09/tesselation-shader-tutorial-with-source.html
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

// load, compile and attach shaders
void initShaders() {

  /**/
  curves_program = glCreateProgram();
  /**/
  curves_vertex = glCreateShader(GL_VERTEX_SHADER);
  curves_fragment = glCreateShader(GL_FRAGMENT_SHADER);
  curves_tesscontrol = glCreateShader(GL_TESS_CONTROL_SHADER);
  curves_tesseval = glCreateShader(GL_TESS_EVALUATION_SHADER);

  // Load all shaders from files
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

  // Add, compile and verify all shaders
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

  // Attach all shaders to the program
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

  // Activate the program as the active rendering pipeline

  glUseProgram(curves_program);

  // Get uniform locations of all elements we need to set from this program
  shader_mv = glGetUniformLocation(curves_program, "MVMatrix");
  shader_mvp = glGetUniformLocation(curves_program, "MVPMatrix");
  shader_lcolor = glGetUniformLocation(curves_program, "LightColor");
  shader_lpos = glGetUniformLocation(curves_program, "LightPosition");

  tess_inner = glGetUniformLocation(curves_program, "TessLevelInner");
  tess_outer = glGetUniformLocation(curves_program, "TessLevelOuter");
  tangent_length = glGetUniformLocation(curves_program, "TangentLength");

  shader_normal = glGetAttribLocation(curves_program, "Normal");

}

// Listen for keyboard input
void keyboard_input(unsigned char key, int x, int y) {
  switch (key) {
    // i-k adjust inner tessellation
    case 'i':
      t_inner++;
      break;
    case 'k':
      if (t_inner > 1) t_inner--;
      break;
    // o-l adjust outer tessellation
    case 'o':
      t_outer++;
      break;
    case 'l':
      if (t_outer > 1) t_outer--;
      break;
    // t-g adjust tangent length
    case 't':
      tan_length += 0.1f;
      break;
    case 'g':
      tan_length -= 0.1f;
      break;
    // n set normal of first vertex to a random value
    case 'n':
      normals[0] = glm::sphericalRand(1.0f);
      break;
    // r pause rotation
    case 'r':
      rotate = !rotate;
      break;
    // wasd move camera
    case 'w':
      position.y += 0.1f;
      break;
    case 's':
      position.y -= 0.1f;
      break;
    case 'd':
      position.x += 0.1f;
      break;
    case 'a':
      position.x -= 0.1f;
      break;
  }


  matrix_mv = glm::lookAt(position,
    position + direction,
    glm::vec3(0.0f, 1.0f, 0.0f));

  cout << "t_inner " << t_inner << " t_outer " << t_outer << " tangent " << tan_length << "\n";
}

int lastX, lastY;
bool firstMouse = true;
void mouse(int x, int y) {

  if (firstMouse) {
    firstMouse = false;
    lastX = x;
    lastY = y;
  }

  direction.x -= (x - lastX) / 100.0f;
  direction.y += (y - lastY) / 100.0f;

  lastX = x;
  lastY = y;

  matrix_mv = glm::lookAt(position,
    position + direction,
    glm::vec3(0.0f, 1.0f, 0.0f));
}

int main(int argc, char **argv)
{
  // Check for proper arguments.
  if (argc < 4) {
    cout << "usage: " << argv[0] << " <obj_filename> <tess_inner> <tess_outer>" << endl;
    exit(0);
  }

  // Read obj file given as argument.
  read_obj_file(argv[1], vertices, normals, triangles);

  cout << "t" << triangles.size() << "v" << vertices.size() << "n" << normals.size() << "\n";

  // Curve it!
  //curve_object(stoi(argv[2]));

  t_inner = stoi(argv[2]);
  t_outer = stoi(argv[3]);
  tan_length = 0.35f;
  rotate = true;

  position = glm::vec3(0.0f, 2.0f, 5.0f);
  direction = glm::vec3(0.0f, -2.0f, -5.0f);
  
  // Set up glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  //glutInitWindowSize(500, 500);
  glutCreateWindow("Shader Curver");


  /* Initialize glew, to access shaders */
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

  // we use the clock to rotate the object
  t = clock();

  // Let glut handle keyboard and mouse input
  glutKeyboardFunc(keyboard_input);
  glutMotionFunc(mouse);
  // Hand control over to glut's main loop.
  glutMainLoop();
}
