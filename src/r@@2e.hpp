#ifndef RATATOUILLE_H//we're just going to write a disgusting header only library
#define RATATOUILLE_H//ratatouille is now our linux-only version. will do portability with windows later.
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <types.hpp>
#include <colors.hpp>
// #include <assets.hpp>
#include <ctype.h>
#include <stdarg.h>
#define DO(x) if(x)
#define ORDIE(s) {::gui::stop(s);exit(1);}
#define BRKST(X,Y) if(::gui::state&STATE_ ## X){Y ::gui::state&=~STATE_ ## X;} //did not want to write out if(state&whatever) like 80 times
#define STATE_TERM 0b00000001//binary literals are only in c++
#define STATE_SIGS 0b00000010//would have to use hex in c
#define STATE_TBUF 0b00000100//which is lowkirkenuinely wild
#define STATE_CBUF 0b00001000
#define STATE_DBUF 0b00010000
#define STATE_BBUF 0b00100000
#define STATE_PMDS 0b01000000
#define STATE_ICLR 0b10000000
namespace gui {
  float screen_scale = tan(mesh::fov/2.0f);
  using namespace mesh;
  void updateFrustum(){
    planes[NEAR].D=nearplanex;
    planes[LEFT]   = (plane_t){vec3<float>{(float)cos(fov/2.0f),(float)sin(fov/2.0f),0},0};
    planes[RIGHT]  = (plane_t){vec3<float>{(float)cos(fov/2.0f),(float)-sin(fov/2.0f),0},0};
    planes[BOTTOM] = (plane_t){vec3<float>{(float)cos(fov/2.0f),0,(float)sin(fov/2.0f)},0};
    planes[TOP]    = (plane_t){vec3<float>{(float)cos(fov/2.0f),0,(float)-sin(fov/2.0f)},0};
  }
  void update_fov(){
    screen_scale=tan(mesh::fov/2.0f);
    updateFrustum();
  }
  menu_t* selected_menu;
  scoord selected_btn;
  using namespace colors;

  const tcflag_t RAWMODE_LFLAGS=~(ECHO|ICANON|/*ISIG|*/IEXTEN),//remember that ~ is bitwise not
                 RAWMODE_IFLAGS=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON),
                 RAWMODE_OFLAGS=~(OPOST);//terminal bits to set for "raw" mode
  const int BLOCKED_SIGS=SIGTTOU|SIGSTOP|SIGTTIN|SIGTSTP;

  assets::font_t f_default;

  char state='\0';//see macros for which bits mean what

  //information about the terminal's settings
  struct termios old_term_state{};
  struct termios cur_term_state{};
  struct timeval input_timeout{0,0};//wait for NOTHING

  //signals to make sure we can ignore things we don't like, like someone telling the program to stop
  sigset_t old_sigset;
  sigset_t cur_sigset;
  
  //screen data. once things get multithreaded, make volatile
  struct winsize term_dims;//represents current terminal dimensions. has fields ws_row and ws_col. should change
  char* term_buffer=NULL;
  char* term_buffer_alt=NULL;
  unsigned char* depth_buffer=NULL;
  color_t* color_buffer=NULL;
  color_t* color_buffer_alt=NULL;
#ifdef do_debug
  char* debug_buffer=NULL;
