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

enum ClipDirection {LOW_X, HIGH_X, LOW_Y, HIGH_Y, LOW_Z, HIGH_Z };

struct vertex {
  double x;
  double y;
  double z;
};
struct triangle {
  int vi1;
  int vi2;
  int vi3;
  int ni1;
  int ni2;
  int ni3;
};

vector<vertex> vertices;
vector<vertex> normals;
vector<triangle> triangles;
GLfloat light_position[] = {10.0, 40.0, 20.0, 0.0};
GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat material_ambient[] = { 0.0, 0.0, 0.2, 1.0 };
GLfloat material_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat eye[] = {0.0, 2.0, 5.0};

void normalize(vertex& v)
{
  float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  v.x /= length;
  v.y /= length;
  v.z /= length;
}

// Assumes only triangles, and that file contains only vertex, vertex normal, and face lines.
void read_obj_file(const char* filename)
{
  ifstream ifs;
  string first_word;
  char junk_char;
  vertex new_vertex;
  vertex new_normal;
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
      else if (first_word == "vn") {
	// Read a vertex-normal line.
	ifs >> new_normal.x >> new_normal.y >> new_normal.z;
	normalize(new_normal); // (Just in case.)
	normals.push_back(new_normal);
      }
      else if (first_word == "f") {
	// Read a face line.
	ifs >> new_triangle.vi1 >> junk_char >> junk_char >> new_triangle.ni1;
	ifs >> new_triangle.vi2 >> junk_char >> junk_char >> new_triangle.ni2;
	ifs >> new_triangle.vi3 >> junk_char >> junk_char >> new_triangle.ni3;

	// Decrement indices from [1,n] in obj file to [0,n-1] in vertex and normal vectors.
	--new_triangle.vi1;
	--new_triangle.vi2;
	--new_triangle.vi3;
	--new_triangle.ni1;
	--new_triangle.ni2;
	--new_triangle.ni3;
	triangles.push_back(new_triangle);
      }
      // Get rid of anything left on this line (including the newline).
      ifs.ignore(numeric_limits<streamsize>::max(), '\n');
    }
  } while (ifs.eof() == false);

  // Close file.
  ifs.close();
}

bool is_inside(vertex v, ClipDirection direction, float position)
{
  switch (direction) {
  case LOW_X:
    return v.x > position;
    break;
  case HIGH_X:
    return v.x < position;
    break;
  case LOW_Y:
    return v.y > position;
    break;
  case HIGH_Y:
    return v.y < position;
    break;
  case LOW_Z:
    return v.z > position;
    break;
  case HIGH_Z:
    return v.z < position;
    break;
  };
}

vertex intersect(vertex v1, vertex v2, ClipDirection direction, float position)
{
  vertex result;
  float normalized_intersection;

  switch (direction) {
  case LOW_X:
  case HIGH_X:
    normalized_intersection =  (position - v1.x) / (v2.x - v1.x);
    break;
  case LOW_Y:
  case HIGH_Y:
    normalized_intersection =  (position - v1.y) / (v2.y - v1.y);
    break;
  case LOW_Z:
  case HIGH_Z:
    normalized_intersection =  (position - v1.z) / (v2.z - v1.z);
    break;
  };

  result.x = v1.x + normalized_intersection * (v2.x - v1.x);
  result.y = v1.y + normalized_intersection * (v2.y - v1.y);
  result.z = v1.z + normalized_intersection * (v2.z - v1.z);

  return result;
}

// NOTE: Currently returns whole polygon if any vertex is un-clipped.
// REPLACE WITH FULL CLIPPING ALGORITHM.
vector<vertex> clip_polygon(vector<vertex> input_vertex_list, ClipDirection direction, float position)
{
  vector<vertex> output_vertex_list; // (Initially empty.)
  if(input_vertex_list.size() < 3) {
    return output_vertex_list;
  }

  vertex last;
  vertex current;
  last = input_vertex_list.back();
  bool was_inside = is_inside(last, direction, position);
  for (int i = 0; i < input_vertex_list.size(); ++i) {
    current = input_vertex_list.at(i);
    if (is_inside(current, direction, position)) {
      if(!was_inside) {
        output_vertex_list.push_back(intersect(last, current, direction, position));
        was_inside = true;
      }
      output_vertex_list.push_back(current);
    } else {
      if(was_inside) {
        output_vertex_list.push_back(intersect(last, current, direction, position));
        was_inside = false;
      }
    }
    last = current;
  }

  return output_vertex_list;
}

void draw_obj(void)
{
  vertex v1, v2, v3;
  vertex e1, e2;
  vertex n;
  vector<vertex> vertex_list;

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    v1 = vertices.at(triangles.at(i).vi1);
    v2 = vertices.at(triangles.at(i).vi2);
    v3 = vertices.at(triangles.at(i).vi3);

    // e1 is edge from v1 to v2.
    e1.x = v2.x - v1.x;
    e1.y = v2.y - v1.y;
    e1.z = v2.z - v1.z;

    // e2 is edge from v1 to v3.
    e2.x = v3.x - v1.x;
    e2.y = v3.y - v1.y;
    e2.z = v3.z - v1.z;

    // Normal is e1 x e2.
    n.x = (e1.y * e2.z) - (e1.z * e2.y);
    n.y = (e1.z * e2.x) - (e1.x * e2.z);
    n.z = (e1.x * e2.y) - (e1.y * e2.x);
    normalize(n);

    // Treat triangle as a general polygon and clip it against all six planes.
    vertex_list.clear();
    vertex_list.push_back(v1);
    vertex_list.push_back(v2);
    vertex_list.push_back(v3);
    vertex_list = clip_polygon(vertex_list, LOW_X, -0.8);
    vertex_list = clip_polygon(vertex_list, HIGH_X, 0.8);
    vertex_list = clip_polygon(vertex_list, LOW_Y, -0.8);
    vertex_list = clip_polygon(vertex_list, HIGH_Y, 0.8);
    vertex_list = clip_polygon(vertex_list, LOW_Z, -0.8);
    vertex_list = clip_polygon(vertex_list, HIGH_Z, 0.8);

    // Draw the resulting polygon.
    glBegin(GL_POLYGON);
    glNormal3f(n.x, n.y, n.z);
    for (int i = 0; i < vertex_list.size(); ++i) {
      glVertex3f(vertex_list.at(i).x, vertex_list.at(i).y, vertex_list.at(i).z);
    }
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
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Configure object material.
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  // Configure viewing.  (Assumes object is within [-1,1] in all axes.)
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(eye[0], eye[1], eye[2],  // Set eye position, target position, and up direction.
    0.0, -2.0, -5.0,
    0.0, 1.0, 0.0);
  glRotatef(30.0, 0.0, 1.0, 0.0);
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
  glutCreateWindow("Topic 6");
  glutDisplayFunc(display);

  // Initialize scene.
  init_scene();

  // Hand control over to glut's main loop.
  glutMainLoop();
}
