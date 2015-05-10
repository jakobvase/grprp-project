#include <vector>
#include <iostream>
#include <fstream>

#ifdef __linux__
#include <GL/glut.h>
#else
#include <GLUT/glut.h>
#endif

#include "glm/glm.hpp" //External library for opengl math functions

#include "vertex.cpp" //home-written class for math on vertices
#include "file_functions.cpp" //home-written class for loading object files

using namespace std;
using namespace vertex_math;

#define LINE_SIZE (256)

vector<glm::vec3> vertices; // A list of our vertices
vector<triangle> triangles; // A list of our triangles, 
//triangle is defined in vertex.cpp
vector<glm::vec3> normals; // A list of normals corresponding to vertices

int t; // t is the time the program launched.

// Curve an object
// parameter count -> the number of times to run through the curving.
// Every run multiplies the number of triangles in the object by 4.
// Effectively, the new triangle count will be:
// original_triangle_count * 4 ^ count.
void curve_object(int count) {
  if(count <= 0) return; // Base case for recursion.

  //v = vertex, n = normal, b = new curved vertex, bn = new curved vertex normal.
  glm::vec3 v1, v2, v3, n1, n2, n3, b1, b2, b3, bn1, bn2, bn3;

  //i = index of new vertex, in = index of new normal, 
  //o = original vertex index, on = original vertex normal index
  int i1, i2, i3, in1, in2, in3, o1, o2, o3, on1, on2, on3;

  //the four new triangles.
  triangle t1, t2, t3, t4;

  //the triangle vector to replace the old triangle vector.
  vector<triangle> new_triangles;

  // Main loop, runs through all the triangles in the object and splits them,
  // sacing the new triangles in new_triangles.
  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices out of triangles vector.
    o1 = triangles.at(i).i1;
    o2 = triangles.at(i).i2;
    o3 = triangles.at(i).i3;
    // Read normals out of triangles vector
    on1 = triangles.at(i).n1;
    on2 = triangles.at(i).n2;
    on3 = triangles.at(i).n3;

    // Find the vertices and normals
    v1 = vertices.at(o1);
    v2 = vertices.at(o2);
    v3 = vertices.at(o3);
    n1 = normals.at(on1);
    n2 = normals.at(on2);
    n3 = normals.at(on3);

    // Curve the points! bezier() is defined in vertex.cpp
    bezier(v1, v2, n1, n2, b1, bn1);
    bezier(v2, v3, n2, n3, b2, bn2);
    bezier(v3, v1, n3, n1, b3, bn3);

    // compute the indexes for the new vertices and normals.
    i1 = vertices.size();
    i2 = i1 + 1;
    i3 = i2 + 1;
    in1 = normals.size();
    in2 = in1 + 1;
    in3 = in2 + 1;

    // Push the new points into the vertex and normal lists.
    // This is very unsafe in multithreaded environments.
    vertices.push_back(b1);
    vertices.push_back(b2);
    vertices.push_back(b3);
    normals.push_back(bn1);
    normals.push_back(bn2);
    normals.push_back(bn3);

    // Create the new triangles. createTriangle() is defined in vertex.cpp
    t1 = createTriangle(o1, i1, i3, on1, in1, in3);
    t2 = createTriangle(o2, i2, i1, on2, in2, in1);
    t3 = createTriangle(o3, i3, i2, on3, in3, in2);
    t4 = createTriangle(i1, i2, i3, in1, in2, in3);

    //Push back the new triangles and end the loop.
    new_triangles.push_back(t1);
    new_triangles.push_back(t2);
    new_triangles.push_back(t3);
    new_triangles.push_back(t4);
  }

  // This is the recursive step!
  // Set the old triangles to the new ones, and call curve again with
  // the count reduced.
  triangles = new_triangles;
  curve_object(--count);
}

// Draw the object on the screen.
void draw_obj(void)
{
  glm::vec3 v1, v2, v3, n1, n2, n3; // vectors and normals
  glm::vec3 n; // The normal of a face

  glPushMatrix(); //Push, so that matrix calculations won't 
  //need to be redone if the camera moves.
  glRotatef((clock() - t) / 5e4, 0.0, 1.0, 0.0); // Rotate slowly around the y axis,
  // based on the time since the program launched
  glBegin(GL_TRIANGLES); // Begin drawing triangles!

  for (int i = 0; i < triangles.size(); ++i) {
    // Read vertices and normals out of triangles vector.
    v1 = vertices.at(triangles.at(i).i1);
    v2 = vertices.at(triangles.at(i).i2);
    v3 = vertices.at(triangles.at(i).i3);
    n1 = normals.at(triangles.at(i).n1);
    n2 = normals.at(triangles.at(i).n2);
    n3 = normals.at(triangles.at(i).n3);

    /* For face lighting (per face lighting) *
    n = (n1 + n2 + n3) * 0.3333f; // compute face normal
    glNormal3fv(&n[0]);
    glVertex3fv(&v1[0]);
    glVertex3fv(&v2[0]);
    glVertex3fv(&v3[0]);
    /**/

    /* For smooth lightning (per vertex lighting) */
    glNormal3f(n1.x, n1.y, n1.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glNormal3f(n2.x, n2.y, n2.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glNormal3f(n3.x, n3.y, n3.z);
    glVertex3f(v3.x, v3.y, v3.z);
    /**/
  }
  glEnd(); // stop drawing!
  glPopMatrix(); // Remember to pop the matrix again.
}

// A function for displaying objects. Resets buffers, draws an object and
// displays it.
void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // TODO what is this?
  draw_obj(); // Draw the object!
  glutSwapBuffers(); // TODO ???
  glutPostRedisplay(); // TODO ???
}

// Set the scene for the object.
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
    0.0, 1.0, 0.0);

  // Rotate object.
  glRotatef(30, 0.0, 1.0, 0.0);
}

// Main
int main(int argc, char **argv)
{
  // Check for proper arguments.
  if (argc < 3) {
    cout << "usage: " << argv[0] << " <obj_filename> <curve_iterations>" << endl;
    exit(0);
  }

  // Read obj file given as argument. read_obj_file() is defined in file_functions.cpp.
  read_obj_file(argv[1], vertices, normals, triangles);

  // Curve it!
  curve_object(stoi(argv[2]));
  
  // Set up glut.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("Topic 1");
  glutDisplayFunc(display);

  // Initialize scene.
  init_scene();

  // Save the time the program was started.
  t = clock();

  // Hand control over to glut's main loop.
  glutMainLoop();
}