#endif
  scoord max_chars=0;

  __attribute__((format (printf,1,2))) void dbprintf(const char* str, ...){
    va_list args;
    fwrite("\x1b[0;0H\x1b[0m",1,10,stdout);
    va_start(args,str);
    vfprintf(stdout,str,args);
    vfprintf(debug,str,args);
    va_end(args);
    fflush(stdout);
  }
  #ifdef do_debug
    #define printd(...) ::gui::dbprintf(__VA_ARGS__)
  #else
    #define printd(...)
  #endif
  int set_term_flags(tcflag_t fl,tcflag_t fi,tcflag_t fo){
    cur_term_state.c_lflag&=fl;
    cur_term_state.c_iflag&=fi;
    cur_term_state.c_oflag&=fo;
    return (tcsetattr(STDIN_FILENO,TCSAFLUSH,&cur_term_state));
  }

  void stop(const char* err){
    if(state&STATE_ICLR){return;}
    BRKST(PMDS,printf("\x1b[c""\x1b""[?1049l\x1b[?25h");)//lowkirk don't know what the [c is for but
    printf("CURING TERMINAL ILLNESS\n\r");//it breaks without it
    BRKST(SIGS,
      printf("\x1b""[0mrestoring sigset\n\r");
      if(sigprocmask(SIG_SETMASK,&old_sigset,NULL)==-1){perror("couldn't restore signal set");}
    )
    BRKST(TERM,
      printf("restoring terminal state\n\r");
      if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&old_term_state)){perror("couldn't to restore terminal state");}
    )
    BRKST(TBUF,if(term_buffer){free(term_buffer);term_buffer=NULL;}if(term_buffer_alt){free(term_buffer_alt);term_buffer_alt=NULL;}max_chars=0;)
    BRKST(CBUF,if(color_buffer){free(color_buffer);color_buffer=NULL;}if(color_buffer_alt){free(color_buffer_alt);color_buffer_alt=NULL;})
    BRKST(DBUF,if(depth_buffer){free(depth_buffer);depth_buffer=NULL;})
    #ifdef do_debug
    BRKST(BBUF,if(debug_buffer){free(debug_buffer);debug_buffer=NULL;})
    #endif
    if(err){perror(err);}
    state|=STATE_ICLR;
  }
  void stop() {stop(NULL);}

  void sig_handler(int sig){//not using mode bit ISIG so doesn't proc (i think)
    FILE* g = fopen("log","w+");
    fprintf(g,"%u\n",sig);
    fclose(g);
  }

  void swap_bufs(){
    char* tmpt=term_buffer;
    color_t* tmpc=color_buffer;
    term_buffer=term_buffer_alt;
    color_buffer=color_buffer_alt;
    term_buffer_alt=tmpt;
    color_buffer_alt=tmpc;
  }

  void clear_scr() {
    for(scoord i=0;i<max_chars;i++){term_buffer[i]=' ';depth_buffer[i]=255;color_buffer[i]=default_color;}
  }

  void init(){
    //make sure we're not doing things twice. idiot.
    DO(state)ORDIE("couldn't init r@@2e: we already started");
    puts("INITIALIZING TERMINAL ILLNESS");
    atexit(stop);

    //get starting terminal state so we can put it back once we're done
    DO(tcgetattr(STDIN_FILENO,&old_term_state))ORDIE("coudln't get initial terminal state");

    //enable alternate screen buffer
    //https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797#common-private-modes
    printf("\x1b[?1049h\x1b[?25l");
    state|=STATE_PMDS;

    //set various terminal flags
    cur_term_state=old_term_state;
    cur_term_state.c_cflag|=CS8;//make sure 8 bit width characters
    cur_term_state.c_cc[VMIN] =0;//double 0 means return asap and 0 if nothing's available
    cur_term_state.c_cc[VTIME]=0;//(non blocking input reads)
    DO(set_term_flags(RAWMODE_LFLAGS,RAWMODE_IFLAGS,RAWMODE_OFLAGS))ORDIE("couldn't set terminal state");
    state|=STATE_TERM;

    //block the TTOU signal which triggers program stop when trying to write to terminal from a background process
    DO(sigemptyset(&cur_sigset)==-1)                      ORDIE("couldn't initialize empty signal set");
    DO(sigaddset(&cur_sigset,BLOCKED_SIGS)==-1)           ORDIE("coudln't add SIGTTOU to the block signal set");
    DO(sigprocmask(SIG_BLOCK,&cur_sigset,&old_sigset)==-1)ORDIE("couldn't block the signal SIGTTOU");
    state|=STATE_SIGS;

    //get some data about what the terminal looks like
    DO(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_dims))ORDIE("couldn't get terminal dimensions");
    max_chars=term_dims.ws_col*term_dims.ws_row;
#define tryalloc(A,B,C,D) DO((A=(B)malloc(C))==NULL)ORDIE("couldn't allocate for " #A);state|=STATE_ ## D;
    tryalloc(term_buffer,char*,max_chars,TBUF);
    tryalloc(term_buffer_alt,char*,max_chars,TBUF);
    tryalloc(color_buffer,color_t*,max_chars,CBUF);
    tryalloc(color_buffer_alt,color_t*,max_chars,CBUF);
    tryalloc(depth_buffer,unsigned char*,max_chars,DBUF);
    #ifdef do_debug
    tryalloc(debug_buffer,char*,term_dims.ws_col,BBUF);
    #endif
