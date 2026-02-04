#define RATATOUILLE_NCURSES
#define PRINT_TRI3(T,F) printf("triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,T.a.z,T.b.x,T.b.y,T.b.z,T.c.x,T.c.y,T.c.z)
#define PRINT_TRI2(T,F) printf("triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,0,T.b.x,T.b.y,0,T.c.x,T.c.y,0)
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
  while(true){
    switch(gui::readInput()){
      case 'w':mesh::camera.x+=1;break;
      case 'a':mesh::camera.y+=1;break;
      case 's':mesh::camera.x-=1;break;
      case 'd':mesh::camera.y-=1;break;
      case ' ':{
        mesh::model_t* models=assets::readModels("assets/cube.stl");
        gui::clear_scr();
        for(short unsigned int i=0;i<models[0].tricount;i++){
          gui::drawMTri(models[0].tris[i]);
        }
        gui::drawFrame();
      }break;
      case 'q':exit(0);break;
    }
  }
  return 0;
}