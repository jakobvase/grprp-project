#include <cmath>
#include <glm/glm.hpp>

namespace vertex_math{

  struct triangle {
    int i1;
    int i2;
    int i3;
    int n1;
    int n2;
    int n3;
  };

  /**
  vertex bezier(vertex p0, vertex p3, vertex n1, vertex n2) {
    vertex b, t1, t2, e1, e2;
    e1 = subtract(p3, p0);
    e2 = subtract(p0, p3);
    t1 = add(p0, cross(cross(n1, e1), n1)); // Create the vector pointing toward the curve.
    t2 = add(p3, cross(cross(n2, e2), n2));
    normalize(t1);
    normalize(t2);
    b = add(scale(p0, .125), add(scale(t1, .375), add(scale(t2, .375), scale(p3, .125))));
    //b = scale(add(p0, p3), .5);
    return b;
  }
  /**/

  void bezier(glm::vec3 p0, glm::vec3 p3, glm::vec3 n1, glm::vec3 n2, glm::vec3& v, glm::vec3& n) {
    glm::vec3 t1, t2, e1, e2, tangent;
    e1 = p3 - p0;
    e2 = p0 - p3;
    t1 = glm::cross(glm::cross(n1, e1), n1); // Create the vector pointing toward the curve.
    t2 = glm::cross(glm::cross(n2, e2), n2);
    t1 = glm::normalize(t1);
    t2 = glm::normalize(t2);
    float l = glm::length(e1);
    t1 = glm::normalize(t1) * (l / 3);
    t2 = glm::normalize(t2) * (l / 3);
    //v = (p0 * .125f) + ((p0 + t1) * .375f) + ((p3 + t2) * .375f) + (p3 * .125f);
    float t = 0.5f;
    float nt = 1 - t;
    glm::vec3 p1 = p0 + t1;
    glm::vec3 p2 = p3 + t2;
    v = nt * nt * nt * p0 +
        3 * nt * nt * t * p1 +
        3 * nt * t * t * p2 +
        t * t * t * p3;
    tangent = 3 * nt * nt * (p1 - p0) +
              6 * nt * t * (p2 - p1) +
              3 * t * t * (p3 - p2);
    n = glm::cross(tangent, glm::cross(n1, e1) + glm::cross(n2, e1));
    //n = n1 + n2;
    n = glm::normalize(n);
  }

  triangle createTriangle(int p0, int p3, int v3, int n1, int n2, int n3) {
    triangle t;
    t.i1 = p0;
    t.i2 = p3;
    t.i3 = v3;
    t.n1 = n1;
    t.n2 = n2;
    t.n3 = n3;
    return t;
  }
}