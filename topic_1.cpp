#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>
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
  int n1;
  int n2;
  int n3;
};

vector<vertex> vertices;
vector<triangle> triangles;
vector<vertex> normals;

// Assumes only triangles, and that file contains only vertex and face lines.
void read_obj_file(const char* filename)
{
  ifstream ifs;
  string first_word;
  string line;
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
        ifs >> new_triangle.i1 >> new_triangle.n1 >> new_triangle.i2 >> new_triangle.n2 >> new_triangle.i3 >> new_triangle.n3;

	      // Decrement indices from [1,n] in obj file to [0,n-1] in vertex vector.
        new_triangle.i1 --;
        new_triangle.i2 --;
        new_triangle.i3 --;
        new_triangle.n1 --;
        new_triangle.n2 --;
        new_triangle.n3 --;

	      triangles.push_back(new_triangle);
        cout << new_triangle.n1 << new_triangle.n2 << new_triangle.n3 << "\n";
      }
      else if (first_word == "vn") {
        ifs >> new_vertex.x >> new_vertex.y >> new_vertex.z;
        normals.push_back(new_vertex);
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
  vertex v1, v2, v3, n1, n2, n3;
  vertex e1, e2;
  vertex n;

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    v1 = vertices.at(triangles.at(i).i1);
    v2 = vertices.at(triangles.at(i).i2);
    v3 = vertices.at(triangles.at(i).i3);
    n1 = normals.at(triangles.at(i).n1);
    n2 = normals.at(triangles.at(i).n2);
    n3 = normals.at(triangles.at(i).n3);

    // Draw this triangle.
    glBegin(GL_TRIANGLES);
    glNormal3f(n1.x, n1.y, n1.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glNormal3f(n2.x, n2.y, n2.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glNormal3f(n3.x, n3.y, n3.z);
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
  gluLookAt(0.0, 2.0, 5.0,  // Set eye position, target position, and up direction.
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.);

  // Rotate object.
  glRotatef(30, 0.0, 1.0, 0.0);
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
