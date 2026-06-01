#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.hpp>
#include <cmath>
#define SCAST(t,v) static_cast<t>(v)
#define MESHTRI_OUTLN_01 0b00000001
#define MESHTRI_OUTLN_12 0b00000010
#define MESHTRI_OUTLN_20 0b00000100

template<arith T> inline auto constexpr triarea(T x0,T y0,T x1,T y1,T x2,T y2){
  if constexpr(std::is_integral_v<T>){
    using sT=std::make_signed_t<T>;
    return ((x0 * ((sT)y1-y2)) + (x1 * ((sT)y2-y0)) + (x2 * ((sT)y0-y1)));//these probably shouldn't be negative
  }else{return ((x0 * (y1-y2)) + (x1 * (y2-y0)) + (x2 * (y0-y1)));}//but it still works so
}
namespace mesh {
  unsigned int farplanex=12;
  const char* charsbyopacity="$@MN%&E0K?UO^!;:,.";
  int opacitylength=18;
  template<arith T> constexpr void rotate(T& axis_0,T& axis_1,signed char d){
    float r1=cos(d/128.0*M_PI),r2=sin(d/128.0*M_PI);
    float axis_0_t=(axis_0*r1)-(axis_1*r2);
    axis_1=axis_1*r1+axis_0*r2;
    axis_0=axis_0_t;
  }
  template<typename T> constexpr inline tri3<T> rotateT(tri3<T>& v,signed char d){
    rotate(v.a.x,v.a.y,d);rotate(v.b.x,v.b.y,d);rotate(v.c.x,v.c.y,d);
    return v;
  }
  int clipModelPlane(model_t m, plane_t p, vec3<float> c, vec3<char> r){
    vec3<float> cen = m.getCenter()-c;rotate(cen.x,cen.y,r.z);
  
    float sd = p.signedDistance(cen);
    float mr = m.getRadius();
    if (sd > mr){
      return 0;
    }else if (sd < -mr){
      return 1;
    }else{
      return 2;
    }
  }
  int clipTriPlane(tri3<float> t, plane_t p, vec3<float> c, vec3<char> r){
    vec3<float> cen = t.getCenter()-c;rotate(cen.x,cen.y,r.z);
    float sd = p.signedDistance(cen);
    float mr = t.getRadius();
    if (sd > mr){
      return 0;
    }else if (sd < -mr){
      return 1;
    }else{
      return 2;
    }
  } 
  template<typename T> int clipTri(tri3<T> t, vec3<float> c, vec3<char> r){
    int ret=0;
    for (int i = 0; i < 5; i++){
      int tc = clipTriPlane(t,planes[i],c,r);
      switch(tc){
        case (0):{
          continue;
        };break;
        case(1):{
          return 1;
        };break;
        case(2):{
          ret=2;
        };break;
      }
    }
    return ret;
  }
  int clipModel(model_t m, vec3<float> c, vec3<char> r){
    int ret = 0;
    for (int i = 0; i < 5; i++){
      int cmp = clipModelPlane(m, planes[i],c,r);
      switch (cmp){
        case(0):{
          continue;
        };break;
        case(1):{
          return 1;
        };break;
        case(2):{
          ret = 2;
        };break;
      } 
    }
    return ret;
  }
  template<typename T> requires std::is_signed_v<T> vec3<T>* clipTriX(const tri3<T>& t,T x){//012,230
    vec3<T>* out=(vec3<T>*)malloc(sizeof(vec3<T>)*4);
    #define p0 t.a
    #define p1 t.b
    #define p2 t.c
    #define o0 out[0]
    #define o1 out[1]
    #define o2 out[2]
    #define o3 out[3]
    o3={0,0,0};
    char v=(p0.x>=x)+(p1.x>=x)+(p2.x>=x);
    switch(v){
      case 0:break;
      case 1:{
        if(p0.x>=x){
          o0=p0;
          o1={
            x,
            (p0.y-p1.y)/(p0.x-p1.x)*(x-p1.x)+p1.y,
            (p0.z-p1.z)/(p0.x-p1.x)*(x-p1.x)+p1.z
          };
          o2={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }else if(p1.x>=x){
          o0={
            x,
            (p1.y-p0.y)/(p1.x-p0.x)*(x-p0.x)+p0.y,
            (p1.z-p0.z)/(p1.x-p0.x)*(x-p0.x)+p0.z
          };
          o1=p1;
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
        }else{
          o0={
            x,
            (p2.y-p0.y)/(p2.x-p0.x)*(x-p0.x)+p0.y,
            (p2.z-p0.z)/(p2.x-p0.x)*(x-p0.x)+p0.z
          };
          o1={
            x,
            (p2.y-p1.y)/(p2.x-p1.x)*(x-p1.x)+p1.y,
            (p2.z-p1.z)/(p2.x-p1.x)*(x-p1.x)+p1.z
          };
          o2=p2;
        }
        break;
      }
      case 2:{
        if(p0.x<x){
          o0={
            x,
            (p1.y-p0.y)/(p1.x-p0.x)*(x-p0.x)+p0.y,
            (p1.z-p0.z)/(p1.x-p0.x)*(x-p0.x)+p0.z
          };
          o1=p1;
          o2=p2;
          o3={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }else if(p1.x<x){
          o0=p0;
          o1={
            x,
            (p0.y-p1.y)/(p0.x-p1.x)*(x-p1.x)+p1.y,
            (p0.z-p1.z)/(p0.x-p1.x)*(x-p1.x)+p1.z
          };
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
          o3=p2;
        }else{
          o0=p0;
          o1=p1;
          o2={
            x,
            (p1.y-p2.y)/(p1.x-p2.x)*(x-p2.x)+p2.y,
            (p1.z-p2.z)/(p1.x-p2.x)*(x-p2.x)+p2.z
          };
          o3={
            x,
            (p0.y-p2.y)/(p0.x-p2.x)*(x-p2.x)+p2.y,
            (p0.z-p2.z)/(p0.x-p2.x)*(x-p2.x)+p2.z
          };
        }
        break;
      }
      case 3:{
        o0=t.a;o1=t.b;o2=t.c;
      }
    }
    return out;
    #undef p0
    #undef p1
    #undef p2
    #undef o0
    #undef o1
    #undef o2
    #undef o3
  }
  vec3<mesh_size> camera_position{-2.0f,0.0f,0.0f};
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
  void drawTri(const tri3<mesh_size>& t1, vec2<float> uv0, vec2<float> uv1, vec2<float> uv2, assets::texture_t& tex){
    mesh_size z0=t1.a.x,z1=t1.b.x,z2=t1.c.x;
    printd("triangle((%.3f,%.3f,%.3f),(%.3f,%.3f,%.3f),(%.3f,%.3f,%.3f));\n\rpolygon((%.3f,%.3f),(%.3f,%.3f),(%.3f,%.3f))\r\n",t1.a.x,t1.a.y,t1.a.z,t1.b.x,t1.b.y,t1.b.z,t1.c.x,t1.c.y,t1.c.z,uv0.x,uv0.y,uv1.x,uv1.y,uv2.x,uv2.y);
    signed short int
      x0=toSSPX(t1.a.y,z0),y0=toSSPY(t1.a.z,z0),
      x1=toSSPX(t1.b.y,z1),y1=toSSPY(t1.b.z,z1),
      x2=toSSPX(t1.c.y,z2),y2=toSSPY(t1.c.z,z2);
    scoord minx=max(min(x0,x1,x2),0),
           miny=max(min(y0,y1,y2),0),
           maxx=min(max(x0,x1,x2,minx),gui::term_dims.ws_col),
           maxy=min(max(y0,y1,y2,miny),gui::term_dims.ws_row);
    float area=triarea(
      SCAST(float,x0),SCAST(float,y0),
      SCAST(float,x1),SCAST(float,y1),
      SCAST(float,x2),SCAST(float,y2));
    if(area<0.1){return;}
    for(scoord x=minx;x<maxx;x++){
      for(scoord y=miny;y<maxy;y++){
        vec3<float> barycentric;
        if(((barycentric.x=triarea(
          SCAST(float,x), SCAST(float,y),
          SCAST(float,x1),SCAST(float,y1),
          SCAST(float,x2),SCAST(float,y2)
        ))>=0)&&((barycentric.y=triarea(
          SCAST(float,x0),SCAST(float,y0),
          SCAST(float,x), SCAST(float,y),
          SCAST(float,x2),SCAST(float,y2)
        ))>=0)&&((barycentric.z=triarea(
          SCAST(float,x0),SCAST(float,y0),
          SCAST(float,x1),SCAST(float,y1),
          SCAST(float,x), SCAST(float,y)
        ))>=0)){
          barycentric=barycentric/area;
          float depth=(barycentric.x*z0+barycentric.y*z1+barycentric.z*z2);
          float d=(depth/farplanex);
          if((depth_buffer[toSSPI(x,gui::term_dims.ws_row-y-1)])>(d*255)){
            depth_buffer[toSSPI(x,gui::term_dims.ws_row-y-1)]=(unsigned char)(d*255);
            if((0<depth)&&(depth<farplanex)){
              float u=uv0.x*barycentric.x+uv1.x*barycentric.y+uv2.x*barycentric.z;
              float v=uv0.y*barycentric.x+uv1.y*barycentric.y+uv2.y*barycentric.z;
              u*=tex.width;
              v*=tex.height;
              int iu=(((int)u%tex.width+tex.width)%tex.width);//hate
              int iv=tex.height-(((int)v%tex.height+tex.height)%tex.height);
              int idx=(iv*tex.width+iu)*3;
              unsigned char r=tex.pixels[idx],g=tex.pixels[idx+1],b=tex.pixels[idx+2];
              char colorIdx = (r>128)|((g>128)<<1)|((b>128)<<2)|(((r+g+b)>(255.0f*3/2))<<3);//don't need to store brightness just calculate it as bool earlier
              char c = charsbyopacity[(int)(d*opacitylength)];
              putChar(x,gui::term_dims.ws_row-y-1,c);
              putColor(x,gui::term_dims.ws_row-y-1,colors::col((colors::color)colorIdx,colors::black));
            }
            // if(logmisc){
              // fprintf(debug,"(%u,%u,%f),",x,y,depth);
              // fprintf(debug,"(%u,%u,%u),",x,y,depth_buffer[toSSPI(x,y)]);
            // }
          }
        }
      }
    }
    // fflush(debug);
  }
  void drawMTri(const meshtri& t, assets::texture_t& tex){
    meshtri t1=t-camera_position;
    rotateT(t1,camera_rotation.z);
    char v=(t1.a.x<nearplanex)+(t1.b.x<nearplanex)+(t1.c.x<nearplanex);
    if(v==3){return;}
    if(v!=0){
      vec3<mesh_size>* clipped=clipTriX(t1,nearplanex);//optimize to reuse
      meshtri t2{
        clipped[0],
        clipped[1],
        clipped[2]};
      vec3<mesh_size> a1=(t1.c-t1.a).cross(t1.b-t1.a);//really this is double the area but we don't care
      mesh_size a2=(a1*a1).total();
      vec3<mesh_size> a=(t1.b-t2.a).cross(t1.c-t2.a);//because it's ratios of areas
      vec3<mesh_size> b=(t1.c-t2.a).cross(t1.a-t2.a);
      vec3<mesh_size> c=(t1.a-t2.a).cross(t1.b-t2.a);
      t2.uv0=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
      a=(t1.b-t2.b).cross(t1.c-t2.b);
      b=(t1.c-t2.b).cross(t1.a-t2.b);
      c=(t1.a-t2.b).cross(t1.b-t2.b);
      t2.uv1=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
      a=(t1.b-t2.c).cross(t1.c-t2.c);
      b=(t1.c-t2.c).cross(t1.a-t2.c);
      c=(t1.a-t2.c).cross(t1.b-t2.c);
      t2.uv2=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
      if(logmisc){fprintf(debug,"polygon((%f,%f),(%f,%f),(%f,%f)),",t2.uv0.x,t2.uv0.y,t2.uv1.x,t2.uv1.y,t2.uv2.x,t2.uv2.y);fflush(debug);}
      drawTri(t2,t2.uv0,t2.uv1,t2.uv2,tex);
      if(clipped[3].x!=0.0f){
        meshtri t3{
          clipped[2],
          clipped[3],
          clipped[0]};
      a=(t1.b-t3.a).cross(t1.c-t3.a);
      b=(t1.c-t3.a).cross(t1.a-t3.a);
      c=(t1.a-t3.a).cross(t1.b-t3.a);
      t3.uv0=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
      a=(t1.b-t3.b).cross(t1.c-t3.b);
      b=(t1.c-t3.b).cross(t1.a-t3.b);
      c=(t1.a-t3.b).cross(t1.b-t3.b);
      t3.uv1=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
      a=(t1.b-t3.c).cross(t1.c-t3.c);
      b=(t1.c-t3.c).cross(t1.a-t3.c);
      c=(t1.a-t3.c).cross(t1.b-t3.c);
      t3.uv2=
        t1.uv0*(float)sqrt((a*a).total()/a2)+
        t1.uv1*(float)sqrt((b*b).total()/a2)+
        t1.uv2*(float)sqrt((c*c).total()/a2);
        drawTri(t3, t3.uv0, t3.uv1, t3.uv2, tex);
      }
      free(clipped);
    }else{drawTri(t1, t1.uv0, t1.uv1, t1.uv2, tex);/*merge uvs into tri2<float>*/}
  }
  int drawModel(assets::asset3d_t m){
    vec3<float> mcc = m.mesh.getCenter()-camera_position; rotate(mcc.x,mcc.y,camera_rotation.z);
    int mc = clipModel(m.mesh, camera_position, camera_rotation);
    switch(mc){
      case(0):{
        for(short unsigned int i=0;i<m.mesh.tricount;i++){
          drawMTri(m.mesh.tris[i],m.texture);
        }
      };break;
      case(1):{
        // for(short unsigned int i=0;i<m.mesh.tricount;i++){
        //   drawMTri(m.mesh.tris[i],m.texture);
        // }
      };break;
      case(2):{
        for(short unsigned int i=0;i<m.mesh.tricount;i++){
          int tc = clipTri(m.mesh.tris[i],camera_position,camera_rotation);
          switch(tc){
            case(0):{
              drawMTri(m.mesh.tris[i],m.texture);
            };break;
            case(1):{
              // die
            };break;
            case(2):{
              drawMTri(m.mesh.tris[i],m.texture);
            };break;
          }
        }
      };break;
    }
    return mc;
  }
}
#endif