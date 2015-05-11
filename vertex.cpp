#include "glm/glm.hpp"

// Create our own namespace for vertex math!
namespace vertex_math{

	// A triangle with 3 corners and normals for each corner.
  struct triangle {
    int i1;
    int i2;
    int i3;
    int n1;
    int n2;
    int n3;
  };

  // Compute a new curved vertex and normal midway between two vertices, 
  // using the normals of the given vertices as guides.
  // parameters: p0 = the first vertex, p3 = the last vertex, n1 = normal of p0,
  // n2 = normal of p3, v = return vector, n = return normal.
  void bezier(glm::vec3 p0, glm::vec3 p3, glm::vec3 n1, glm::vec3 n2, glm::vec3& v, glm::vec3& n) {
    glm::vec3 t1, t2, e1, e2, tangent; // t = tangents, e = edges, tangent = tangent at new point.
    e1 = p3 - p0; // vector from p0 to p3
    e2 = p0 - p3; // vector from p3 to p0

    // Cross product magic to get a vector pointing toward the curve.
    // By crossing the normal of the point with the vector to the other point,
    // we get a vector orthogonal to their plane. By then crossing that vector 
    // with the normal, we get a vector orthogonal to that plane, 90 degrees from
    // the normal, in the plane with the the other vertex. Yay.
    t1 = glm::cross(glm::cross(n1, e1), n1);
    t2 = glm::cross(glm::cross(n2, e2), n2);

    // Set the tangents to be 1/3 of the length between the points.
    // This value can be varied, but using 1/3 to make sure that all four
    // control points are evenly distributed on the edge.
    float l = glm::length(e1);
    t1 = glm::normalize(t1) * (l / 3);
    t2 = glm::normalize(t2) * (l / 3);

    // Compute p1 and p2
    glm::vec3 p1 = p0 + t1;
    glm::vec3 p2 = p3 + t2;

    // Compute the new vertex
    float t = 0.5f;
    float nt = 1 - t;
    v = nt * nt * nt * p0 +
        3 * nt * nt * t * p1 +
        3 * nt * t * t * p2 +
        t * t * t * p3;

    // Compute the new normal
    tangent = 3 * nt * nt * (p1 - p0) +
              6 * nt * t * (p2 - p1) +
              3 * t * t * (p3 - p2);
    // This is the same double-crossing as before, just the other way around.
    // The reason for both n1 and n2 cross products is that sometimes the cross
    // product has a length of 0.
    n = glm::cross(tangent, glm::cross(n1, e1) + glm::cross(n2, e1));
    n = glm::normalize(n);
  }

  // Create a new triangle from the given parameters.
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