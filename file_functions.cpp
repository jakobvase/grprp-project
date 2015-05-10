#include <fstream>
#include "glm/glm.hpp"

using namespace std;
using namespace vertex_math;

// Read a file into a char vector,
// this is useful for loading shaders.
// parameter file: The file name to read.
// returns: a vector<char> with the contents of the file.
vector<char> readFileToCharVector(string file) {
  std::ifstream in(file);
  in.seekg(0, std::ios::end); // move to end of file
  int length = in.tellg();// Save the length of the file
  in.seekg(0, std::ios::beg); // move to beginning of file
  //save the contents of the file into vector.
  std::vector<char> contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
  contents.push_back('\0'); // add null character at the end
  contents.resize(length); // make the vector only as long as the file was.
  // The length of the file is the last index +1, so the null character gets saved too.
  cout << contents.size() << "\n"; // Write content size to output
  cout << &contents[0] << "\n"; // Write read text to output.
  return contents;
}

// Read an object file into three arrays.
// Assumes that the object only contains triangles, 
// and that file contains only vertex, face, and vertex normal lines.
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
        // Assumes that all faces have normals.
        ifs >> new_triangle.i1 >> new_triangle.n1 >> new_triangle.i2 >> new_triangle.n2 >> new_triangle.i3 >> new_triangle.n3;

        // Decrement indices from [1,n] in obj file to [0,n-1] in vertex and normal vectors.
        new_triangle.i1 --;
        new_triangle.i2 --;
        new_triangle.i3 --;
        new_triangle.n1 --;
        new_triangle.n2 --;
        new_triangle.n3 --;

        // Save the triangle
        triangles.push_back(new_triangle);
      }
      else if (first_word == "vn") {
      	// Read a vertex normal line
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

