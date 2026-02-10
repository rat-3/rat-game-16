#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.h>
#include <cmath>
#define SCAST(t,v) static_cast<t>(v)
template<typename T> concept arith=std::is_arithmetic_v<T>;
template<typename T> concept comp =requires(T a,T b){a<b;a>b;a==b;};
template<typename T,typename U> T min(T a,U b){return a<b?a:b;}
template<comp T,comp U> T max(T a,U b){return a<b?b:a;}
template<comp T,comp...U> T min(T t, U...a){
  T b=min(a...);
  return t<b?t:b;
}
template<comp T,comp...U> T max(T t, U...a){
  T b=max(a...);
  return t<b?b:t;
}
template<arith T> inline auto triarea(T x0,T y0,T x1,T y1,T x2,T y2){
  if constexpr(std::is_integral_v<T>){
    using sT=std::make_signed_t<T>;
    return((x0 * ((sT)y1-y2)) + (x1 * ((sT)y2-y0)) + (x2 * ((sT)y0-y1)));
  }else{return((x0 * ((T)y1-y2)) + (x1 * ((T)y2-y0)) + (x2 * ((T)y0-y1)));}
}
namespace mesh {
  const char* charsbyopacity="$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`\'.";
  int opacitylength=69;//
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
    template<typename U> auto constexpr operator+(const tri3<U>& t)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()+std::declval<vec3<U>>())>>){a+t.a,b+t.b,c+t.c};}
    template<typename U> auto constexpr operator+(const vec3<U>& v)const{return (tri3<vec3_inner_t<decltype(std::declval<vec3<T>>()+std::declval<vec3<U>>())>>){a+v,b+v,c+v};}
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
  template<arith T> inline void rotate(T& axis_0,T& axis_1,char d){
    float r1=cos(d/128.0*M_PI),r2=sin(d/128.0*M_PI);
    float axis_0_t=(axis_0*r1)-(axis_1*r2);
    axis_1=axis_1*r1+axis_0*r2;
    axis_0=axis_0_t;
  }
  template<typename T> inline tri3<T> rotateT(tri3<T>& v,char d){
    rotate(v.a.x,v.a.y,d);rotate(v.b.x,v.b.y,d);rotate(v.c.x,v.c.y,d);
    return v;
  }
  vec3<mesh_size> camera_position{-5.0f,0.0f,0.0f};
  vec3<char>      camera_rotation{0,0,0};//roll pitch yaw
}
namespace gui {
  using namespace mesh;
  inline vec2<scoord> toSSPV(vec3<mesh_size> v){
    return (vec2<scoord>){toSSPX(v.y,v.x),toSSPY(v.z,v.x)};
  }
  inline tri2<scoord> toSSPT(tri3<mesh_size> t){
    return (tri2<scoord>){toSSPV(t.a),toSSPV(t.b),toSSPV(t.c)};
  }
  void drawCTri(scoord x0,scoord y0,scoord x1,scoord y1,scoord x2,scoord y2){
    scoord minx=max(min(x0,x1,x2),0),miny=max(min(y0,y1,y2),0),maxx=min(max(x0,x1,x2),gui::term_dims.ws_col),maxy=min(max(y0,y1,x2),gui::term_dims.ws_row);
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
  template<arith T>
  void drawDTri(scoord x0,scoord y0,T z0,scoord x1,scoord y1,T z1,scoord x2,scoord y2,T z2,FILE* fuck){
    scoord minx=max(min(x0,x1,x2),0),miny=max(min(y0,y1,y2),0),maxx=min(max(x0,x1,x2),gui::term_dims.ws_col),maxy=min(max(y0,y1,x2),gui::term_dims.ws_row);
    for(scoord x=minx;x<maxx;x++){
      for(scoord y=miny;y<maxy;y++){
        vec3<float> barycentric;
        if((barycentric.x=triarea(
          SCAST(float,x),  SCAST(float,y),
          SCAST(float,x1), SCAST(float,y1),
          SCAST(float,x2), SCAST(float,y2)
        ))>=0){
          if((barycentric.y=triarea(
            SCAST(float,x0), SCAST(float,y0),
            SCAST(float,x),  SCAST(float,y),
            SCAST(float,x2), SCAST(float,y2)
          ))>=0){
            if((barycentric.z=triarea(
              SCAST(float,x0), SCAST(float,y0),
              SCAST(float,x1), SCAST(float,y1),
              SCAST(float,x),  SCAST(float,y)
            ))>=0){
              barycentric=barycentric/triarea(
                SCAST(float,x0),SCAST(float,y0),
                SCAST(float,x1),SCAST(float,y1),
                SCAST(float,x2),SCAST(float,y2));
              float depth=(barycentric.x*z0+barycentric.y*z1+barycentric.z*z2)/(z0+z1+z2);
              float d=(depth/8);
              fprintf(fuck,"%u?%u\n",depth_buffer[x+y*term_dims.ws_col],static_cast<char>(d*255));
              if((depth_buffer[x+y*term_dims.ws_col]) > static_cast<char>(d*255)){
                depth_buffer[x+y*term_dims.ws_col]=static_cast<char>(d*255);
                if(0<depth&&depth<4){putChar(x,y,charsbyopacity[static_cast<int>(d*opacitylength)]);}
              }
            }
          }
        }
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
  void drawMTri(meshtri t,FILE* fuck){
    tri3<mesh_size> t1=t-camera_position;
    return drawDTri(
      toSSPX(t1.a.y,t1.a.x),toSSPY(t1.a.z,t1.a.x),t1.a.x,
      toSSPX(t1.b.y,t1.b.x),toSSPY(t1.b.z,t1.b.x),t1.b.x,
      toSSPX(t1.c.y,t1.c.x),toSSPY(t1.c.z,t1.c.x),t1.c.x,fuck
    );
  }
}
#endif