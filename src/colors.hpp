#ifndef COLORS_H
#define COLORS_H
// #define ansi_fg(x) x+30
// #define ansi_bg(x) x+40
namespace colors {
  const char color_reset=0;//chars as integers not chars
  inline constexpr color_t col(enum color fore,enum color back){return ((fore)|((back&0x07)<<5));}
  constexpr color_t default_color=col(white,black);
  constexpr char* ansi_fg(color_t c,char* buf){//could prolly use optimization
    if(c&0x80){//bg bright
      if(c&0x08){snprintf(buf,6,"\x1b""[%.2dm",(c&0x07)+30);return buf;}//fg bright
      else{snprintf(buf,8,"\x1b""[%.2d;2m",  (c&0x07)+30);return buf;}//fg not bright
    }else{//bg not bright
      if(c&0x08){snprintf(buf,8,"\x1b""[%.2d;1m",(c&0x07)+30);return buf;}//fg bright
      else{snprintf(buf,6,"\x1b""[%.2dm",  (c&0x07)+30);return buf;}//fg not bright
    }
  }
  constexpr char* ansi_bg(color_t c,char* buf){
    snprintf(buf,6,"\x1b""[%.2dm",  ((c>>5)&0x07)+40);return buf;//something mean
  }
}
#endif
