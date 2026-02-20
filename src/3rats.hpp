#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.hpp>
#include <cmath>
#define SCAST(t,v) static_cast<t>(v)
#define FARPLANEX 8
#define MESHTRI_OUTLN_01 0b00000001
#define MESHTRI_OUTLN_12 0b00000010
#define MESHTRI_OUTLN_20 0b00000100
template<typename T> concept arith=std::is_arithmetic_v<T>;
template<typename T> concept comp =requires(T a,T b){a<b;a>b;a==b;};
template<comp T,comp U> T constexpr min(T a,U b){return a<b?a:b;}
template<comp T,comp U> T constexpr max(T a,U b){return a<b?b:a;}
template<comp T,comp...U> T constexpr min(T t, U...a){
  T b=min(a...);
  return t<b?t:b;
}
template<comp T,comp...U> T constexpr max(T t, U...a){
  T b=max(a...);
  return t<b?b:t;
}
template<arith T> inline auto constexpr triarea(T x0,T y0,T x1,T y1,T x2,T y2){
  if constexpr(std::is_integral_v<T>){
    using sT=std::make_signed_t<T>;
    return -((x0 * ((sT)y1-y2)) + (x1 * ((sT)y2-y0)) + (x2 * ((sT)y0-y1)));
  }else{return -((x0 * (y1-y2)) + (x1 * (y2-y0)) + (x2 * (y0-y1)));}
}
namespace mesh {
  const char* charsbyopacity="$@MN%&0EK?UO^!;:,.";
  int opacitylength=18;
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
  template<typename T> struct vec_inner;//partial template specialization
  template<typename T> struct vec_inner<vec2<T>>{using type=T;};
  template<typename T> struct vec_inner<vec3<T>>{using type=T;};
  template<typename T> using  vec_inner_t=typename vec_inner<T>::type;
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
  };
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
    unsigned char flags=255;
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
  void drawLine(scoord x0,scoord y0,mesh_size z0,scoord x1,scoord y1,mesh_size z1){
    signed short int dx=(signed short int)x1-x0;
    signed short int dy=(signed short int)y1-y0;
    scoord &x0_=(x0<x1?x0:x1),&x1_=(x0_==x0?x1:x0),&y0_=(x0_==x0?y0:y1),&y1_=(x0_==x0?y1:y0);
    mesh_size &z0_=(x0_==x0?z0:z1),&z1_=(x0_==x0?z1:z0);
    if((abs(dx)>abs(dy))&&(dx!=0)){
      float m=(float)dy/dx;
      float mz=(z1-z0)/(dx);
      scoord ly=(x0_>0)?m*(-1)+y0_:y0_;
      scoord ny=y0_;
      char c=(m>0)?'\\':'/';
      for(scoord x=x0_;x<x1_;){
        scoord y=ny;
        float d=mz*(x-x0_)+z0_;
        if(depth_buffer[x+y*term_dims.ws_col]>(d/FARPLANEX*255)){
          depth_buffer[x+y*term_dims.ws_col]=(d/FARPLANEX*255);
          putChar(x,y,(y!=ly||y!=ny)?c:'-');
        }
        ly=y;
        ny=m*(++x-x0_)+y0_;
      }
    }else{
      float m=(float)dx/dy;
      float mz=(z1-z0)/(dy);
      scoord lx=(y0_>0)?m*(-1)+x0_:x0_;
      scoord nx=x0_;
      char c=(m>0)?'\\':'/';
      for(scoord y=y0_;y<y1_;){
        scoord x=nx;
        float d=mz*(y-y0_)+z0_;
        if(depth_buffer[x+y*term_dims.ws_col]>(d/FARPLANEX*255)){
          depth_buffer[x+y*term_dims.ws_col]=(d/FARPLANEX*255);
          putChar(x,y,(x!=lx||x!=nx)?c:'|');
        }
        lx=x;
        nx=m*(++y-y0_)+x0_;
      }
    }
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
  void drawMTri(const meshtri& t){
    tri3<mesh_size> t1=t-camera_position;
    rotateT(t1,camera_rotation.z);
    mesh_size z0=t1.a.x,z1=t1.b.x,z2=t1.c.x;
    scoord x0=toSSPX(t1.a.y,z0),y0=toSSPY(t1.a.z,z0),
           x1=toSSPX(t1.b.y,z1),y1=toSSPY(t1.b.z,z1),
           x2=toSSPX(t1.c.y,z2),y2=toSSPY(t1.c.z,z2);
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
              float depth=(barycentric.x*z0+barycentric.y*z1+barycentric.z*z2);
              float d=(depth/FARPLANEX);
              if((depth_buffer[x+y*term_dims.ws_col]) > (unsigned char)(d*255)){
                depth_buffer[x+y*term_dims.ws_col]=(unsigned char)(d*255);
                char c=charsbyopacity[(int)(d*opacitylength)];
                if(0<depth&&depth<FARPLANEX){
                  putChar(x,y,c);
                  putColor(x,y,colors::col(colors::red,colors::black));
                }
              }
            }
          }
        }
      }
    }
  }
  void drawMLines(const meshtri& t){
    tri3<mesh_size> t1=t-camera_position;
    rotateT(t1,camera_rotation.z);
    mesh_size z0=t1.a.x,z1=t1.b.x,z2=t1.c.x;
    scoord x0=toSSPX(t1.a.y,z0),y0=toSSPY(t1.a.z,z0),
           x1=toSSPX(t1.b.y,z1),y1=toSSPY(t1.b.z,z1),
           x2=toSSPX(t1.c.y,z2),y2=toSSPY(t1.c.z,z2);
    drawLine(x0,y0,z0,x1,y1,z1);
    drawLine(x1,y1,z1,x2,y2,z2);
    drawLine(x2,y2,z2,x0,y0,z0);
  }
}
#endif