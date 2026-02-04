#define RATATOUILLE_NCURSES
#include <r@@2e.h>
#include <3rats.h>
#include <assets.h>
// #include <curses.h>
int main() {
  puts("\rRAT GAME 16\n\r");
  gui::init();
  mesh::model_t* models=assets::readModels("assets/cube.stl");
  for(short unsigned int i=0;i<models[0].tricount;i++){
    gui::drawMTri(models[0].tris[i]);
  }
  gui::drawFrame();
  while(gui::readInput()!='q');
  return 0;
}