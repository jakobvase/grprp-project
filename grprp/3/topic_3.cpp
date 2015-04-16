#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>
#include <cmath>
#include <GL/glut.h>

using namespace std;

#define LINE_SIZE (256)

struct vertex {
  double x;
  double y;
  double z;
};
struct triangle {
  int i1;
  int i2;
  int i3;
};

vector<vertex> vertices;
vector<triangle> triangles;

// Assumes only triangles, and that file contains only vertex and face lines.
void read_obj_file(const char* filename)
{
  ifstream ifs;
  string first_word;
  vertex new_vertex;
  triangle new_triangle;

  // Open file.
  ifs.open(filename);

  // Process file, one line at a time.
  do {
    // Start a line, and process it if the file's not empty.
    first_word.clear();  // (Then if line is empty, first_word won't persist from last line.)
    ifs >> first_word;
    if (ifs.eof() == false) {
      if (first_word == "v") {
	// Read a vertex line.
	ifs >> new_vertex.x >> new_vertex.y >> new_vertex.z;
	vertices.push_back(new_vertex);
      }
      else if (first_word == "f") {
	// Read a face line.
	ifs >> new_triangle.i1 >> new_triangle.i2 >> new_triangle.i3;
	// Decrement indices from [1,n] in obj file to [0,n-1] in vertex vector.
	--new_triangle.i1;
	--new_triangle.i2;
	--new_triangle.i3;
	triangles.push_back(new_triangle);
      }
      // Get rid of anything left on this line (including the newline).
      ifs.ignore(numeric_limits<streamsize>::max(), '\n');
    }
  } while (ifs.eof() == false);

  // Close file.
  ifs.close();
}

void draw_obj(void)
{
  vertex v1, v2, v3;
  vertex e1, e2;
  vertex n;

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    v1 = vertices.at(triangles.at(i).i1);
    v2 = vertices.at(triangles.at(i).i2);
    v3 = vertices.at(triangles.at(i).i3);

    // e1 is edge from v1 to v2.
    e1.x = v2.x - v1.x;
    e1.y = v2.y - v1.y;
    e1.z = v2.z - v1.z;

    // e2 is edge from v1 to v3.
    e2.x = v3.x - v1.x;
    e2.y = v3.y - v1.y;
    e2.z = v3.z - v1.z;

    // normal is e1 x e2.  (Note: Does not need to be unit length for glNormal.)
    n.x = (e1.y * e2.z) - (e1.z * e2.y);
    n.y = (e1.z * e2.x) - (e1.x * e2.z);
    n.z = (e1.x * e2.y) - (e1.y * e2.x);

    // Draw this triangle.
    glBegin(GL_TRIANGLES);
    glNormal3f(n.x, n.y, n.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
    glEnd();
  }
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_obj();
  glutSwapBuffers();
}

void make_identity_matrix(GLfloat* matrix)
{
  for (int i = 0; i < 16; ++i) {
    matrix[i] = 0;
  }

  matrix[0] = 1;
  matrix[5] = 1;
  matrix[10] = 1;
  matrix[15] = 1;
}

void make_translate_matrix(GLfloat* matrix, float tx, float ty, float tz)
{
  make_identity_matrix(matrix);

  matrix[12] = tx;
  matrix[13] = ty;
  matrix[14] = tz;
}

void make_y_rotate_matrix(GLfloat* matrix, float ry_degrees)
{
  float ry_radians = (M_PI / 180.0) * ry_degrees;
  float cos_ry = cos(ry_radians);
  float sin_ry = sin(ry_radians);

  make_identity_matrix(matrix);

  matrix[0] = cos_ry;
  matrix[2] = -sin_ry;
  matrix[8] = sin_ry;
  matrix[10] = cos_ry;
}

void make_scale_matrix(GLfloat* matrix, float sx, float sy, float sz)
{
  make_identity_matrix(matrix);

  matrix[0] = sx;
  matrix[5] = sy;
  matrix[10] = sz;
}

void init_scene(void)
{
  // Configure a light.
  GLfloat light_diffuse[] = {1.0, 0.0, 0.0, 1.0};  // Red diffuse light.
  GLfloat light_position[] = {1.0, 4.0, 2.0, 0.0};  // Infinite light location.
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  // Configure viewing.  (Assumes object is within [-1,1] in all axes.)
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0.0, 0.0, 0.0,  // Set eye position, target position, and up direction.
    0.0, -2.0, -5.0,
    0.0, 1.0, 0.);

  /**/
  // Pose object using your own matrices.
  GLfloat matrix[16];
  make_translate_matrix(matrix, 0.0, -2.0, -5.0);
  glMultMatrixf(matrix);
  make_y_rotate_matrix(matrix, 30.0);
  glMultMatrixf(matrix);
  make_scale_matrix(matrix, 0.5, 1.25, 1.0);
  glMultMatrixf(matrix);
  /**/

  /*
  // Pose object using OpenGL calls.
  glTranslatef(0.0, -2.0, -5.0);
  glRotatef(30.0, 0.0, 1.0, 0.0);
  glScalef(0.5, 1.25, 1.0);
  /**/
}

int main(int argc, char **argv)
{
  // Check for proper arguments.
  if (argc < 2) {
    cout << "usage: " << argv[0] << " <obj_filename>" << endl;
    exit(0);
  }

  // Read obj file given as argument.
  read_obj_file(argv[1]);

  // Set up glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Topic 1");
  glutDisplayFunc(display);

  // Initialize scene.
  init_scene();

  // Hand control over to glut's main loop.
  glutMainLoop();
}
