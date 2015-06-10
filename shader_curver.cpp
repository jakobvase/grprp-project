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

vector<glm::vec3> vertices; // The list of vertices of the object.
vector<triangle> triangles; // The list of faces of the object.
vector<glm::vec3> normals; 	// The list of normals on the object

int t; // A timer

GLuint curves_program; 		// Integer for the program
GLuint curves_vertex; 		// Integer for the vertex shader
GLuint curves_fragment; 	// Integer for the fragment shader
GLuint curves_tesscontrol; 	// Integer for the tesselation control shader
GLuint curves_tesseval; 	// Integer for the tesselation evaluator

glm::mat4 matrix_mv; 		// Model-view matrix
glm::mat4 matrix_mvp; 		// Model-view-projection matrix
glm::vec3 light_position; 	// Position of the light in the space
glm::vec3 light_color; 		// Color of the light in the space

GLuint shader_mv; 			// Integers for the shaders (???)
GLuint shader_mvp;
GLuint shader_lpos;
GLuint shader_lcolor;
GLuint shader_normal;

GLuint tess_inner; 			// Controllers. Inner tesselation level.
GLuint tess_outer;			// Outer tesselation level.
GLuint tangent_length;		// Length of the tangent.
int t_inner;				// Local inner tesselation level
int t_outer; 				// Local outer tesselation level
float tan_length;			// Length of the tangent locally.
bool rotate; 				// Local rotation switch.
float angle;				// Local angle for the first vertex of the object - to be randomised.
glm::vec3 position; 		// The position of the object (???)
glm::vec3 direction; 		// The facing of the object (???)

/* Vectors and arrays for loading the shader source code. */
vector<char> fragment_source;
vector<char> vertex_source;
vector<char> tesscontrol_source;
vector<char> tesseval_source;
const char *vertex_source_pointer;
const char *fragment_source_pointer;
const char *tesscontrol_source_pointer;
const char *tesseval_source_pointer;
/**/

// Draw the object to the screen.
void draw_obj(void)
{
  glm::vec3 v1, v2, v3, n1, n2, n3; // Vectors and normals of face
  glm::vec3 n; 						// Face normal.

  if (rotate) { // Check whether we should rotate object
    angle += (clock() - t) / 1e6f; // If yes, add tickes since last draw divided by 1000000. (???)
  }
  t = clock(); // Keep track of the time, so it doesn't run away from us.

  // (???)
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
    // we pass them as attributes, and use them as normals in our shaders
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
