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
#define DO(x) if(x)
#define ORDIE(s) {::gui::stop(s);exit(1);}
#define STATE_TERM 0b00000001//binary literals are only in c++
#define STATE_SIGS 0b00000010//would have to use hex in c
#define STATE_TBUF 0b00000100
#define STATE_LBUF 0b00001000
#define STATE_ICLR 0b10000000
#define IFST(X) if(::gui::state&STATE_ ## X) //did not want to write out if(state&whatever) like 80 times
namespace gui {
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
  sigset_t old_sigset{};
  sigset_t cur_sigset{};
  
  //screen data. once things get multithreaded, make volatile
  struct winsize term_dims;//represents current terminal dimensions. has fields ws_row and ws_col
  char* term_buffer=NULL;
  scoord* line_lengths=NULL;
  scoord max_chars=0;

  int set_term_flags(tcflag_t fl,tcflag_t fi,tcflag_t fo){
    cur_term_state.c_lflag&=fl;
    cur_term_state.c_iflag&=fi;
    cur_term_state.c_oflag&=fo;
    return (tcsetattr(STDIN_FILENO,TCSAFLUSH,&cur_term_state));
  }

  void stop(const char* err){
    IFST(ICLR){return;}
    IFST(SIGS){
      if(sigprocmask(SIG_SETMASK,&old_sigset,NULL)==-1){perror("couldn't restore signal set");}
      state&=~STATE_SIGS;
    }
    IFST(TERM){
      if(tcsetattr(STDIN_FILENO,TCSAFLUSH,&old_term_state)){perror("couldn't to restore terminal state");}
      state&=~STATE_TERM;
    }
    IFST(TBUF){
      if(term_buffer){free(term_buffer);term_buffer=NULL;max_chars=0;}
      state&=~STATE_TBUF;
    }
    IFST(LBUF){
      if(line_lengths){free(line_lengths);}
      state&=!STATE_LBUF;
    }
    if(err){
      perror(err);
    }
    state|=STATE_ICLR;
  }
  void stop() {state|=STATE_ICLR;stop(NULL);}

  void sig_handler(int sig){
    printf("bazinga%u",sig);
    FILE* g = fopen("log","w+");
    fprintf(g,"%u\n",sig);
    fclose(g);
  }

  void clear_scr() {for(scoord i=0;i<max_chars;i++){term_buffer[i]=' ';}}

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
    DO((term_buffer=(char*)malloc(max_chars))==NULL)ORDIE("couldn't allocate enough for screen");
    state|=STATE_TBUF;
    DO((line_lengths=(scoord*)calloc(term_dims.ws_row,sizeof(scoord)))==NULL)ORDIE("couldn't allocate enough for screen");
    state|=STATE_LBUF;
    
    clear_scr();
    struct sigaction t;
    DO(sigaction(SIGTTOU,&t,NULL)==-1)ORDIE("couldn't examine action for ttou");
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

  char putChar(scoord x,scoord y,char c){
    scoord p=toSSPI(x,y);
    if(p>=max_chars){return '\0';}
    char d=term_buffer[p];
    term_buffer[p]=c;
    return d;
  }

  void drawFrame(){
    DO(fwrite("\x1b[2J\x1b[0;0H",1,10,stdout)<10)ORDIE("couldn't write control codes to terminal");
    DO(fwrite(term_buffer,1,max_chars,stdout)<max_chars)ORDIE("couldn't write screen to terminal");
  }
}
#undef DO
#undef ORDIE
#undef IFST
#endif