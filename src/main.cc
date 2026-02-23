#define RATATOUILLE_NCURSES
#define PRINT_TRI3(T,F) printf("triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,T.a.z,T.b.x,T.b.y,T.b.z,T.c.x,T.c.y,T.c.z)
#define PRINT_TRI2(T,F) printf("polygon((%" #F ",%" #F "),(%" #F ",%" #F "),(%" #F ",%" #F ")),",T.a.x,T.a.y,T.b.x,T.b.y,T.c.x,T.c.y)
#define PRINT_TRI23(T,F) printf("triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,0,T.b.x,T.b.y,0,T.c.x,T.c.y,0)
#include<stdio.h>
FILE* debug=fopen("./debug/debug.log","w");
#include <r@@2e.hpp>
#include <3rats.hpp>
#include <assets.hpp>
// #include <curses.h>
int main() {
  puts("\rRAT GAME 16\n\r");
  gui::init();
  mesh::model_t* models=assets::readModels("assets/cube.stl");
  unsigned char escapes=0;
  unsigned char mode=0;
  unsigned char modes=2;
  while(true){
    char c=gui::readInput();
    switch(c){//escapey bits. add more later probably. note that tmux is doing strange things to us
      case '\e':escapes|='\x01';continue;
      case '[' :if(escapes&'\x03'=='\x01'){escapes|='\x02';}continue;
      case 'q':exit(0);break;
    }
    if(escapes&'\x03'=='\x03'){
      switch(c){
        case 'A':mode=(mode+1)%modes;break;//up
        case 'B':mode=(mode+modes-1)%modes;break;//down
        case 'C':mesh::camera_rotation.z-=16;break;//left
        case 'D':mesh::camera_rotation.z+=16;break;//right
      }
      escapes=0;
      // continue;
    }
    switch(c){
      case 'w':mesh::camera_position.x+=1;break;
      case 's':mesh::camera_position.x-=1;break;
      case 'd':mesh::camera_position.y+=1;break;
      case 'a':mesh::camera_position.y-=1;break;
      case 'x':{
        gui::clear_scr();
        for(short unsigned int i=0;i<models[0].tricount;i++){
          gui::drawMTri(models[0].tris[i]);
        }
        assets::writeGrayScaleToPPM("debug/frame.ppm",gui::depth_buffer,gui::term_dims.ws_col,gui::term_dims.ws_row);
      }
    }
    if(c){
      gui::clear_scr();
      snprintf(gui::term_buffer,9,"mode=%.1u/%.1u",mode+1,modes);
      // fseek(stdout,-1,SEEK_CUR);
      for(short unsigned int i=0;i<models[0].tricount;i++){
        if(mode==0){gui::drawMTri(models[0].tris[i]);}
        if(mode==1){gui::drawMLines(models[0].tris[i]);}
      }
      fputs("\n",debug);
      gui::drawFrame();
      escapes=0;
    }
  }
  fclose(debug);
  return 0;
}