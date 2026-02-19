#ifndef ASSETS_H
#define ASSETS_H :3
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <3rats.hpp>
#include <type_traits>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#define DO(x) if(x)
#define ORDIE(s) {fclose(file);if(tmp){free(tmp);};perror(s);exit(1);}
#define FEXPECT(EXP_STR,EXP_STR_LEN) \
fgets(tmp,128,file);\
DO(ferror(file))ORDIE("ferror");i=0;\
while(tmp[i]==' ' || tmp[i]=='\n'){i++;};\
asdf=memcmp(&tmp[i],EXP_STR,EXP_STR_LEN);\
if(asdf){printf("'%.*s'!='%.*s'\n\r\n\r",EXP_STR_LEN,&tmp[i],EXP_STR_LEN,EXP_STR);}\
DO(asdf)
#define WSPACE(VAR) while(tmp[VAR]==' ' || tmp[VAR]=='\n'){VAR++;}
#define NSPACE(VAR) while(tmp[VAR]!=' ' && tmp[VAR]!='\n'){VAR++;}
static int asdf;
namespace assets {
  using namespace mesh;
  static std::vector<meshtri> readModel(const char* filename) {
    unsigned short int i;
    FILE* file=fopen(filename,"r");
    std::vector<meshtri> tris;
    char* tmp=(char*)malloc(512);
    DO(!tmp)ORDIE("couldn't alloc name");
    DO(ferror(file))ORDIE("ferror 1");
    FEXPECT("solid ",5);
    unsigned short int l=strlen(&tmp[i]-5);
    char* name=(char*)malloc(l+1);
    memcpy(name,&tmp[i+5],l);name[l]='\0';
    while(!feof(file)){
      fgets(tmp,128,file);
      DO(ferror(file))ORDIE("ferror 2");
      i=0;
      WSPACE(i);
      if(!memcmp(&tmp[i],"endsolid",8)){
        FEXPECT(name,l)ORDIE("couldn't read stl file: bad ending");
        break;
      }
      FEXPECT("outer loop",10)ORDIE("couldn't read stl file: expected outer loop")
      FEXPECT("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACE(i);
      float x0=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float y0=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float z0=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      FEXPECT("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACE(i);
      float x1=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float y1=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float z1=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      FEXPECT("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACE(i);
      float x2=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float y2=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      float z2=atof(&tmp[i]);
      NSPACE(i);WSPACE(i);
      FEXPECT("endloop",7)ORDIE("expected endloop");
      FEXPECT("endfacet",8)ORDIE("expected endfacet");
      tris.push_back((meshtri){x0,y0,z0,x1,y1,z1,x2,y2,z2});
    }
    free(tmp);
    fclose(file);
    return tris;//caller is responsible for reallocating tris and deleting vector
  }
  template<typename ...T> requires (std::is_convertible_v<T,const char*>&&...)
  model_t* readModels(const T... names) {
    size_t count=sizeof...(T);
    model_t* out=(model_t*)malloc(count*sizeof(model_t));
    int i=0;
    for(const char* n: {names...}){
      std::vector<meshtri> a=readModel(n);
      out[i].tricount=a.size();
      size_t s=out[i].tricount*sizeof(meshtri);
      out[i].tris=(meshtri*)malloc(s);
      out[i].name=n;
      memcpy(out[i].tris,&a[0],s);
      i++;//let a go out of scope and die
    }
    return out;
  }
  void writeGrayScaleToPPM(const char* name,const unsigned char* buf,size_t width,size_t height){
    FILE* file=fopen(name,"w");
    fprintf(file,"P3 %u %u 255\n",width,height*3);
    for(int i=0;i<height;i++){
      for(int j=0;j<width;j++){
        fprintf(file,"%3u %3u %3u ",buf[i*width+j],buf[i*width+j],buf[i*width+j]);
      }
      fputs("\n",file);
      for(int j=0;j<width;j++){
        fprintf(file,"%3u %3u %3u ",buf[i*width+j],buf[i*width+j],buf[i*width+j]);
      }
      fputs("\n",file);
      for(int j=0;j<width;j++){
        fprintf(file,"%3u %3u %3u ",buf[i*width+j],buf[i*width+j],buf[i*width+j]);
      }
      fputs("\n",file);
    }
    fclose(file);
  }
}
#undef FEXPECT
#undef NOFERR
#undef WSPACE
#undef NSPACE
#undef DO
#undef ORDIE
#pragma GCC diagnostic warning "-Wint-to-pointer-cast"
#endif