#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.h>
template<typename T> concept arith=std::is_arithmetic_v<T>;
template<typename T> concept comp =requires(T a,T b){a<b;a>b;a==b;};
template<typename T> T min(T a,T b){return a<b?a:b;}
template<typename T> T max(T a,T b){return a<b?b:a;}
template<typename T,typename...U> requires comp<T>&&(comp<U>&&...)
T min(T t, U...a){
  T b=min(a...);
  return t<b?t:b;
}
template<typename T,typename...U> requires comp<T>&&(comp<U>&&...)
T max(T t, U...a){
  T b=max(a...);
  return t<b?b:t;
}
template<arith T>
inline std::make_signed_t<T> triarea(T x0,T y0,T x1,T y1,T x2,T y2){
  using sT=std::make_signed_t<T>;
  return((x0 * ((sT)y1-y2)) + (x1 * ((sT)y2-y0)) + (x2 * ((sT)y0-y1)));
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
  };//all of these should implement https://cplusplus.com/reference/type_traits/is_nothrow_move_constructible/
  template<typename T> struct vec2_inner;//partial template specialization
  template<typename T> struct vec2_inner<vec2<T>>{using type=T;};
  template<typename T> using  vec2_inner_t=typename vec2_inner<T>::type;//T is a vec2 here
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
  };
  template<typename T> struct vec3_inner;
  template<typename T> struct vec3_inner<vec3<T>>{using type=T;};
  template<typename T> using  vec3_inner_t=typename vec3_inner<T>::type;
  template<typename T> requires arith<T>&&comp<T> struct tri2 {
    vec2<T> a,b,c;
    template<typename U> auto constexpr operator+(const tri2<U>& t)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+t.a,b+t.b,c+t.c};}
    template<typename U> auto constexpr operator+(const vec2<U>& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator+(const U& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()+std::declval<U>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator-(const tri2<U>& t)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()-std::declval<vec2<U>>())>>){a-t.a,b-t.b,c-t.c};}
    template<typename U> auto constexpr operator-(const vec2<U>& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()-std::declval<vec2<U>>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator-(const U& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()-std::declval<U>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator*(const tri2<U>& t)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()*std::declval<vec2<U>>())>>){a*t.a,b*t.b,c*t.c};}
    template<typename U> auto constexpr operator*(const vec2<U>& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()*std::declval<vec2<U>>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator*(const U& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()*std::declval<U>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator/(const tri2<U>& t)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()/std::declval<vec2<U>>())>>){a/t.a,b/t.b,c/t.c};}
    template<typename U> auto constexpr operator/(const vec2<U>& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()/std::declval<vec2<U>>())>>){a/v,b/v,c/v};}
    template<typename U> auto constexpr operator/(const U& v)const{return (tri2<vec2_inner_t<decltype(std::declval<vec2<T>>()/std::declval<U>())>>){a/v,b/v,c/v};}
  };
  vec3_inner_t<vec3<float>> n;
  template<typename T> requires arith<T>&&comp<T> struct tri3 {
    vec3<T> a,b,c;//my compiler is going to blow its brains out
    template<typename U> auto constexpr operator+(const tri3<U>& t)const{return (tri3<vec3_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+t.a,b+t.b,c+t.c};}
    template<typename U> auto constexpr operator+(const vec3<U>& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec2<T>>()+std::declval<vec2<U>>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator+(const U& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()+std::declval<U>())>>){a+v,b+v,c+v};}
    template<typename U> auto constexpr operator-(const tri3<U>& t)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()-std::declval<vec3<U>>())>>){a-t.a,b-t.b,c-t.c};}
    template<typename U> auto constexpr operator-(const vec3<U>& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()-std::declval<vec3<U>>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator-(const U& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()-std::declval<U>())>>){a-v,b-v,c-v};}
    template<typename U> auto constexpr operator*(const tri3<U>& t)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()*std::declval<vec3<U>>())>>){a*t.a,b*t.b,c*t.c};}
    template<typename U> auto constexpr operator*(const vec3<U>& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()*std::declval<vec3<U>>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator*(const U& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()*std::declval<U>())>>){a*v,b*v,c*v};}
    template<typename U> auto constexpr operator/(const tri3<U>& t)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()/std::declval<vec3<U>>())>>){a/t.a,b/t.b,c/t.c};}
    template<typename U> auto constexpr operator/(const vec3<U>& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()/std::declval<vec3<U>>())>>){a/v,b/v,c/v};}
    template<typename U> auto constexpr operator/(const U& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()/std::declval<U>())>>){a/v,b/v,c/v};}
  };
  struct meshtri:tri3<mesh_size>{
    // short unsigned int texture_id;
    // vec2<short unsigned int> uv;vec2<short unsigned int> texdims;
  };
  struct model_t {
    short unsigned int tricount;
    meshtri* tris;
    const char* name;
   ~model_t() noexcept {free(tris);}
  };
  int dumptris(char* buffer,size_t buf_size,model_t* m){
    int a=snprintf(buffer,buf_size,"%s:%u triangles:",m->name,m->tricount);
    for(int i=0;i<m->tricount;i++){
      if(a>buf_size){break;}
      a+=snprintf(&buffer[a],buf_size-a,"triangle((%f,%f,%f),(%f,%f,%f),(%f,%f,%f)),",
        m->tris[i].a.x,
        m->tris[i].a.y,
        m->tris[i].a.z,
        m->tris[i].b.x,
        m->tris[i].b.y,
        m->tris[i].b.z,
        m->tris[i].c.x,
        m->tris[i].c.y,
        m->tris[i].c.z);
    }
    return a;
  }
  vec3<mesh_size> camera{-5.0f,0.0f,0.0f};
}
namespace gui {
  using namespace mesh;
  vec2<scoord> toSSPV(vec3<mesh_size> v){
    return (vec2<scoord>){
      toSSPX(v.y,v.x),
      toSSPY(v.z,v.x)};
  }
  tri2<scoord> toSSPT(tri3<mesh_size> t){
    return (tri2<scoord>){
      toSSPX(t.a.y,t.a.x),
      toSSPY(t.a.z,t.a.x),
      toSSPX(t.a.y,t.a.x),
      toSSPY(t.b.z,t.b.x),
      toSSPX(t.c.y,t.c.x),
      toSSPY(t.c.z,t.c.x)};
  }
  void drawCTri(scoord x0,scoord y0,scoord x1,scoord y1,scoord x2,scoord y2){
    scoord minx=min(x0,x1,x2),miny=min(y0,y1,y2),maxx=max(x0,x1,x2),maxy=max(y0,y1,x2);
    for(scoord x=minx;x<maxx;x++){
      for(scoord y=miny;y<maxy;y++){
        if(triarea(x,y,x1,y1,x2,y2)>=0){if(triarea(x0,y0,x,y,x2,y2)>=0){if(triarea(x0,y0,x1,y1,x,y)>=0){
          putChar(x,y,'#');
        }}}
      }
    }
    putChar(x0,y0,'+');
    putChar(x1,y1,'+');
    putChar(x2,y2,'+');
  }
  void drawTTri(tri2<scoord> t){
    auto [p0,p1,p2]=t;
    auto [x0,y0]=p0;auto [x1,y1]=p1;auto [x2,y2]=p2;
    return drawCTri(x0,y0,x1,y1,x2,y2);
  }
  void drawMTri(meshtri t){
    return drawTTri(toSSPT(t-camera));
  }
}
#endif