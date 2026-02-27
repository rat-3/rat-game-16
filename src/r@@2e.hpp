#ifndef RATATOUILLE_H//we're just going to write a disgusting header only library
#define RATATOUILLE_H//ratatouille is now our linux-only version. will do portability with windows later.
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <type_traits>
#include <colors.hpp>
#define DO(x) if(x)
#define ORDIE(s) {::gui::stop(s);exit(1);}
#define BRKST(X,Y) if(::gui::state&STATE_ ## X){Y ::gui::state&=~STATE_ ## X;} //did not want to write out if(state&whatever) like 80 times
#define STATE_TERM 0b00000001//binary literals are only in c++
#define STATE_SIGS 0b00000010//would have to use hex in c
#define STATE_TBUF 0b00000100//which is lowkirkenuinely wild
#define STATE_CBUF 0b00001000
#define STATE_DBUF 0b00010000
#define STATE_ICLR 0b10000000
namespace gui {
  using namespace colors;
  typedef unsigned short int scoord;//coordinate on the screen, in characters
  typedef unsigned char sfrac;//represents the fraction of width/this

  const tcflag_t RAWMODE_LFLAGS=~(ECHO|ICANON|/*ISIG|*/IEXTEN),//remember that ~ is bitwise not
                 RAWMODE_IFLAGS=~(BRKINT|ICRNL|INPCK|ISTRIP|IXON),
                 RAWMODE_OFLAGS=~(OPOST);//terminal bits to set for "raw" mode
  const int BLOCKED_SIGS=SIGTTOU|SIGSTOP|SIGTTIN|SIGTSTP;

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
  unsigned char* depth_buffer=NULL;
  color_t* color_buffer=NULL;
  scoord max_chars=0;

  int set_term_flags(tcflag_t fl,tcflag_t fi,tcflag_t fo){
    cur_term_state.c_lflag&=fl;
    cur_term_state.c_iflag&=fi;
    cur_term_state.c_oflag&=fo;
    return (tcsetattr(STDIN_FILENO,TCSAFLUSH,&cur_term_state));
  }

  void stop(const char* err){
    if(state&STATE_ICLR){return;}
    BRKST(SIGS,
      if(sigprocmask(SIG_SETMASK,&old_sigset,NULL)==-1){perror("couldn't restore signal set");}
    )
    BRKST(TERM,
      if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&old_term_state)){perror("couldn't to restore terminal state");}
    )
    BRKST(TBUF,if(term_buffer){free(term_buffer);term_buffer=NULL;max_chars=0;})
    BRKST(CBUF,if(color_buffer){free(color_buffer);color_buffer=NULL;})
    BRKST(DBUF,if(depth_buffer){free(depth_buffer);depth_buffer=NULL;})
    if(err){perror(err);}
    state|=STATE_ICLR;
  }
  void stop() {state|=STATE_ICLR;stop(NULL);}

  void sig_handler(int sig){
    printf("bazinga%u",sig);
    FILE* g = fopen("log","w+");
    fprintf(g,"%u\n",sig);
    fclose(g);
  }

  void clear_scr() {
    for(scoord i=0;i<max_chars;i++){term_buffer[i]=' ';depth_buffer[i]=255;color_buffer[i]=default_color;}
  }

  void init(){
    //make sure we're not doing things twice. idiot.
    DO(state)ORDIE("couldn't init r@@2e: we already started");

    //get starting terminal state so we can put it back once we're done
    DO(tcgetattr(STDIN_FILENO,&old_term_state))ORDIE("coudln't get initial terminal state");
    atexit(stop);
    state|=STATE_TERM;

    //set various terminal flags
    cur_term_state=old_term_state;
    cur_term_state.c_cflag|=CS8;//make sure 8 bit width characters
    cur_term_state.c_cc[VMIN] =0;//double 0 means return asap and 0 if nothing's available
    cur_term_state.c_cc[VTIME]=0;
    DO(set_term_flags(RAWMODE_LFLAGS,RAWMODE_IFLAGS,RAWMODE_OFLAGS))ORDIE("couldn't set terminal state");

    //block the TTOU signal which triggers program stop when trying to write to terminal from a background process
    DO(sigemptyset(&cur_sigset)==-1)                      ORDIE("couldn't initialize empty signal set");
    DO(sigaddset(&cur_sigset,BLOCKED_SIGS)==-1)           ORDIE("coudln't add SIGTTOU to the block signal set");
    DO(sigprocmask(SIG_BLOCK,&cur_sigset,&old_sigset)==-1)ORDIE("couldn't block the signal SIGTTOU");
    state|=STATE_SIGS;

    //get some data about what the terminal looks like
    DO(ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_dims))ORDIE("couldn't get terminal dimensions");
    max_chars=term_dims.ws_col*term_dims.ws_row;
    DO((term_buffer=(char*)malloc(max_chars))==NULL)ORDIE("couldn't allocate for screen");//malloc because we clear it later with spaces instead of '\0'
    state|=STATE_TBUF;
    DO((color_buffer=(color_t*)malloc(max_chars*sizeof(color_t)))==NULL)ORDIE("couldn't allocate for colors");
    state|=STATE_CBUF;
    DO((depth_buffer=(unsigned char*)malloc(max_chars))==NULL)ORDIE("couldn't allocate for depth buffer");
    state|=STATE_DBUF;
    
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
  inline scoord toSSPX(T x,T d){return (scoord)((x/d+1)*term_dims.ws_col/2);}
  template<typename T> requires (std::is_arithmetic_v<T>)&&(std::is_signed_v<T>)
  inline scoord toSSPY(T y,T d){return (scoord)((y/d+1)*term_dims.ws_row/2);}
  inline scoord toSSPI(scoord x,scoord y){return (y*term_dims.ws_col)+x;}

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

  void drawFrame(){
    DO(fwrite("\x1b[2J\x1b[0;0H\x1b[0m",1,10,stdout)<10)ORDIE("couldn't write control codes to terminal");
    color_t last_color_fg=color_buffer[0]&0x0F;
    color_t last_color_bg=color_buffer[0]&0xF0;
    scoord last_char=0;
    for(scoord i=1;i<max_chars;i++){
      bool fg_change=((color_buffer[i]&0x0F)!=last_color_fg);
      bool bg_change=((color_buffer[i]&0xF0)!=last_color_bg);
      if(fg_change||bg_change){
        fwrite(term_buffer+last_char,1,i-last_char,stdout);
        if(fg_change){
          last_color_fg=color_buffer[i];
          if(bg_change){
            fprintf(stdout,"%s%s",ansi_fg(color_buffer[i]),ansi_bg(color_buffer[i]));
            fseek(stdout,-1,SEEK_CUR);
            last_color_bg=color_buffer[i];
          }else{
            fprintf(stdout,"%s",ansi_fg(color_buffer[i]));
            fseek(stdout,-1,SEEK_CUR);
          }
        }else{
          if(bg_change){
            fprintf(stdout,"%s",ansi_bg(color_buffer[i]));
            fseek(stdout,-1,SEEK_CUR);
            last_color_bg=color_buffer[i];
          }
        }
        last_char=i;
      }
    }
    fwrite(term_buffer+last_char,1,max_chars-last_char,stdout);
    // fseek(stdout,-1,SEEK_CUR);
    fflush(stdout);
  }
}
#undef DO
#undef ORDIE
#undef BRKST
#endif