#undef tryalloc
    clear_scr();
    swap_bufs();
    clear_scr();
    // struct sigaction t;
    // DO(sigaction(SIGTTOU,&t,NULL)==-1)ORDIE("couldn't examine action for ttou"); //double check things work later
  }

  bool hasInput(){
    static fd_set fds;FD_SET(STDIN_FILENO,&fds);//ts shit is not thread safe probably
    return select(STDIN_FILENO+1, &fds, NULL, NULL,&input_timeout);
  }
  char readInput(){
    char out='\0';
    read(STDIN_FILENO, &out, 1);//mm write to stack always feels weird
    return out;
  }

  template<typename T> requires (std::is_arithmetic_v<T>)&&(std::is_signed_v<T>)
  inline const scoord toSSPX(T x,T d){return (scoord)(((x/d)*(term_dims.ws_col*screen_scale/2))+term_dims.ws_col/2);}
  template<typename T> requires (std::is_arithmetic_v<T>)&&(std::is_signed_v<T>)
  inline const scoord toSSPY(T y,T d){return (scoord)(((y/d)*(term_dims.ws_row*screen_scale/2))+term_dims.ws_row/2);}
  inline const scoord toSSPI(scoord x,scoord y){return min(y,term_dims.ws_row-1)*term_dims.ws_col+min(x,term_dims.ws_col-1);}

  char putChar(scoord x,scoord y,unsigned char c){
    scoord p=toSSPI(x,y);
    if(p>=max_chars){return '\0';}
    char d=term_buffer[p];
    term_buffer[p]=c;
    return d;
  }
  color_t putColor(scoord x,scoord y,color_t c){
    scoord p=toSSPI(x,y);
    if(p>=max_chars){return 0;}
    color_t d=color_buffer[p];
    color_buffer[p]=c;
    return d;
  }

  scoord putFText(assets::font_t* fontp,const char* text,unsigned int length,scoord x1,scoord y1,scoord width,scoord height,text_align align){
    if(!text||!*text){return 0;}
    assets::font_t* font=fontp?fontp:&gui::f_default;
    DO(!font)ORDIE("where's the default font")
    scoord x=x1,y=y1,w=0,lw=0;
    unsigned int last_char=0;
    #define align_stuff \
    if(align==gui::CENTER){\
      for(unsigned int k=0;k<font->sizey;k++){\
        memmove(&term_buffer[toSSPI(x1+(width-(x-x1))/2,y+k)],&term_buffer[toSSPI(x1,y+k)],x-x1);\
        memset(&term_buffer[toSSPI(x1,y+k)],' ',(width-(x-x1))/2);\
      }\
    }else if(align==gui::RIGHT){\
      for(unsigned int k=0;k<font->sizey;k++){\
        memmove(&term_buffer[toSSPI(x1+(width-x),y+k)],&term_buffer[toSSPI(x1,y+k)],x-x1);\
        memset(&term_buffer[toSSPI(x1,y+k)],' ',(width-x));\
      }\
    }
    for(unsigned int i=0;(i<=length)&&(y<(y1+height));i++){
      w+=font->sizex[(unsigned char)text[i]];
      if((w-lw)>width){
        if(x!=x1){
          align_stuff;
          y+=(font->sizey);
          x=x1;
          last_char++;
        }
        for(unsigned int j=last_char;j<i-1;j++){
          unsigned char sizex=font->sizex[(unsigned char)text[j]];
          for(unsigned int k=0;(k<font->sizey)&&((y+k)<(height+y1));k++){
            memcpy(
              &term_buffer[toSSPI(x,y+k)],
              &font->map[(unsigned char)text[j]][k*sizex],
              sizex);
            memset(&color_buffer[toSSPI(x,y+k)],default_color,sizex);
          }
          x+=sizex;
        }
        align_stuff;
        x=x1;y+=font->sizey;last_char=i-1;w=0;
        continue;
      }
      if((text[i]==' ')||(i==length)||(text[i]=='\n')){
        if(w>width){
          align_stuff;
          y+=font->sizey;x=x1;w-=lw;
          last_char++;
        }
        lw=w;
        for(unsigned int j=last_char;j<i;j++){
          unsigned char sizex=font->sizex[(unsigned char)text[j]];
          for(unsigned int k=0;(k<font->sizey)&&((y+k)<(height+y1));k++){
            memcpy(
              &term_buffer[toSSPI(x,y+k)],
              &font->map[(unsigned char)text[j]][k*sizex],
              sizex);
            memset(&color_buffer[toSSPI(x,y+k)],default_color,sizex);
          }
          x+=sizex;
        }
        
        if((text[i]=='\n')||(i==length)){
          align_stuff;
          do{
            i++;y+=font->sizey;
          }while(text[i]=='\n');
          x=x1;w=0;
        }
        last_char=i;
      }
    }
    return y;
#undef align_stuff
  }
  scoord putFText(gui::text_t text,scoord x,scoord y,scoord width,scoord height){
    return putFText(text.font,text.text,text.length,x,y,width,height,text.alignment);
  }

  void putMenu(menu_t* menu,scoord x,scoord y){//needs bounds checking
    scoord y1=y;
    scoord w=menu->sizex?min(menu->sizex,term_dims.ws_col-1-x):term_dims.ws_col-x-1;
    scoord h=menu->sizey?min(menu->sizey,term_dims.ws_row-1-y):term_dims.ws_row-y-1;
    putChar(x,y,menu->borders[4]);
    putChar(x+w,y,menu->borders[4]);
    putChar(x,y+h,menu->borders[4]);
    putChar(x+w,y+h,menu->borders[4]);
    for(scoord i=y+1;i<y+h;i++){//fix all those toSSPI calls and replace with like 2 calculations
      putChar(x,i,menu->borders[2]);
      color_buffer[toSSPI(x,i)]=default_color;
      putChar(x+w,i,menu->borders[3]);
      color_buffer[toSSPI(x+w,i)]=default_color;
    }
    memset(&term_buffer[toSSPI(x+1,y)],menu->borders[0],w-1);
    memset(&color_buffer[toSSPI(x,y)],default_color,w);
    memset(&term_buffer[toSSPI(x+1,y+h)],menu->borders[1],w-1);
    memset(&color_buffer[toSSPI(x,y+h)],default_color,w);
    y++;
    for(scoord i=0;(i<menu->textcount)&&(y<(h+y1));i++){
      y=putFText(menu->items[i],x+1,y,w,h+y1-y);
    }
    for(scoord i=0;(i<menu->btncount)&&(y<(h+y1));i++){
      scoord y2=y;
      y=putFText(menu->buttons[i],x+3,y,w,h+y1-y);//i know why it works
      y2+=(y-y2-1)/2;
      putChar(x+1,y2,(i==selected_btn)?'>':'-');
      putChar(x+2,y2,' ');
      color_buffer[toSSPI(x+1,y2)]=default_color;
      color_buffer[toSSPI(x+2,y2)]=default_color;
    }
  }
  void putSprite(assets::sprite_t* sprite,scoord x,scoord y){
    if((x>term_dims.ws_col)||(y>term_dims.ws_row)){return;}
    for(scoord y1=0;(y1<term_dims.ws_row-y)&&(y1<sprite->height);y1++){
      // c=0;while(!isprint(sprite->chars[y*sprite->width+c])){c++;/*holy shit lois*/}
      // memcpy(&term_buffer[toSSPI(x,y1+y)],&sprite->chars[y1*sprite->width],min(sprite->width,(unsigned)(term_dims.ws_col-x)));
      for(scoord x1=0;(x1<sprite->width)&&(x1<(term_dims.ws_col-x));x1++){
        if(isprint(sprite->chars[y1*sprite->width+x1])){
          unsigned char r=sprite->pixels[3*((y1*sprite->width)+x1)],
          g=sprite->pixels[3*((y1*sprite->width)+x1)+1],
          b=sprite->pixels[3*((y1*sprite->width)+x1)+2];
          char c=(r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);
          putColor(x1+x,term_dims.ws_row-y1+y-1,colors::col((colors::color)c,colors::black));
          putChar(x1+x,term_dims.ws_row-y1+y-1,sprite->chars[y1*sprite->width+x1]);
        }
      }
    }
  }

  void drawFrame(){
    // DO(fwrite("\x1b[2J\x1b[0;0H\x1b[0m",1,10,stdout)<10)ORDIE("couldn't write control codes to terminal");
    color_t lcolfg=color_buffer[0]&0x0F;//why the FUCK would he have bits 3 and 4 be
    color_t lcolbg=color_buffer[0]&0xF0;//brightness instead of 3 and 7 like a normal person
    scoord lchar;
    char* buf=(char*)malloc(8);
    buf[7]='\0';
    for(scoord i=0;i<max_chars;i++){
      if((term_buffer[i]!=term_buffer_alt[i])||(color_buffer[i]!=color_buffer_alt[i])){
        fprintf(stdout,"\x1b[%u;%uH",i/term_dims.ws_col,i%term_dims.ws_col);
        lchar=i;lcolfg=color_buffer[i]&0x0F;lcolbg=color_buffer[i]&0xF0;
        scoord three=0;
        do{
          i++;
          if(color_buffer[i]==color_buffer_alt[i]){
            if(term_buffer[i]==term_buffer_alt[i]){
              three++;
            }
          }else{
            three=0;
            fwrite(&term_buffer[lchar],1,i-lchar,stdout);
            if(lcolfg!=(color_buffer[i]&0x0F)){
              lcolfg=color_buffer[i]&0x0F;
              fputs(ansi_fg(lcolfg,buf),stdout);
            }
            if(lcolbg!=(color_buffer[i]&0xF0)){
              lcolbg=color_buffer[i]&0xF0;
              fputs(ansi_bg(lcolbg,buf),stdout);
            }
            lchar=i;
          }
        }while(three<8);
      }
    }
    free(buf);
    // fseek(stdout,-1,SEEK_CUR);
    fflush(stdout);
    swap_bufs();
  }
}
#undef DO
#undef ORDIE
#undef BRKST
#endif

