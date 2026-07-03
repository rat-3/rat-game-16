#include <stdio.h>
FILE* debug;
typedef void (*function)(void);
bool logmisc=false;
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
  unsigned len,idx;
  puts("-- RAT GAME 16 DEBUG CLI --");
#define EXPS(S) if(strncmp(&input[idx],S,strlen(S))){printf("unrecognized input \"%s\", expected \"%s\"!\n",&input[idx],S);goto getInput;}else{idx+=strlen(S);}
getInput:
  printf("> ");
  fgets(input,127,stdin);
  len=strlen(input);
  input[len-1]='\0';
  idx=0;
  if(!strncmp(input,"quit",4)){
    exit(0);
  }else if(!strncmp(input,"read",4)){
    idx+=4;
    EXPS(" ");
    if(!strncmp(&input[idx],"tex",3)){
      idx+=3;
      EXPS(" ");
      assets::texture_t tex=assets::readPPM(&input[idx]);
      for(unsigned i=0;i<(tex.width*tex.height);i++){
        unsigned char r=tex.pixels[i*3];
        unsigned char g=tex.pixels[i*3+1];
        unsigned char b=tex.pixels[i*3+2];
        unsigned char c=(r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);//don't need to store brightness just calculate it as bool earlier
        colors::color_t color=colors::col((colors::color)c,colors::black);
        char buf[8];buf[7]='\0';
        fputs(colors::ansi_fg(color,buf),stdout);
        if(i&&!(i%tex.width)){
          puts("");
        }
        putc('#',stdout);
      }
      puts("\x1b[0m");
      free(tex.pixels);
    }else{
      printf("unrecognized input \"%s\", expected \"tex\" or\n",&input[idx]);
    }
  }else{
    printf("unrecognized input \"%s\", expected \"quit\" or \"read\" or\n",&input[idx]);
    
  }
  goto getInput;
}
