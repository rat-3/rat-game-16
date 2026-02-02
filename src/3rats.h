#ifndef RATS3D_H
#define RATS3D_H
#include <type_traits>
#include <cstdlib>
#include <r@@2e.h>
namespace mesh {
  typedef float mesh_size;
  template<typename T> concept arith=std::is_arithmetic_v<T>;
  template<typename T> concept comp =requires(T a,T b){a<b;a>b;a==b;};
  template<typename T> requires arith<T>&&comp<T> struct vec2 {T x,y;};//all of these should implement https://cplusplus.com/reference/type_traits/is_nothrow_move_constructible/
  template<typename T> requires arith<T>&&comp<T> struct vec3 {T x,y,z;};
  template<typename T> requires arith<T>&&comp<T> struct tri2 {vec2<T> a,b;};
  template<typename T> requires arith<T>&&comp<T> struct tri3 {vec3<T> a,b,c;};
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
}
#endif