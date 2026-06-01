// #define do_debug :3 //makes everything really slow
#define VERSION "v0.0.2"
#define PRINT_TRI3(B,T,F) fprintf(B,"triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,T.a.z,T.b.x,T.b.y,T.b.z,T.c.x,T.c.y,T.c.z)
#define PRINT_TRI2(B,T,F) fprintf(B,"polygon((%" #F ",%" #F "),(%" #F ",%" #F "),(%" #F ",%" #F ")),",T.a.x,T.a.y,T.b.x,T.b.y,T.c.x,T.c.y)
#define PRINT_TRI21(B,F)   fprintf(B,"polygon((%" #F ",%" #F "),(%" #F ",%" #F "),(%" #F ",%" #F ")),",x0,y0,x1,y1,x2,y2)
#define PRINT_TRI23(B,T,F) fprintf(B,"triangle((%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F "),(%" #F ",%" #F ",%" #F ")),",T.a.x,T.a.y,0,T.b.x,T.b.y,0,T.c.x,T.c.y,0)
#include <stdio.h>
#include <stdlib.h>
#include <type_traits>
#include <cmath>
#include <time.h>
FILE* debug;
typedef void (*function)(void);
bool logmisc=false;
template<typename T> concept arith=std::is_arithmetic_v<T>;
template<typename T> concept comp =requires(T a,T b){a<b;a>b;};
template<comp T,comp U> T constexpr const min(T a,U b){return a<b?a:b;}
template<comp T,comp U> T constexpr const max(T a,U b){return a<b?b:a;}
template<comp T,comp...U> T constexpr const min(T t, U...a){
  T b=min(a...);
  return t<b?t:b;
}
template<comp T,comp...U> T constexpr const max(T t, U...a){
  T b=max(a...);
  return t<b?b:t;
}
#include <types.hpp>
#include <assets.hpp>
#include <r@@2e.hpp>
#include <3rats.hpp>
assets::font_t f_big;
assets::font_t f_avatar;

assets::asset3d_t scene;

assets::sprite_t sprite;

gui::menu_t mainmenu;
gui::menu_t pausemenu;

struct gamestate {
  char paused;
  void (*drawFunc)();
} gamestate{};

void drawMainMenu(){
  gui::putMenu(&mainmenu,0,0);
}
void drawWorld(){
  if(gamestate.paused){
    gui::putMenu(&pausemenu,0,0);
  }else{
    for(unsigned int i=0;i<scene.mesh.tricount;i++){
      gui::drawMTri(scene.mesh.tris[i],scene.textures[scene.tex_binds[i]]);
    }
    gui::putSprite(&sprite,0,0);
  }
}
void buttonContinue(){
  gamestate.drawFunc=drawWorld;
  gamestate.paused=0;
}
void quit(){
  gui::stop();
  exit(0);
}

void loadFonts(){
  puts("LOADING FONTS");
  gui::f_default=assets::readFont("./assets/font/1x1.rgft");
  f_big         =assets::readFont("./assets/font/big.rgft");
  f_avatar      =assets::readFont("./assets/font/avatar.rgft");
}
void loadModels(){
  puts("LOADING MODELS");
  scene=assets::readAsset3d("./assets/newscene.rgmdl");//ari i'm going to ear you
}
void loadSprites(){
  puts("LOADING SPRITES\r");
  sprite=assets::readRGVTX("./assets/sprite/hand.rgvtx",gui::term_dims.ws_col,gui::term_dims.ws_row);
}
void loadMenus(){
  mainmenu={
    .sizex=0,.sizey=0,
    .borders={'=','=','H','H','#'},
    .textcount=1,.btncount=2
  };
  mainmenu.items=(gui::text_t*)malloc(sizeof(gui::text_t));
  mainmenu.items[0]={&f_big,"RAT GAME 16" VERSION,strlen("RAT GAME 16" VERSION),gui::LEFT};
  mainmenu.buttons=(gui::text_t*)malloc(sizeof(gui::text_t)*2);
  mainmenu.funcs=(function*)malloc(sizeof(function)*2);
  mainmenu.buttons[0]={&f_avatar,"start game",10,gui::LEFT};
  mainmenu.funcs[0]=&buttonContinue;
  mainmenu.buttons[1]={&f_avatar,"quit",4,gui::LEFT};
  mainmenu.funcs[1]=&quit;
  pausemenu={
    .sizex=0,.sizey=0,
    .borders={'-','-','|','|','+'},
    .textcount=1,.btncount=2,
  };
  pausemenu.items=(gui::text_t*)malloc(sizeof(gui::text_t)*2);//trust me
  pausemenu.items[0]={&f_big,"game paused",11,gui::CENTER};
  pausemenu.buttons=(gui::text_t*)malloc(sizeof(gui::text_t)*2);
  pausemenu.funcs=(function*)malloc(sizeof(function)*2);
  pausemenu.buttons[0]={&f_avatar,"continue",8,gui::CENTER};
  pausemenu.funcs[0]=&buttonContinue;
  pausemenu.buttons[1]={&f_avatar,"quit",4,gui::CENTER};
  pausemenu.funcs[1]=&quit;
}

