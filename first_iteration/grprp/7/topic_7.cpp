#include <vector>
#include <algorithm>
#include <GL/glut.h>

using namespace std;

#define WINDOW_WIDTH (400)
#define WINDOW_HEIGHT (300)

struct vertex {
  double x;
  double y;
  double z;
};

struct ETEntry {
  int last_y;
  int start_x;
  float dx_over_dy;
};

struct AETEntry {
  float x;
  int last_y;
  float dx_over_dy;
};

bool aet_entry_less_than(AETEntry e1, AETEntry e2)
{
  return e1.x < e2.x;
}

// x and y are numbered starting from 0, and the filled range is inclusive of x1 and x2.
void fill_scanline(GLfloat* pixels, int y, int x1, int x2, float r, float g, float b)
{
  for (int x = x1; x <= x2; ++x) {
    pixels[(y * WINDOW_WIDTH + x) * 3 + 0] = r;
    pixels[(y * WINDOW_WIDTH + x) * 3 + 1] = g;
    pixels[(y * WINDOW_WIDTH + x) * 3 + 2] = b;
  }
}

void scan_convert_polygon()
{
  GLfloat pixels[WINDOW_WIDTH * WINDOW_HEIGHT * 3] = { 0 };
  vector<vector<ETEntry> > edge_table(WINDOW_HEIGHT);
  vector<AETEntry> active_edge_table;

  // Build edge table.
  ETEntry et1, et2, et3, et4, et5;
  et1.last_y = 200;
  et1.start_x = 200;
  et1.dx_over_dy = -1;
  edge_table.at(100).push_back(et1);
  et2.last_y = 200;
  et2.start_x = 250;
  et2.dx_over_dy = 0.5;
  edge_table.at(100).push_back(et2);
  et3.last_y = 200;
  et3.start_x = 10;
  et3.dx_over_dy = 0.1;
  edge_table.at(10).push_back(et3);
  et4.last_y = 49;
  et4.start_x = 10;
  et4.dx_over_dy = 4;
  edge_table.at(10).push_back(et4);
  et5.last_y = 200;
  et5.start_x = 170;
  et5.dx_over_dy = -0.9;
  edge_table.at(50).push_back(et5);

  // Scanline conversion algorithm.
  for (int y = 0; y < WINDOW_HEIGHT; ++y) {
    // 1. Add current scanline's edges from ET to AET.
    for (int i = 0; i < edge_table[y].size(); ++i) {
      AETEntry new_aet_entry;
      /* ADD CODE HERE */
      ETEntry edge = edge_table[y].at(i);
      new_aet_entry.last_y = edge.last_y;
      new_aet_entry.dx_over_dy = edge.dx_over_dy;
      new_aet_entry.x = (float) edge.start_x;
      active_edge_table.push_back(new_aet_entry);
    }
    // 2. Sort AET by x.
    sort(active_edge_table.begin(), active_edge_table.end(), aet_entry_less_than);
    // 3. Fill between edge pairs in AET.
    vector<AETEntry>::iterator iter;
    int first_x;
    for (iter = active_edge_table.begin(); iter != active_edge_table.end(); ++iter) {
      first_x = (int) iter->x;
      ++iter;
      if (iter != active_edge_table.end()) {
        fill_scanline(pixels, y, first_x, (int) iter->x, 0.8, 0.3, 0.4);
      }
    }
    // 4. Remove any finished edges from AET.
    for (iter = active_edge_table.begin(); iter != active_edge_table.end(); ++iter) {
      if (iter->last_y == y) {
	iter = active_edge_table.erase(iter);
	--iter;
      }
    }
    // 5. Udpate all x in AET for the upcoming y.
    for (iter = active_edge_table.begin(); iter != active_edge_table.end(); ++iter) {
      iter->x = iter->x + iter->dx_over_dy;
    }
    // 6. (y will increment as next iteration begins.)
  }

  // Draw completed pixels to window.
  glDrawPixels(WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGB, GL_FLOAT, pixels);
}

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  scan_convert_polygon();

  glutSwapBuffers();
}

int main(int argc, char **argv)
{
  // Initial configuration.
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
  glutCreateWindow("Topic 7");
  glutDisplayFunc(display);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0, 0.0, 0.0, 1.0);

  // Hand control over to glut's main loop.
  glutMainLoop();
}
