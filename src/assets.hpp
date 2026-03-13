#ifndef ASSETS_H
#define ASSETS_H
#include <cstdio>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#define DO(x) if(x)
#define ABORT if(file){fclose(file);file=NULL;}if(tmp){free(tmp);tmp=NULL;};exit(1);
#define ORDIE(s) {perror(s);ABORT}
#define FEXPECTL(EXP_STR,EXP_STR_LEN)\
fgets(tmp,128,file);\
DO(ferror(file))ORDIE("ferror");i=0;\
while(tmp[i]==' ' || tmp[i]=='\n'){i++;};\
debug_bad_stl=memcmp(&tmp[i],EXP_STR,EXP_STR_LEN);\
if(debug_bad_stl){printf("'%.*s'!='%.*s'\n\r\n\r",EXP_STR_LEN,&tmp[i],EXP_STR_LEN,EXP_STR);}\
DO(debug_bad_stl)
#define FEXPECTS(EXP_STR,EXP_STR_LEN)\
fread(tmp,1,EXP_STR_LEN,file);\
DO(memcmp(tmp,EXP_STR,EXP_STR_LEN))
#define FGETI(VAR)\
VAR=0;\
do{tmp[VAR]=fgetc(file);VAR++;}while(('0'<=tmp[VAR-1])&&(tmp[VAR-1]<='9'));\
VAR--;ungetc(tmp[VAR],file);\
tmp[VAR]='\0';VAR=atoi(tmp);
#define WSPACEL(VAR) while((tmp[VAR]==' ')||(tmp[VAR]=='\n')||(tmp[VAR]=='#')){if(tmp[VAR]=='#'){while(tmp[VAR]!='\n'){VAR++;}}VAR++;}
#define NSPACEL(VAR) while((tmp[VAR]!=' ')&&(tmp[VAR]!='\n')){VAR++;}
static int debug_bad_stl;
static char* tmp;
static FILE* file;
namespace assets {
  static void int_fexpectl(const char* EXP_STR,int EXP_STR_LEN){
    int i;
    fgets(tmp,128,file);
    DO(ferror(file))ORDIE("ferror");i=0;
    while(tmp[i]==' ' || tmp[i]=='\n'){i++;};
    debug_bad_stl=memcmp(&tmp[i],EXP_STR,EXP_STR_LEN);
    if(debug_bad_stl){printf("'%.*s'!='%.*s'\n\r\n\r",EXP_STR_LEN,&tmp[i],EXP_STR_LEN,EXP_STR);}
  }
  static int WSPACE(){
    int o=0;
    do{
      tmp[o]=fgetc(file);
      if(tmp[o]=='#'){
        while(tmp[o]!='\n'){o++;tmp[o]=fgetc(file);}
      }
      o++;
    }while((tmp[o-1]==' ')||(tmp[o-1]=='\n'));
    o--;ungetc(tmp[o],file);
    return o;
  }
  static std::vector<mesh::meshtri> readModel(const char* filename) {
    unsigned short int i;
    file=fopen(filename,"r");
    std::vector<mesh::meshtri> tris;
    tmp=(char*)malloc(512);
    DO(!tmp)ORDIE("couldn't alloc name");
    DO(ferror(file))ORDIE("ferror 1");
    FEXPECTL("solid ",5);
    unsigned short int l=strlen(&tmp[i]-5);
    char* name=(char*)malloc(l+1);
    memcpy(name,&tmp[i+5],l);name[l]='\0';
    while(!feof(file)){
      fgets(tmp,128,file);
      DO(ferror(file))ORDIE("ferror 2");
      i=0;
      WSPACEL(i);
      if(!memcmp(&tmp[i],"endsolid",8)){
        FEXPECTL(name,l)ORDIE("couldn't read stl file: bad ending");
        break;
      }
      FEXPECTL("outer loop",10)ORDIE("couldn't read stl file: expected outer loop")
      FEXPECTL("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACEL(i);
      float x0=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float y0=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float z0=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      FEXPECTL("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACEL(i);
      float x1=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float y1=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float z1=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      FEXPECTL("vertex",6)ORDIE("couldn't read stl file: expected vertex");
      i+=6;
      WSPACEL(i);
      float x2=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float y2=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      float z2=atof(&tmp[i]);
      NSPACEL(i);WSPACEL(i);
      FEXPECTL("endloop",7)ORDIE("expected endloop");
      FEXPECTL("endfacet",8)ORDIE("expected endfacet");
      tris.push_back((mesh::meshtri){x0,y0,z0,x1,y1,z1,x2,y2,z2});
    }
    free(tmp);
    fclose(file);
    return tris;
  }
  template<typename ...T> requires (std::is_convertible_v<T,const char*> && ...)
  mesh::model_t* readModels(const T... names) {
    size_t count=sizeof...(T);
    mesh::model_t* out=(mesh::model_t*)malloc(count*sizeof(mesh::model_t));
    int i=0;
    for(const char* n: {names...}){
      std::vector<mesh::meshtri> a=readModel(n);
      out[i].tricount=a.size();
      size_t s=out[i].tricount*sizeof(mesh::meshtri);
      out[i].tris=(mesh::meshtri*)malloc(s);
      out[i].name=n;
      memcpy(out[i].tris,&a[0],s);
      i++;
    }
    return out;
  }
  static texture_t readPPM(const char* filename){
    texture_t out;
    file=fopen(filename, "r");
    tmp = (char*)malloc(128);
    DO(!file){memcpy(tmp,"couldn't open file for read: ",30);strcat(tmp,filename);perror(tmp);ABORT};
    int i=0;
    int width, height, maxVal;
    char format=0;
    printf("reading file %s:",filename);
    FEXPECTS("P3",2){//segfaults i guess
      DO(memcmp(tmp,"P6",2))ORDIE("unsupported file format: not PPM3 or PPM6")else{
        format=2;
        puts("ppm6");
      }
    }else{
      format=1;
      puts("ppm3");
    }
    WSPACE();FGETI(width);
    WSPACE();FGETI(height);
    WSPACE();FGETI(maxVal);
    printf("%ix%i:%i\n",width,height,maxVal);
    out.width=width;out.height=height;out.pixels=(unsigned char*)malloc(3*width*height);
    if(format==1){//ppm3
      unsigned int r=0,g=0,b=0,j=0;
      while(!feof(file)&&(j<width*height)){
        WSPACE();FGETI(r);
        WSPACE();FGETI(g);
        WSPACE();FGETI(b);
        out.pixels[j*3]  =r*255/maxVal;
        out.pixels[j*3+1]=g*255/maxVal;
        out.pixels[j*3+2]=b*255/maxVal;
        j++;
      }
    }else if(format==2){//ppm6
      WSPACE();
      short int r,g,b,j;
      while(!feof(file)&&(j<width*height)){
        fread(&r,1+(maxVal>255),1,file);
        fread(&g,1+(maxVal>255),1,file);
        fread(&b,1+(maxVal>255),1,file);
        out.pixels[j*3]  =r*255/maxVal;
        out.pixels[j*3+1]=g*255/maxVal;
        out.pixels[j*3+2]=b*255/maxVal;
        j++;
      }
    }
    free(tmp);
    fclose(file);
    return out;
  }
  void writeGrayScaleToPPM(const char* name,const unsigned char* buf,size_t width,size_t height){
    file=fopen(name,"w");
    fprintf(file,"P3 %lu %lu 255\n",width,height*3);
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
#undef FEXPECTL
#undef NOFERR
#undef WSPACEL
#undef NSPACEL
#undef DO
#undef ORDIE
#pragma GCC diagnostic warning "-Wint-to-pointer-cast"
#endif