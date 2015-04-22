#include <glm/glm.hpp>

using namespace std;
using namespace vertex_math;

vector<char> readFileToCharVector(string file) {
  std::ifstream in(file);
  in.seekg(0, std::ios::end);
  int length = in.tellg();
  in.seekg(0, std::ios::beg);
  std::vector<char> contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  contents.push_back('\0');
  contents.resize(length);
  cout << contents.size() << "\n";
  cout << &contents[0] << "\n";
  return contents;
}

// Assumes only triangles, and that file contains only vertex and face lines.
void read_obj_file(const char* filename, vector<glm::vec3> &vertices, vector<glm::vec3> &normals, vector<triangle> &triangles)
{
  ifstream ifs;
  string first_word;
  string line;
  glm::vec3 new_vertex;
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
        //cout << new_triangle.n1 << new_triangle.n2 << new_triangle.n3 << "\n";
      }
      else if (first_word == "vn") {
        ifs >> new_vertex.x >> new_vertex.y >> new_vertex.z;
        new_vertex = glm::normalize(new_vertex);
        normals.push_back(new_vertex);
      }
      // Get rid of anything left on this line (including the newline).
      ifs.ignore(numeric_limits<streamsize>::max(), '\n');
    }
  } while (ifs.eof() == false);

  // Close file.
  ifs.close();
}

