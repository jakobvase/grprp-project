#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <string>
#include <GLUT/glut.h>

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

/*void normalize(vertex& v)
{
  float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  v.x /= length;
  v.y /= length;
  v.z /= length;
}*/

float dot(vertex v1, vertex v2)
{
  double result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

  return result;
}

vertex cross(vertex v1, vertex v2) {
  vertex cr;
  cr.x = v1.y * v2.z - v1.z * v2.y;
  cr.y = v1.z * v2.x - v1.x * v2.z;
  cr.z = v1.x * v2.y - v1.y * v2.x;
  return cr;
}

vertex subtract(vertex v1, vertex v2) {
  vertex sub;
  sub.x = v1.x - v2.x;
  sub.y = v1.y - v2.y;
  sub.z = v1.z - v2.z;
  return sub;
}

vertex add(vertex v1, vertex v2) {
  vertex sub;
  sub.x = v1.x + v2.x;
  sub.y = v1.y + v2.y;
  sub.z = v1.z + v2.z;
  return sub;
}

vertex scale(vertex v, double s) {
  vertex sub;
  sub.x = v.x * s;
  sub.y = v.y * s;
  sub.z = v.z * s;
  return sub;
}

vertex bezier(vertex v1, vertex v2, vertex n1, vertex n2) {
  vertex b, t1, t2;
  t1 = cross(cross(n1, subtract(v2, v1)), n1); // Create the vector pointing toward the curve.
  t2 = cross(cross(n2, subtract(v1, v2)), n2);
  b = add(scale(v1, .125), add(scale(t1, .75), add(scale(t2, .75), scale(v2, .125))));
  return b;
}

triangle createTriangle(int v1, int v2, int v3, int n1, int n2, int n3) {
  triangle t;
  t.i1 = v1;
  t.i2 = v2;
  t.i3 = v3;
  t.n1 = n1;
  t.n2 = n2;
  t.n3 = n3;
  return t;
}

void curve_object(int count) {
  if(count == 0) return;
  vertex v1, v2, v3, n1, n2, n3, b1, b2, b3, bn1, bn2, bn3;
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

    b1 = bezier(v1, v2, n1, n2);
    b2 = bezier(v2, v3, n2, n3);
    b3 = bezier(v3, v1, n3, n1);
    bn1 = scale(add(n1, n2), .5);
    bn2 = scale(add(n2, n3), .5);
    bn3 = scale(add(n3, n1), .5);

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

  // Curve it!
  curve_object(1);
  
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
