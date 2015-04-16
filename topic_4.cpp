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
GLfloat light_position[] = {10.0, 40.0, 20.0, 1.0};
GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat material_ambient[] = { 0.0, 0.0, 0.2, 1.0 };
GLfloat material_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };
GLfloat material_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat material_shininess[] = { 50.0 };
GLfloat eye[] = {0.0, 0.0, 5.0};

void normalize(vertex& v)
{
  float length = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
  v.x /= length;
  v.y /= length;
  v.z /= length;
}

float dot(vertex v1, vertex v2)
{
  double result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

  return result;
}

float nonnegative_dot(vertex v1, vertex v2)
{
  double result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;

  if (result < 0.0)
    return 0;
  else
    return result;
}

// Reflect vector v through vector n.
// (Assumes both are normalized.)
vertex reflect(vertex v, vertex n)
{
  vertex v_on_n = {dot(v,n) * n.x, dot(v,n) * n.y, dot(v,n) * n.z};
  vertex result = {2.0 * v_on_n.x - v.x, 2.0 * v_on_n.y - v.y, 2.0 * v_on_n.z - v.z};

  return result;
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

// (Note: Assumes no modeling transformations.)
void shade_vertex(vertex v, vertex n)
{
  float result_r, result_g, result_b;
  vertex l = {light_position[0], light_position[1], light_position[2]};
  float Kd_r = material_diffuse[0];
  float Kd_g = material_diffuse[1];
  float Kd_b = material_diffuse[2];
  float Ka_r = material_ambient[0];
  float Ka_g = material_ambient[1];
  float Ka_b = material_ambient[2];
  float Ks_r = material_specular[0];
  float Ks_g = material_specular[1];
  float Ks_b = material_specular[2];
  float Kexp = material_shininess[0] / 5.0; // HACK! Not sure why, but required to approximate OpenGL results.
  vertex v_to_l = {l.x - v.x, l.y - v.y, l.z - v.z};
  normalize(v_to_l);
  vertex l_reflect = reflect(v_to_l, n);
  vertex v_to_eye = {eye[0] - v.x, eye[1] - v.y, eye[2] - v.z};
  normalize(v_to_eye);

  /**  
  // Set colors without real shading.
  // REPLACE WITH YOUR OWN SPECULAR SHADING CALCULATIONS BELOW.
  result_r = v.x / 2.0 + 0.5;
  result_g = 1.0 - (v.x / 2.0 + 0.5);
  result_b = 0.0;
  */  

  // ACCUMULATE SPECULAR SHADING COLOR:
  // SET TO ZERO TO START.
  result_r = 0.0;
  result_g = 0.0;
  result_b = 0.0;

  // ADD AMBIENT COMPONENT.
  result_r += Ka_r;
  result_g += Ka_g;
  result_b += Ka_b;

  float NL = nonnegative_dot(n, v_to_l);
  // ADD DIFFUSE COMPONENT.  (Try using nonnegative_dot().)
  result_r += Kd_r * NL;
  result_g += Kd_g * NL;
  result_b += Kd_b * NL;

  float ERKexp =  pow(nonnegative_dot(v_to_eye, l_reflect), Kexp);
  // ADD SPECULAR COMPONENT.  (Try using nonnegative_dot() and pow().)
  result_r += Ks_r * ERKexp;
  result_g += Ks_g * ERKexp;
  result_b += Ks_b * ERKexp;
  

  // Apply final lighting result to vertex.
  glColor3f(result_r, result_g, result_b);
}

void draw_obj(void)
{
  vertex v1, v2, v3;
  vertex n1, n2, n3; 

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices and normals out of triangles vector.
    v1 = vertices.at(triangles.at(i).vi1);
    v2 = vertices.at(triangles.at(i).vi2);
    v3 = vertices.at(triangles.at(i).vi3);
    n1 = normals.at(triangles.at(i).ni1);
    n2 = normals.at(triangles.at(i).ni2);
    n3 = normals.at(triangles.at(i).ni3);

    /**/
    // UNCOMMENTING THIS LINE ALLOWS YOUR LIGHTING TO TAKE EFFECT.
    glDisable(GL_LIGHTING);
    /**/

    // Draw this triangle.
    glBegin(GL_TRIANGLES);

    shade_vertex(v1, n1);
    glNormal3f(n1.x, n1.y, n1.z);
    glVertex3f(v1.x, v1.y, v1.z);

    shade_vertex(v2, n2);
    glNormal3f(n2.x, n2.y, n2.z);
    glVertex3f(v2.x, v2.y, v2.z);

    shade_vertex(v3, n3);
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
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);

  // Configure object material.
  glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

  // Use depth buffering for hidden surface elimination.
  glEnable(GL_DEPTH_TEST);

  // Configure viewing.  (Assumes object is within [-1,1] in all axes.)
  glMatrixMode(GL_PROJECTION);
  gluPerspective(40.0, 1.0, 1.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  gluLookAt(eye[0], eye[1], eye[2],  // Set eye position, target position, and up direction.
    0.0, 0.0, 0.0,
    0.0, 1.0, 0.0);
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
  glutCreateWindow("Topic 4");
  glutDisplayFunc(display);

  // Initialize scene.
  init_scene();

  // Hand control over to glut's main loop.
  glutMainLoop();
}