int main() {
  puts("RAT GAME 16");
  debug=fopen("./debug/debug.log","w");
  if(ferror(debug)||errno||!debug){perror("couldn't open debug log file :(");exit(1);}
  loadFonts();
  loadModels();
  loadMenus();
  gamestate.drawFunc=drawMainMenu;
  gui::init();
  printf("SCREEN INIT: %i,%i\n",gui::term_dims.ws_col,gui::term_dims.ws_row);
  loadSprites();
  unsigned char escapes=0;
  unsigned char rotamnt=16;
  float rotamntrad = (rotamnt/128.0f)*M_PI;
  float rottrck=0;
  gui::selected_menu=&pausemenu;
  gui::selected_btn=0;
  while(true){
    char c=gui::readInput();
    if(c=='q'){quit();}
    switch(c){//escapey bits. add more later probably. note that tmux is doing strange things to us
      case '\e':escapes|='\x01';continue;
      case '[' :if((escapes&'\x03')=='\x01'){escapes|='\x02';continue;}else{//am i getting ear'd for this one
        gui::selected_btn=(gui::selected_btn+gui::selected_menu->btncount-1)%gui::selected_menu->btncount;
        break;
      }
    }
    if((escapes&'\x03')=='\x03'){
      switch(c){
        case 'A':mesh::fov+=M_PI_4/3.0f;gui::update_fov();break;
        case 'B':mesh::fov-=M_PI_4/3.0f;gui::update_fov();break;
        case 'C':mesh::camera_rotation.z-=rotamnt;rottrck+=rotamntrad;break;//left
        case 'D':mesh::camera_rotation.z+=rotamnt;rottrck-=rotamntrad;break;//right
      }
      if(rottrck > 2*M_PI){rottrck-=2*M_PI;}
      if(rottrck < 2*M_PI){rottrck+=2*M_PI;}
      escapes=0;
    }else{
      switch(c){
        case 'w':mesh::camera_position.x+=cos(rottrck);mesh::camera_position.y+=sin(rottrck);break;//ari i just looked at these
        case 's':mesh::camera_position.x-=cos(rottrck);mesh::camera_position.y-=sin(rottrck);break;//,,, not a big fan
        case 'd':mesh::camera_position.x-=sin(rottrck);mesh::camera_position.y+=cos(rottrck);break;//either put everything on radians
        case 'a':mesh::camera_position.x+=sin(rottrck);mesh::camera_position.y-=cos(rottrck);break;//or dont, PICK ONE
        case ',':mesh::camera_position.z++;break;
        case '.':mesh::camera_position.z--;break;
        case 'e':logmisc=!logmisc;break;
        case ']':gui::selected_btn=(gui::selected_btn+1)%gui::selected_menu->btncount;break;
        case '\r':
          if(gui::selected_menu&&gui::selected_menu->funcs&&gui::selected_menu->funcs[gui::selected_btn]){
            gui::selected_menu->funcs[gui::selected_btn]();
          }break;//could do something with \n too but idk how the input formatting works
      }
      if(escapes&'\x01'){gamestate.paused=!gamestate.paused;}
    }
    if(c||escapes){
      clock_t t=clock();
      gui::clear_scr();
      gui::drawModel(model0);
      gui::drawModel(model1);
      gui::putMenu(&menu,1,1);
      gui::drawFrame();
      clock_t t1=clock()-t;
      float s=t1/(float)CLOCKS_PER_SEC;
      if(s>0.1){
        fprintf(debug,"took %.3fs to draw a frame!!!\n",s);
      }
      escapes=0;
    }
  }
  fclose(debug);
  return 0;
}//WHAT ARE YOU DOING
