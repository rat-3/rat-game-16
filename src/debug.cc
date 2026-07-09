#include <stdio.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <termios.h>
FILE* debug;
typedef void (*function)(void);
bool logmisc=false,shouldredraw=false;
#include <type_traits>
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
#include <colors.hpp>
#define HSTRY_MAX 8
struct termios prev_term_state;
void reset_term_state(){
  tcsetattr(STDIN_FILENO,TCSAFLUSH,&prev_term_state);
}
int main(void){
  tcgetattr(STDIN_FILENO,&prev_term_state);
  atexit(reset_term_state);
  struct termios new_term_state=prev_term_state;
  new_term_state.c_iflag&=~(IGNCR);
  new_term_state.c_lflag&=~(ICANON|IEXTEN|ECHO|ECHOE);
  new_term_state.c_oflag|=ONLCR;
  new_term_state.c_cflag|=CS8;
  tcsetattr(STDIN_FILENO,TCSANOW,&new_term_state);
  char* prev_inputs[HSTRY_MAX];
  for(char i=0;i<HSTRY_MAX;i++){
    prev_inputs[i]=(char*)calloc(0,128);//program lifetime scope, don't worry about free
  }
  unsigned len,idx,hstry_idx=0,hstry_seek_idx=0;;
  puts("-- RAT GAME 16 DEBUG CLI --");
  #define EXPS(S) if(strncmp(&input[idx],S,strlen(S))){printf("unrecognized input \"%s\", expected \"%s\"!\n",&input[idx],S);goto get_input;}else{idx+=strlen(S);}
  #define input prev_inputs[hstry_idx]
  #define reprint_input printf("\r\x1b[K> %s",input);if(hstry_idx!=hstry_seek_idx){printf("\t(%i)\x1b[%iG",hstry_seek_idx-hstry_idx,idx+3);}else{printf("\x1b[%iG",idx+3);}
  get_input:
  hstry_idx=(hstry_idx+1)%HSTRY_MAX;
  hstry_seek_idx=hstry_idx;
  memset(input,'\0',127);
  reprint_input;
  for(idx=0;idx<127;){
    char c=getc(stdin);
    if(!c){continue;}
    switch(c){
      case '\e':
        switch(c=getc(stdin)){
          case '[':break;
          case '~':printf("\nwhat are you inserting at\n");goto get_input;
          default:
          printf("\nUnsupported escape :(\n",c);goto get_input;
          break;
        }
        switch(c=getc(stdin)){
          case 'A'://up
            hstry_seek_idx=(hstry_seek_idx+HSTRY_MAX-1)%HSTRY_MAX;
            if(hstry_idx==hstry_seek_idx){memset(input,'\0',127);}else{strncpy(input,prev_inputs[hstry_seek_idx],127);}
            idx=strlen(input);
            reprint_input;
          break;
          case 'B'://down
            hstry_seek_idx=(hstry_seek_idx+1)%HSTRY_MAX;
            if(hstry_idx==hstry_seek_idx){memset(input,'\0',127);}else{strncpy(input,prev_inputs[hstry_seek_idx],127);}
            idx=strlen(input);
            reprint_input;
          break;
          case 'C'://right
            idx=(idx<strlen(input))?idx+1:idx;
            reprint_input;
          break;
          case 'D'://left
            idx=(idx>0)?idx-1:idx;
            reprint_input;
          break;
          default:
            if((c=getc(stdin))!='['){
              printf("\nUnsupported escape :(\n",c);goto get_input;
            }
        }
      break;
      case '\n'://\r mapped to \n by icrnl
        len=strlen(input);putc('\n',stdout);goto process_input;
      break;
      case '\b':
      case '\x7f':
        idx=(idx>0)?idx-1:idx;memmove(&input[idx],&input[idx+1],strlen(&input[idx+1])+1);reprint_input;
      break;
      default:
        if(input[idx]){
          memmove(&input[idx+1],&input[idx],strlen(&input[idx]));
          input[idx]=c;idx++;
          reprint_input;
        }else{
          input[idx]=c;idx++;
        }
        reprint_input;
      break;
    }
  }
  process_input:
  idx=0;
  if(!strncmp(input,"quit",5)||!strncmp(input,"q",2)){
    exit(0);
  }else if(!strncmp(input,"read",4)){
    idx+=4;
    EXPS(" ");
    if(!strncmp(&input[idx],"vectex",6)){
      idx+=6;
      rgvtx:
      short unsigned width=0,height=0,argsread=0;
      int chrsread=0;
      EXPS(" ");
      struct winsize term_dims;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_dims);
      if(!strncmp(&input[idx],"full",4)){
        width=term_dims.ws_col;height=term_dims.ws_row;idx+=4;
      }else if((argsread=sscanf(&input[idx],"%hux%hu%n",&width,&height,&chrsread))!=2){
        printf("expected width and height in the form of \"123x123\" or \"full\",instead got \"%.*s\"(at %u,length %i:%hu/2)\n",chrsread,&input[idx],idx,chrsread,argsread);
        goto get_input;
      }
      if(width>term_dims.ws_col){
        printf("provided dimensions too wide for your terminal! (%hu>%hu)\n",width,term_dims.ws_col);
        goto get_input;
      }
      idx+=chrsread;
      EXPS(" ");
      assets::sprite_t tex=assets::readRGVTX(&input[idx],width,height);
      for(unsigned i=(tex.width*tex.height)-1;i>0;i--){
        unsigned char r=tex.pixels[i*3];
        unsigned char g=tex.pixels[i*3+1];
        unsigned char b=tex.pixels[i*3+2];
        unsigned char c=(r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);
        colors::color_t color=colors::col((colors::color)c,colors::black);
        char buf[8];buf[7]='\0';
        fputs(colors::ansi_fg(color,buf),stdout);
        putc(isprint(tex.chars[i])?tex.chars[i]:' ',stdout);
        if(i&&!(i%tex.width)){puts("");}
      }
      puts("\x1b[0m");
      free(tex.pixels);free(tex.chars);
    }else if(!strncmp(&input[idx],"sprite",6)){
      idx+=6;
      goto rgvtx;
    }else if(!strncmp(&input[idx],"rgvtx",5)){
      idx+=5;
      goto rgvtx;
    }else if(!strncmp(&input[idx],"tex",3)){
      idx+=3;
      EXPS(" ");
      assets::texture_t tex=assets::readPPM(&input[idx]);
      struct winsize term_dims;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_dims);
      if(term_dims.ws_col>=tex.width){
        for(unsigned i=0;i<(tex.width*tex.height);i++){
          unsigned char r=tex.pixels[i*3];
          unsigned char g=tex.pixels[i*3+1];
          unsigned char b=tex.pixels[i*3+2];
          unsigned char c=(r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);
          colors::color_t color=colors::col((colors::color)c,colors::black);
          char buf[8];buf[7]='\0';
          fputs(colors::ansi_fg(color,buf),stdout);
          if(i&&!(i%tex.width)){puts("");}
          putc('#',stdout);
        }
        puts("\x1b[0m");
      }else{
        printf("texture is too wide for your terminal! remind me to add a way to scale it or implement it yourself (%hu>%hu)\n",tex.width,term_dims.ws_col);
      }
      free(tex.pixels);
    }else{
      printf("unrecognized input \"%s\", expected \"tex\" or\n",&input[idx]);
    }
  }else{
    printf("unrecognized input \"%s\", expected \"quit\" or \"read\" or\n",&input[idx]);
  }
  goto get_input;
}
