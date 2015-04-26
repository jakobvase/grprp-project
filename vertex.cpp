#include <cmath>
#include "glm/glm.hpp"

namespace vertex_math{

  struct triangle {
    int i1;
    int i2;
    int i3;
    int n1;
    int n2;
    int n3;
  };

  void bezier(glm::vec3 v1, glm::vec3 v2, glm::vec3 n1, glm::vec3 n2, glm::vec3& v, glm::vec3& n) {
    glm::vec3 t1, t2, e1, e2, t;
    e1 = v2 - v1;
    e2 = v1 - v2;
    t1 = glm::cross(glm::cross(n1, e1), n1); // Create the vector pointing toward the curve.
    t2 = glm::cross(glm::cross(n2, e2), n2);
    t1 = glm::normalize(t1);
    t2 = glm::normalize(t2);
    float l = glm::length(e1);
    t1 = glm::normalize(t1) * (l / 3);
    t2 = glm::normalize(t2) * (l / 3);
    v = (v1 * .125f) + ((v1 + t1) * .375f) + ((v2 + t2) * .375f) + (v2 * .125f);
    t = ((v1 + t1 - v1) * .75f) + ((v2 + t2 - (v1 + t1)) * 1.5f) + ((v2 - (v2 + t2)) * .75f);
    //t = add(scale(subtract(add(v1, t1), v1), 0.75), add(scale(subtract(add(v2, t2), add(v1, t1)), 1.5), scale(subtract(v2, add(v2, t2)), 0.75)));
    n = glm::cross(t, glm::cross(n1, e1) + glm::cross(n2, e1));
    //n = n1 + n2;
    n = glm::normalize(n);
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