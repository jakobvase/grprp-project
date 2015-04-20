#include <cmath>

namespace vertex_math{

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
    vertex b, t1, t2, e1, e2;
    e1 = subtract(v2, v1);
    e2 = subtract(v1, v2);
    t1 = add(v1, cross(cross(n1, e1), n1)); // Create the vector pointing toward the curve.
    t2 = add(v2, cross(cross(n2, e2), n2));
    normalize(t1);
    normalize(t2);
    b = add(scale(v1, .125), add(scale(t1, .375), add(scale(t2, .375), scale(v2, .125))));
    //b = scale(add(v1, v2), .5);
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
}