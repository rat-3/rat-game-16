#ifndef TYPES_H
#define TYPES_H
#include <cstring>
#include <cstdlib>
#include <math.h>
namespace colors {
  enum color:char{//3 bit color 1 bit intensity
    black=0,red,green,brown,blue,purple,cyan,white,
    gray,bright_red,bright_green,yellow,bright_blue,bright_purple,bright_cyan,bright_white
  };
  typedef unsigned char color_t;//4 bits for fg and bg. bg lshift 4
}
namespace assets {
  struct texture_t{
    unsigned int width, height;
    unsigned char* pixels;
  };
  struct sprite_t:texture_t{//SFAWTDE or SFINAE or something
    char* chars;
  };
  #define UPPER(f) &f.map[(unsigned char)'A']
  #define LOWER(f) &f.map[(unsigned char)'a']
  #define SPECIAL(f) &f.map[(unsigned char)'!']
  #define NUMBERS(f) &f.map[(unsigned char)'0']
  #define SPECIAL2(f) &f.map[(unsigned char)'[']
  #define SPECIAL3(f) &f.map[(unsigned char)'{']
  struct font_t{
    unsigned char sizey;
    unsigned char* sizex;
    char** map;
  };
}
namespace gui {
  typedef unsigned short int scoord;//coordinate on the screen, in characters
  enum text_align{
    LEFT,CENTER,RIGHT//tbd
  };
  struct text_t{
    assets::font_t* font;
    const char* text;
    scoord length;
    text_align alignment;
  };
  struct menu_t{
    scoord sizex,sizey;
    char borders[5];//up/down/left/right/corner
    scoord textcount;
    text_t* items;
    scoord btncount;
    text_t* buttons;
    void (**funcs)();
  };
}
namespace mesh {
  typedef float mesh_size;
  template<typename T> requires arith<T>&&comp<T> struct vec2 {
    T x,y;//these operators are fuckin wild
    template<typename U> auto constexpr operator+(const vec2<U>& v)const{return (vec2<decltype(std::declval<T>()+std::declval<U>())>){x+v.x,y+v.y};}
    template<typename U> auto constexpr operator+(const U& v)const{return (vec2<decltype(std::declval<T>()+std::declval<U>())>){x+v,y+v};}
    template<typename U> auto constexpr operator-(const vec2<T>& v)const{return (vec2<decltype(std::declval<T>()-std::declval<U>())>){x-v.x,y-v.y};}
    template<typename U> auto constexpr operator-(const U& v)const{return (vec2<decltype(std::declval<T>()-std::declval<U>())>){x-v,y-v};}
    template<typename U> auto constexpr operator*(const vec2<U>& v)const{return (vec2<decltype(std::declval<T>()*std::declval<U>())>){x*v.x,y*v.y};}
    template<typename U> auto constexpr operator*(const U& v)const{return (vec2<decltype(std::declval<T>()*std::declval<U>())>){x*v,y*v};}
    template<typename U> auto constexpr operator/(const vec2<U>& v)const{return (vec2<decltype(std::declval<T>()/std::declval<U>())>){x/v.x,y/v.y};}
    template<typename U> auto constexpr operator/(const U& v)const{return (vec2<decltype(std::declval<T>()/std::declval<U>())>){x/v,y/v};}
    template<typename U> constexpr operator U()const;
    auto constexpr magnitude()const{return sqrt((x*x)+(y*y));}
    auto constexpr total()const{return x+y;}
  };//all of these should implement https://cplusplus.com/reference/type_traits/is_nothrow_move_constructible/
  template<typename T> requires arith<T>&&comp<T> struct vec3 {
    T x,y,z;
    template<typename U> auto constexpr operator+(const vec3<U>& v)const{return (vec3<decltype(std::declval<T>()+std::declval<U>())>){x+v.x,y+v.y,z+v.z};}
    template<typename U> auto constexpr operator+(const U& v)const{return (vec3<decltype(std::declval<T>()+std::declval<U>())>){x+v,y+v,z+v};}
    template<typename U> auto constexpr operator-(const vec3<U>& v)const{return (vec3<decltype(std::declval<T>()-std::declval<U>())>){x-v.x,y-v.y,z-v.z};}
    template<typename U> auto constexpr operator-(const U& v)const{return (vec3<decltype(std::declval<T>()-std::declval<U>())>){x-v,y-v,z-v};}
    template<typename U> auto constexpr operator*(const vec3<U>& v)const{return (vec3<decltype(std::declval<T>()*std::declval<U>())>){x*v.x,y*v.y,z*v.z};}
    template<typename U> auto constexpr operator*(const U& v)const{return (vec3<decltype(std::declval<T>()*std::declval<U>())>){x*v,y*v,z*v};}
    template<typename U> auto constexpr operator/(const vec3<U>& v)const{return (vec3<decltype(std::declval<T>()/std::declval<U>())>){x/v.x,y/v.y,z/v.z};}
    template<typename U> auto constexpr operator/(const U& v)const{return (vec3<decltype(std::declval<T>()/std::declval<U>())>){x/v,y/v,z/v};}
    template<typename U> auto constexpr cross(const vec3<U>& v)const{return (vec3<decltype(std::declval<T>()*std::declval<U>()-std::declval<T>()*std::declval<U>())>){y*v.z-z*v.y,z*v.x-x*v.z,x*v.y-y*v.x};}
    auto constexpr magnitude()const{return sqrt((x*x)+(y*y)+(z*z));}
    auto constexpr total()const{return x+y+z;}
  };
  template<typename T> struct vec_inner;//partial template specialization
  template<typename T> struct vec_inner<vec2<T>>{using type=T;};
  template<typename T> struct vec_inner<vec3<T>>{using type=T;};
  template<typename T> using  vec_inner_t=typename vec_inner<T>::type;
  template<typename T> requires arith<T>&&comp<T> template<typename U> constexpr mesh::vec2<T>::operator U() const {return vec2<vec_inner_t<U>>{(vec_inner_t<U>)x,(vec_inner_t<U>)y};}
  template<typename T> requires arith<T>&&comp<T> struct tri2 {
    vec2<T> a,b,c;
    template<typename U> auto constexpr operator+(const tri2<U>& t)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+t.a,b+t.b,c+t.c};}
    template<typename U> auto constexpr operator+(const vec2<U>& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator+(const U& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()+std::declval<U>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator-(const tri2<U>& t)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()-std::declval<vec2<U>>())>>){a-t.a,b-t.b,c-t.c};}
    template<typename U> auto constexpr operator-(const vec2<U>& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()-std::declval<vec2<U>>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator-(const U& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()-std::declval<U>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator*(const tri2<U>& t)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()*std::declval<vec2<U>>())>>){a*t.a,b*t.b,c*t.c};}
    template<typename U> auto constexpr operator*(const vec2<U>& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()*std::declval<vec2<U>>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator*(const U& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()*std::declval<U>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator/(const tri2<U>& t)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()/std::declval<vec2<U>>())>>){a/t.a,b/t.b,c/t.c};}
    template<typename U> auto constexpr operator/(const vec2<U>& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()/std::declval<vec2<U>>())>>){a/v,b/v,c/v};}
    template<typename U> auto constexpr operator/(const U& v)const{return (tri2<vec_inner_t<decltype(std::declval<vec2<T>>()/std::declval<U>())>>){a/v,b/v,c/v};}
  };//welcome to templat hell
  template<typename T> requires arith<T>&&comp<T> struct tri3 {
    vec3<T> a,b,c;//my compiler is going to blow its brains out
    template<typename U> auto constexpr operator+(const tri3<U>& t)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()+std::declval<vec3<U>>())>>){a+t.a,b+t.b,c+t.c};}
    template<typename U> auto constexpr operator+(const vec3<U>& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()+std::declval<vec3<U>>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator+(const U& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()+std::declval<U>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator-(const tri3<U>& t)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()-std::declval<vec3<U>>())>>){a-t.a,b-t.b,c-t.c};}
    template<typename U> auto constexpr operator-(const vec3<U>& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()-std::declval<vec3<U>>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator-(const U& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()-std::declval<U>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator*(const tri3<U>& t)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()*std::declval<vec3<U>>())>>){a*t.a,b*t.b,c*t.c};}
    template<typename U> auto constexpr operator*(const vec3<U>& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()*std::declval<vec3<U>>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator*(const U& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()*std::declval<U>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator/(const tri3<U>& t)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()/std::declval<vec3<U>>())>>){a/t.a,b/t.b,c/t.c};}
    template<typename U> auto constexpr operator/(const vec3<U>& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()/std::declval<vec3<U>>())>>){a/v,b/v,c/v};}
    template<typename U> auto constexpr operator/(const U& v)const{return (tri3<vec_inner_t<decltype(std::declval<vec3<T>>()/std::declval<U>())>>){a/v,b/v,c/v};}
  };
  struct meshtri:tri3<mesh_size>{
    vec2<float> uv0, uv1, uv2;
    template<typename U> auto constexpr operator+(const tri3<U>& t)const{return (meshtri){a+t.a,b+t.b,c+t.c,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator+(const vec3<U>& v)const{return (meshtri){a+v,b+v,c+v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator+(const U& v)const{return (meshtri){a+v,b+v,c+v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator-(const tri3<U>& t)const{return (meshtri){a-t.a,b-t.b,c-t.c,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator-(const vec3<U>& v)const{return (meshtri){a-v,b-v,c-v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator-(const U& v)const{return (meshtri){a-v,b-v,c-v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator*(const tri3<U>& t)const{return (meshtri){a*t.a,b*t.b,c*t.c,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator*(const vec3<U>& v)const{return (meshtri){a*v,b*v,c*v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator*(const U& v)const{return (meshtri){a*v,b*v,c*v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator/(const tri3<U>& t)const{return (meshtri){a/t.a,b/t.b,c/t.c,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator/(const vec3<U>& v)const{return (meshtri){a/v,b/v,c/v,uv0,uv1,uv2};}
    template<typename U> auto constexpr operator/(const U& v)const{return (meshtri){a/v,b/v,c/v,uv0,uv1,uv2};}
  };
  struct model_t {
    short unsigned int tricount;
    meshtri* tris;
  };
}
namespace assets{
  struct asset3d_t{
    mesh::model_t mesh;
    unsigned char* tex_binds;
    assets::texture_t* textures;
  };
}
#endif
