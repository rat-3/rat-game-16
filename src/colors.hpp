// #define ansi_fg(x) x+30
// #define ansi_bg(x) x+40
namespace colors {
  typedef unsigned char color_t;//4 bits for fg and bg. bg lshift 4
  constexpr char color_reset=0;//chars as integers not chars
  enum color:char{//3 bit color 1 bit intensity
    black=0,red,green,brown,blue,purple,cyan,white,
    gray,bright_red,bright_green,yellow,bright_blue,bright_purple,bright_cyan,bright_white
  };
  constexpr color_t col(enum color fore,enum color back){return ((fore)|(back<<4));}
  constexpr color_t default_color=col(white,black);
  constexpr char* ansi_fg(char c){
    if(c&0x08){char* out=(char*)malloc(7);snprintf(out,8,"\x1b""[%.2d;1m",(c&0x07)+30);return out;}
    else{char* out=(char*)malloc(5);snprintf(out,6,"\x1b""[%.2dm",  (c&0x07)+30);return out;}
  }
  constexpr char* ansi_bg(char c){
    if(c&0x80){char* out=(char*)malloc(7);snprintf(out,8,"\x1b""[%.2d;1m",(c>>4)&0x07+40);return out;}
    else{char* out=(char*)malloc(5);snprintf(out,6,"\x1b""[%.2dm",  (c>>4)&0x07+40);return out;}
  }
}