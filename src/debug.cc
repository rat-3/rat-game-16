#include <stdio.h>
#include <sys/ioctl.h>
#include <ctype.h>
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
int main(void){
  char* input=(char*)malloc(128);
  input[127]='\0';
  unsigned len,idx;
  puts("-- RAT GAME 16 DEBUG CLI --");
  #define EXPS(S) if(strncmp(&input[idx],S,strlen(S))){printf("unrecognized input \"%s\", expected \"%s\"!\n",&input[idx],S);goto getInput;}else{idx+=strlen(S);}
  getInput:
  printf("> ");
  fgets(input,127,stdin);
  len=strlen(input);
  input[len-1]='\0';
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
      if((argsread=sscanf(&input[idx],"%hux%hu%n",&width,&height,&chrsread))!=2){
        printf("expected width and height in the form of \"123x123\",instead got \"%.*s\"(at %u,length %i:%hu/2)\n",chrsread,&input[idx],idx,chrsread,argsread);
        goto getInput;
      }
      struct winsize term_dims;
      ioctl(STDOUT_FILENO, TIOCGWINSZ, &term_dims);
      if(width>term_dims.ws_col){
        printf("provided dimensions too wide for your terminal! (%hu>%hu)\n",width,term_dims.ws_col);
        goto getInput;
      }
      idx+=chrsread;
      EXPS(" ");
      assets::sprite_t tex=assets::readRGVTX(&input[idx],width,height);
      puts("vvv");
      for(unsigned i=(tex.width*tex.height)-1;i>0;i--){
        unsigned char r=tex.pixels[i*3];
        unsigned char g=tex.pixels[i*3+1];
        unsigned char b=tex.pixels[i*3+2];
        unsigned char c=(r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);
        colors::color_t color=colors::col((colors::color)c,colors::black);
        char buf[8];buf[7]='\0';
        fputs(colors::ansi_fg(color,buf),stdout);
        putc(isprint(tex.chars[i])?tex.chars[i]:' ',stdout);
        if(i&&!(i%tex.width)){puts("<");}
      }
      puts("\x1b[0m");
      free(tex.pixels);free(tex.chars);
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
  goto getInput;
}
