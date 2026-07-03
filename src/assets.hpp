#ifndef ASSETS_H//i'm a little bit sorry about DO()ORDIE() but
#define ASSETS_H//like be so real it's pretty cool. i don't
#include <cstdio>//regret ANYTHING
#include <vector>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <type_traits>
#include <r@@2e.hpp>
#include <3rats.hpp>
// #pragma GCC diagnostic ignored "-Wint-to-pointer-cast" wait what
#define DO(x) if(x)
#define ABORT if(file){fclose(file);file=NULL;}if(tmp){free(tmp);tmp=NULL;};exit(8);
#define ORDIE(s) {if(errno){perror(s);}else{puts(s);}ABORT}
#define ORTHENDIE(c,s) {c;if(errno){perror(s);}else{puts(s);}ABORT}
#define FEXPECTL(EXP_STR,EXP_STR_LEN)\
fgets(tmp,128,file);\
DO(ferror(file))ORDIE("ferror");i=0;\
while(tmp[i]==' ' || tmp[i]=='\n'){i++;};\
debug_bad_stl=memcmp(&tmp[i],EXP_STR,EXP_STR_LEN);\
DO(debug_bad_stl)
#define FEXPECTS(EXP_STR,EXP_STR_LEN)\
DO(fread(tmp,1,EXP_STR_LEN,file)<1){perror("expected to be able to read more");ABORT}\
DO(memcmp(tmp,EXP_STR,EXP_STR_LEN))
#define FGETI(VAR)\
VAR=0;\
do{tmp[VAR]=fgetc(file);VAR++;}while(('0'<=tmp[VAR-1])&&(tmp[VAR-1]<='9')&&(VAR<127));\
VAR--;ungetc(tmp[VAR],file);\
tmp[VAR]='\0';VAR=atoi(tmp);
#define WSPACEL(VAR) while((tmp[VAR]==' ')||(tmp[VAR]=='\n')||(tmp[VAR]=='#')){if(tmp[VAR]=='#'){while(tmp[VAR]!='\n'){VAR++;}}VAR++;}
#define NSPACEL(VAR) while((tmp[VAR]!=' ')&&(tmp[VAR]!='\n')){VAR++;}
#define NSPACE_TOKENS ' ','\n','#','=',',',')','(','\'','\"'
#define nspace(F,B) readUntil(F,B,NSPACE_TOKENS)
namespace assets {
  sprite_t default_texture{
    3,3,
    (unsigned char[]){
      000,000,000, 000,000,255, 000,255,000,
      000,255,255, 255,000,000, 255,000,255,
      255,255,255, 255,255,255, 255,255,255
    },strdup(
    "123"
    "456"
    "789")
  };
  static int debug_bad_stl;
  static unsigned int wspace(FILE* file,char* tmp){//read until not whitespace
    unsigned int o=0;
    char j;//=fgetc(file);
    do{
      j=fgetc(file);
      if(j=='#'){
        while(j!='\n'&&(!feof(file))){o++;j=fgetc(file);}
      }
      o++;
    }while((j==' ')||(j=='\n')||(j=='#'));
    o--;ungetc(j,file);//fseek(file,-1,SEEK_CUR);
    return o;
  }
  template<typename... T> requires (std::is_convertible_v<T,char>&&...)
  static int readUntil(unsigned int* l,FILE* file,char* tmp,T... c){
    int o=0;
    do{
      tmp[o]=fgetc(file);
      if(l&&(tmp[o]=='\n')){(*l)++;}
      o++;
    }while(o<128&&((tmp[o-1]!=c)&&...));
    o--;ungetc(tmp[o],file);
    return o;
  }
  template<typename... T> requires (std::is_convertible_v<T,char>&&...)
  static int readUntil(FILE* file,char* tmp,T... c){return readUntil(NULL,file,tmp,c...);}
  static mesh::vec2<float> readV2f(FILE* file,char* tmp){//should use this for the transformations tbh i just forgot
    mesh::vec2<float> out;
    DO(fgetc(file)!='(')ORDIE("expected '(' to start vector");wspace(file,tmp);
    tmp[nspace(file,tmp)]='\0';
    out.x=atof(tmp);
    DO(fgetc(file)!=',')ORDIE("expected ',' to separate vector coordinates");wspace(file,tmp);
    tmp[nspace(file,tmp)]='\0';
    out.y=atof(tmp);
    wspace(file,tmp);DO(fgetc(file)!=')')ORDIE("expected ')' to end vector");
    return out;
  }
  static mesh::vec3<float> readV3f(FILE* file,char* tmp){
    mesh::vec3<float> out;
    DO(fgetc(file)!='(')ORDIE("expected '(' to start vector");wspace(file,tmp);
    tmp[nspace(file,tmp)]='\0';
    out.x=atof(tmp);
    DO(fgetc(file)!=',')ORDIE("expected ',' to separate vector coordinates");wspace(file,tmp);
    tmp[nspace(file,tmp)]='\0';
    out.y=atof(tmp);
    DO(fgetc(file)!=',')ORDIE("expected ',' to separate vector coordinates");wspace(file,tmp);
    tmp[nspace(file,tmp)]='\0';
    out.z=atof(tmp);
    wspace(file,tmp);DO(fgetc(file)!=')')ORDIE("expected ')' to end vector");
    return out;
  }
  static int readColor(FILE* file,char* tmp){//first 3 bytes are r,g,b because this is normal
    char c[4];//i hope int is 4 bytes :3
    fread(c,1,3,file);
    if(!memcmp(c,"rgb",3)){
      auto [r,g,b]=readV3f(file,tmp);
      c[0]=r;c[1]=g;c[2]=b;c[3]=0;
      return *(int*)c;
    }else if(!memcmp(c,"hsv",3)) ORDIE("unsupported color format")else ORDIE("unknown color format")
  }
  static std::vector<mesh::vec2<float>> readV2fVec(FILE* file,char* tmp){
    std::vector<mesh::vec2<float>> out={};
    wspace(file,tmp);
    DO(fgetc(file)!='[')ORDIE("expected '[' to start array");
    while(!feof(file)){
      wspace(file,tmp);
      out.push_back(readV2f(file,tmp));
      char c=fgetc(file);
      if(c!=','){
        ungetc(c,file);break;
      }
    }
    wspace(file,tmp);
    DO(fgetc(file)!=']')ORDIE("expected ']' to end array");
    return out;
  }
  static std::vector<mesh::meshtri> readSTL(const char* filename) {
    unsigned int i;
    FILE* file=fopen(filename,"r");
    char* tmp=(char*)malloc(128);
    DO(!tmp)ORDIE("couldn't alloc temporary space")
    DO(!file)ORDIE("couldn't open file")
    DO(ferror(file))ORDIE("ferror 1");
    std::vector<mesh::meshtri> tris;
    FEXPECTL("solid ",6){
      fseek(file,0,SEEK_SET);
      unsigned int trinum;
      float data[12];//first 3 are normal
      fread(tmp,80,1,file);
      fread(&trinum,4,1,file);
      for(i=0;(i<trinum)&&!feof(file);i++){
        fread(data,12,4,file);
        fseek(file,2,SEEK_CUR);//extra flags we don't really care for
        mesh::meshtri tri{
          data[ 3],data[ 4],data[ 5],
          data[ 6],data[ 7],data[ 8],
          data[ 9],data[10],data[11]};
        if((tri.c-tri.a).cross(tri.b-tri.a).magnitude()>0.1){tris.push_back(tri);}
      }
    }else{//ascii stl
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
        mesh::meshtri tri{x0,y0,z0,x1,y1,z1,x2,y2,z2};
        if((
          (fabs(tri.c.x-tri.a.x)<0.1)+(fabs(tri.c.y-tri.a.y)<0.1)+(fabs(tri.c.z-tri.a.z)<0.1)+
          (fabs(tri.b.x-tri.a.x)<0.1)+(fabs(tri.b.y-tri.a.y)<0.1)+(fabs(tri.b.z-tri.a.z)<0.1))
          <3//ily
        ){continue;}
        if((tri.c-tri.a).cross(tri.b-tri.a).magnitude()>0.1){tris.push_back(tri);}
      }
    }
    free(tmp);
    tmp=NULL;
    DO(file){fclose(file);file=NULL;}else ORDIE("???")
    printf("%li triangles\n",tris.size());
    return tris;
  }
  texture_t readPPM(const char* filename){
    texture_t out;
    FILE* file=fopen(filename, "r");
    char* tmp = (char*)malloc(128);
    DO(!file){fflush(stdout);perror("couldn't open texture file for read");return default_texture;errno=0;};
    int width, height, maxVal;
    char format=0;
    FEXPECTS("P3",2){//segfaults i guess
      DO(memcmp(tmp,"P6",2)){fflush(stdout);perror("unsupported file format: not PPM3 or PPM6");return default_texture;errno=0;}{
        format=2;
      }
    }else{
      format=1;
    }
    wspace(file,tmp);FGETI(width);
    wspace(file,tmp);FGETI(height);
    wspace(file,tmp);FGETI(maxVal);
    printf("%ix%i,%i\n",width,height,maxVal);
    out.width=width;out.height=height;out.pixels=(unsigned char*)malloc(3*width*height);
    if(format==1){//ppm3
      unsigned int r,g,b,j=0;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
      while(!feof(file)&&(j<(width*height))&&!ferror(file)){
        wspace(file,tmp);FGETI(r);
        wspace(file,tmp);FGETI(g);
        wspace(file,tmp);FGETI(b);
        out.pixels[j*3]  =r*255/maxVal;
        out.pixels[j*3+1]=g*255/maxVal;
        out.pixels[j*3+2]=b*255/maxVal;
        j++;
      }
    }else if(format==2){//ppm6
      wspace(file,tmp);
      short unsigned r,g,b;
      unsigned j=0;
      while(!feof(file)&&(j<(width*height))&&!ferror(file)){
#pragma GCC diagnostic pop
        fread(&r,1+(maxVal>255),1,file);
        fread(&g,1+(maxVal>255),1,file);
        fread(&b,1+(maxVal>255),1,file);
        out.pixels[j*3]  =r*255/maxVal;
        out.pixels[j*3+1]=g*255/maxVal;
        out.pixels[j*3+2]=b*255/maxVal;
        //fprintf(debug,"%0.3u %0.3u %0.3u\n",r,g,b);
        j++;
      }
    }
    free(tmp);
    tmp=NULL;
    DO(file||ferror(file)){fclose(file);file=NULL;}else{fflush(stdout);perror("error reading file, using default texture");return default_texture;errno=0;}
    return out;
  }
#define EXPCORDIE(N,C,N1) DO((buddyholly=fgetc(file))!=C){printf("%li:\'%c\':",ftell(file),buddyholly);fflush(stdout);ORDIE(N " " #C " " N1)}
  static char readChar(FILE* file,char* tmp){
    char buddyholly;
    EXPCORDIE("expected",'\'',"to start character literal")
    char c=fgetc(file);
    if(c=='\\'){
      tmp[0]=fgetc(file);
      if(!((tmp[0]=='\\')||(tmp[0]=='\''))){
        // tmp[readUntil(file,tmp,'\'')]='\0';
        tmp[nspace(file,tmp+1)+1]='\0';//there's a buffer overflow here but im lowkirk lazy
        unsigned d=atoi(tmp);//and it's only 1 byte
        DO(d>255)ORDIE("char literal too big")
        c=d;
      }else{c=tmp[0];}
    }
    EXPCORDIE("expected",'\'',"to end character literal")
    return c;
  }
  static void doSVGLine(FILE* file,char* tmp,mesh::vec2<unsigned> a,mesh::vec2<unsigned> b,int c,char d,sprite_t* out,unsigned width,unsigned height){
    char e=(abs((int)a.x-(int)b.x)>abs((int)a.y-(int)b.y));
    if(e){
      if(a.x>b.x){
        mesh::vec2<unsigned> f=a;
        a=b;
        b=f;
      }
    }else{
      if(a.y>b.y){
        mesh::vec2<unsigned> f=a;
        a=b;
        b=f;
      }
    }
    float s=((float)b.y-a.y)/(b.x-a.x);
    static const float slopes[4]={tan(-M_PI_4*3/2),tan(-M_PI_4/2),tan(M_PI_4/2),tan(M_PI_4*3/2)};
    if(!d){d=(s<slopes[0])?'|':((s<slopes[1])?'\\':((s<slopes[2])?'-':((s<slopes[3])?'/':'|')));}
    if(e){
      for(unsigned i=a.x;i<b.x;i++){
        unsigned j=((int)((i-a.x)*s+a.y)*width)+i;
        memcpy(&out->pixels[j*3],&c,3);//they call me the developer
        out->chars[j]=d;
      }
    }else{
      s=((float)b.x-a.x)/(b.y-a.y);
      for(unsigned i=a.y;i<(b.y);i++){
        unsigned j=(i*width)+(unsigned)((i-a.y)*s)+a.x;
        memcpy(&out->pixels[j*3],&c,3);
        out->chars[j]=d;
      }
    }
  }
  sprite_t readRGVTX(const char* filename,unsigned int width,unsigned int height){//these are getting
    printf("loading asset %s@%ux%u\r\n",filename,width,height);
    if(!filename||!width||!height||!*filename){//bloated and could use some trimming
      printf("warning: illegal parameters (%p,%u,%u), using default texture >:(\r\n",filename,width,height);
      return default_texture;
    }
    sprite_t out{width,height,(unsigned char*)malloc(3*width*height),(char*)malloc(width*height)};
    memset(out.chars,'\0',width*height);
    memset(out.pixels,255,3*width*height);
    FILE* file=fopen(filename,"r");
    char* tmp = (char*)malloc(128);
    DO(!out.pixels)ORDIE("couldn't alloc for texture")
    DO(!tmp)ORDIE("couldn't alloc for read")
    DO(!file||ferror(file))ORDIE("couldn't open file for read")
    unsigned int token_length;
    size_t l=0;
    char buddyholly='\0';
    mesh::vec2<float> scale{1,1};
    while(!feof(file)&&!ferror(file)){
      wspace(file,tmp);
      token_length=nspace(file,tmp);
#define state(N) if((token_length==(l=strlen(N)))&&(!memcmp(tmp,N,l)))
#define getParam(T,N,F) T N=F(file,tmp);EXPCORDIE("expected",',',"to separate parameters");wspace(file,tmp)
      state("line"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start line call")
        wspace(file,tmp);
        getParam(mesh::vec2<float>,a,readV2f);
        getParam(mesh::vec2<float>,b,readV2f);
        int c=readColor(file,tmp);
        char d='\0';
        wspace(file,tmp);
        char z=fgetc(file);
        if(z==','){
          wspace(file,tmp);
          d=readChar(file,tmp);
          wspace(file,tmp);
          EXPCORDIE("expected",')',"to end line call")
        }else DO(z!=')'){
          printf("%li:\'%c\':",ftell(file),z);fflush(stdout);
          ORDIE("expected ')' to end line call or ',' to continue line call")
        }
        doSVGLine(file,tmp,a*scale,b*scale,c,d,&out,width,height);
      }else state("char"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start char call")
        wspace(file,tmp);
        getParam(mesh::vec2<float>,p,readV2f);p=p*scale;
        char c=readChar(file,tmp);
        wspace(file,tmp);
        EXPCORDIE("expected",')',"to end char call")
        out.chars[((unsigned)p.y*width)+(unsigned)p.x]=c;
      }else state("fill"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start fill call")
        getParam(mesh::vec2<float>,a,readV2f);a=a*scale;
        getParam(mesh::vec2<float>,b,readV2f);b=b*scale;
        getParam(int,c,readColor);
        char d=readChar(file,tmp);
        wspace(file,tmp);
        EXPCORDIE("expected",')',"to end fill call")
        DO((a.x>b.x)||(a.y>b.y))ORDIE("expected first point to be top left and second to be bottom right")
        for(unsigned y=a.y;y<b.y;y++){
          for(unsigned x=a.x;x<b.x;x++){
            memcpy(&out.pixels[3*(y*width+x)],&c,3);
            out.chars[(y*width+x)]=d;
          }
        }
      }else state("lines"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start lines call")
        wspace(file,tmp);
        getParam(std::vector<mesh::vec2<float>>,pts,readV2fVec);
        int c=readColor(file,tmp);
        char d='\0';
        wspace(file,tmp);
        char z=fgetc(file);
        if(z==','){
          wspace(file,tmp);
          d=readChar(file,tmp);
          wspace(file,tmp);
          EXPCORDIE("expected",')',"to end line call")
        }else DO(z!=')'){
          printf("%li:\'%c\':",ftell(file),z);fflush(stdout);
          ORDIE("expected ')' to end line call or ',' to continue lines call")
        }
        for(unsigned i=1;i<pts.size();i++){
          doSVGLine(file,tmp,pts[i-1]*scale,pts[i]*scale,c,d,&out,width,height);
        }
      }else state("scale"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start scale call")
        tmp[nspace(file,tmp)]='\0';scale.x=(width-1)/atof(tmp);
        EXPCORDIE("expected",',',"to delimit parameters");wspace(file,tmp);
        tmp[nspace(file,tmp)]='\0';scale.y=(height-1)/atof(tmp);wspace(file,tmp);
        EXPCORDIE("expected",')',"to end scale call")
      }else state("color"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start color call")
        wspace(file,tmp);
        getParam(mesh::vec2<float>,p,readV2f);p=p*scale;
        int c=readColor(file,tmp);wspace(file,tmp);
        EXPCORDIE("expected",')',"to end color call")
        memcpy(&out.pixels[3*(((unsigned)p.y*width)+(unsigned)p.x)],&c,3);
      }else state("polygon"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start triangle call")
        wspace(file,tmp);
        getParam(std::vector<mesh::vec2<float>>,pts,readV2fVec);
        getParam(int,c,readColor);
        char d=readChar(file,tmp);wspace(file,tmp);
        EXPCORDIE("expected",')',"to end triangle call")
        DO(pts.size()<3)ORDIE("not enough coordinates for a polygon")
        float minx=out.width,maxx=0,miny=out.height,maxy=0;
        for(unsigned i=0;i<pts.size();i++){
          minx=min(pts[i].x,minx);maxx=max(pts[i].x,maxx);
          miny=min(pts[i].y,miny);maxy=max(pts[i].y,maxy);
        }
        minx*=scale.x;maxx*=scale.x;miny*=scale.y;maxy*=scale.y;
        // fprintf(debug,"(%.3f,%.3f),(%.3f,%.3f)\n",minx,miny,maxx,maxy);
        for(int x=minx;x<maxx;x++){//lowk ear but it works i think
          for(int y=miny;y<maxy;y++){
            for(unsigned i=2;i<pts.size();i++){
              mesh::vec2<int> a=pts[0]*scale,b=pts[i]*scale,c=pts[i-1]*scale;
              int a0=abs(triarea(a.x,a.y,b.x,b.y,c.x,c.y));
              int a1=abs(triarea(x,y,b.x,b.y,c.x,c.y))+
              abs(triarea(a.x,a.y,x,y,c.x,c.y))+
              abs(triarea(a.x,a.y,b.x,b.y,x,y));
              if(a1<=a0){
                goto draw;
              }
            }
            goto end;
            draw:
            memcpy(&out.pixels[3*(y*width+x)],&c,3);
            out.chars[(y*width+x)]=d;
            end:;
          }
        }
      }/*else state("circle"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start circle call")
        wspace(file,tmp);
        getParam(mesh::vec2<float>,p,readV2f);p=p*scale;
        tmp[nspace(file,tmp)]='\0';
        int r=atof(tmp);//do later
      }*/else state("triangle"){
        wspace(file,tmp);
        EXPCORDIE("expected",'(',"to start triangle call")
        wspace(file,tmp);
        getParam(mesh::vec2<int>,a,readV2f);a=a*scale;
        getParam(mesh::vec2<int>,b,readV2f);b=b*scale;
        getParam(mesh::vec2<int>,c,readV2f);c=c*scale;
        getParam(int,d,readColor);
        char e=readChar(file,tmp);
        wspace(file,tmp);
        EXPCORDIE("expected",')',"to end triangle call")
        for(int x=min(a.x,b.x,c.x);x<max(a.x,b.x,c.x);x++){
          for(int y=min(a.y,b.y,c.y);y<max(a.y,b.y,c.y);y++){
            if(
              (triarea(x,y,b.x,b.y,c.x,c.y)>=0)&&
              (triarea(a.x,a.y,x,y,c.x,c.y)>=0)&&
              (triarea(a.x,a.y,b.x,b.y,x,y)>=0)
            ){
              memcpy(&out.pixels[3*(y*width+x)],&d,3);
              out.chars[(y*width+x)]=e;
            }
          }
        }
      }else if(!feof(file)){
#undef getParam
#undef state//should prolly use allat in the other one but im lazy
        printf("unrecognized token %.*s at %li! skipping line\n",token_length,tmp,ftell(file));
        while(fgetc(file)!='\n'){}
      }
    }
    return out;
  }
  assets::asset3d_t readAsset3d(const char* name) {
#define ORDIE1(S) {if(mesh_fp){free(mesh_fp);mesh_fp=NULL;}if(out.mesh.tris){free(out.mesh.tris);out.mesh.tris=NULL;}perror(S);if(file){fclose(file);file=NULL;}if(tmp){free(tmp);tmp=NULL;};exit(1);}
#define ORTHENDIE1(c,s){c;ORDIE1(s)}
#undef  EXPCORDIE
#define EXPCORDIE(N,C,N1) DO((buddyholly=fgetc(file))!=C){printf("%li:\'%c\':",ftell(file),buddyholly);fflush(stdout);ORDIE1(N " " #C " " N1)}
    char buddyholly;
    DO(strlen(name)>=128){perror("file name too long");exit(1);}
    printf("loading asset %s:\n",name);
    asset3d_t out{};
    FILE* file=fopen(name,"r");
    char* tmp=(char*)malloc(128);
    tmp[127]='\0';
    DO(!file)ORDIE("couldn't open asset file for read :(")
    DO(!tmp)ORDIE("couldn't alloc memory for tmp buffer")
    char* mesh_fp=(char*)calloc(128,1);
    DO(!mesh_fp)ORDIE("couldn't alloc memory for mesh file path for asset")
    char* uvassignedtris=NULL;//bool size is implementation defined so we don't use it
    std::vector<texture_t> textures{};
    while(!feof(file)){
      wspace(file,tmp);
      unsigned int token_length=nspace(file,tmp);
      DO(!token_length)ORTHENDIE1(printf("\"%s\" at %li:",tmp,ftell(file));fflush(stdout),"bad tokens in asset")
      if((token_length==5)&&!(memcmp(tmp,"model",5))){
        DO(mesh_fp[0])ORDIE1("duplicate models in asset")
        wspace(file,tmp);DO(fgetc(file)!='=')ORDIE("expected '=' to assign model path");wspace(file,tmp);
        token_length=readUntil(file,tmp,';');
        printf("  reading model file assets/model/%.*s:",token_length,tmp);
        DO(!token_length)ORDIE1("bad model filepath in asset")
        DO((13+token_length+1)>=128)ORDIE1("model filepath too long")
        tmp[token_length]='\0';
        memcpy(mesh_fp,"assets/model/",13);
        memcpy(&mesh_fp[13],tmp,token_length+1);
        std::vector<mesh::meshtri> tris=readSTL(mesh_fp);//go out of scope and free yourself NOW
        out.mesh.tricount=tris.size();
        size_t s=out.mesh.tricount*sizeof(mesh::meshtri);
        out.mesh.tris=(mesh::meshtri*)calloc(s,1);
        uvassignedtris=(char*)calloc(out.mesh.tricount,1);
        out.tex_binds=(unsigned char*)calloc(out.mesh.tricount,1);
        memcpy(out.mesh.tris,&tris[0],s);
      }else if((token_length==6)&&!(memcmp(tmp,"texmap",6))){
        DO(!mesh_fp[0])ORDIE1("expected model before texmap")
        wspace(file,tmp);EXPCORDIE("expected",'=',"to assign texmap");wspace(file,tmp);
        std::vector<mesh::vec2<float>> v2fv=readV2fVec(file,tmp);
        unsigned int tricount=v2fv.size()/3;
        DO(tricount>out.mesh.tricount)ORDIE1("too many uv coordinates")
        for(unsigned int i=0;i<tricount;i++){
          out.mesh.tris[i].uv0=v2fv[i*3];
          out.mesh.tris[i].uv1=v2fv[i*3+1];
          out.mesh.tris[i].uv2=v2fv[i*3+2];
          uvassignedtris[i]=1;
        }
      }else if(token_length==7){
        if(!memcmp(tmp,"texture",7)){
          DO(out.textures)ORDIE1("duplicate textures in asset")
          wspace(file,tmp);EXPCORDIE("expected",'=',"to assign texture path");wspace(file,tmp);
          char c=fgetc(file);
          if(c=='['){
            while(c!=']'){
              wspace(file,tmp);
              token_length=readUntil(file,tmp,',',']');
              DO((15+token_length+1)>=128)ORDIE1("texture filepath too long")
              memmove(tmp+15,tmp,token_length);
              memcpy(tmp,"assets/texture/",15);
              tmp[15+token_length]='\0';
              printf("  reading texture file %s:",tmp);
              textures.push_back(readPPM(tmp));
              c=fgetc(file);
            }
          }else{
            tmp[0]=c;
            token_length=readUntil(file,&tmp[1],';')+1;
            DO(!token_length)ORDIE1("bad texture filepath in asset")
            DO((15+token_length+1)>=128)ORDIE1("texture filepath too long")
            memmove(tmp+15,tmp,token_length);
            memcpy(tmp,"assets/texture/",15);
            tmp[15+token_length]='\0';
            printf("  reading textures %s:",tmp);
            textures.push_back(readPPM(tmp));
          }
        }else if(!memcmp(tmp,"tex_set",7)){
          DO(!out.mesh.tricount)ORDIE1("you need a model to assign triangle texture indices, dumbass")
          EXPCORDIE("expected",'(',"to start tex_set")
          unsigned int i;FGETI(i);
          EXPCORDIE("expected",',',"to delimit parameters of tex_set")
          unsigned int j;FGETI(j);
          EXPCORDIE("expected",')',"to end tex_set")
          DO(i>=out.mesh.tricount)ORDIE1("tex_set triangle index out of bounds")
          DO(j>=textures.size())ORDIE1("tex_set texture index out of bounds")
          out.tex_binds[i]=(unsigned char)j;
        }
      }else if((token_length==9)&&!(memcmp(tmp,"tex_binds",9))){
        wspace(file,tmp);
        EXPCORDIE("expected",'=',"to assign tex_binds")
        wspace(file,tmp);
        char c=fgetc(file);
        DO(c!='[')ORDIE1("expected '[' to start tex_binds")
        wspace(file,tmp);
        unsigned int i=0;
        do{
          unsigned int j;
          FGETI(j);
          out.tex_binds[i]=j;
          i++;
          wspace(file,tmp);
          c=fgetc(file);//could definitely optimize the constant ungetc and fgetc calls but
          wspace(file,tmp);
        }while((c==',')&&(i<out.mesh.tricount));//like that's a whole load of work :/
        DO(c!=']')ORDIE1("expected ']' to end tex_binds")
      }else if((token_length==11)&&!(memcmp(tmp,"model_scale",11))){
          DO(!out.mesh.tricount)ORDIE1("expected model before model_scale")
          EXPCORDIE("expected",'(',"to start model_scale parameters")
          tmp[nspace(file,tmp)]='\0';
          float x=atof(tmp);
          EXPCORDIE("expected",',',"to separate model_scale parameters")
          tmp[nspace(file,tmp)]='\0';
          float y=atof(tmp);
          EXPCORDIE("expected",',',"to separate model_scale parameters")
          tmp[nspace(file,tmp)]='\0';
          float z=atof(tmp);
          EXPCORDIE("expected",')',"to end model_scale parameters")
          mesh::vec3<float> scale{x,y,z};
          for(unsigned int i=0;i<out.mesh.tricount;i++){
            out.mesh.tris[i]=out.mesh.tris[i]*scale;
          }
      }else if(token_length==12){
        if(!memcmp(tmp,"texmap_scale",12)){
          EXPCORDIE("expected",'(',"to start texmap_scale parameters")
          tmp[nspace(file,tmp)]='\0';
          float x=atof(tmp);
          EXPCORDIE("expected",',',"to separate texmap_scale parameters")
          tmp[nspace(file,tmp)]='\0';
          float y=atof(tmp);
          EXPCORDIE("expected",')',"to end texmap_scale parameters")
          for(unsigned int i=0;i<out.mesh.tricount;i++){
            out.mesh.tris[i].uv0.x*=x;
            out.mesh.tris[i].uv1.x*=x;
            out.mesh.tris[i].uv2.x*=x;
            out.mesh.tris[i].uv0.y*=y;
            out.mesh.tris[i].uv1.y*=y;
            out.mesh.tris[i].uv2.y*=y;
          }
        }else if(!memcmp(tmp,"model_offset",12)){
          DO(!out.mesh.tricount)ORDIE1("expected model before model_offset")
          EXPCORDIE("expected",'(',"to start model_offset parameters")
          tmp[nspace(file,tmp)]='\0';
          float x=atof(tmp);
          EXPCORDIE("expected",',',"to separate model_offset parameters")
          tmp[nspace(file,tmp)]='\0';
          float y=atof(tmp);
          EXPCORDIE("expected",',',"to separate model_offset parameters")
          tmp[nspace(file,tmp)]='\0';
          float z=atof(tmp);
          EXPCORDIE("expected",')',"to end model_offset parameters")
          mesh::vec3<float> off{x,y,z};
          for(unsigned int i=0;i<out.mesh.tricount;i++){
            out.mesh.tris[i]=out.mesh.tris[i]+off;
          }
        }else if(!memcmp(tmp,"model_rotate",12)){
          DO(!out.mesh.tricount)ORDIE1("expected model before model_rotate")
          EXPCORDIE("expected",'(',"to start model_rotate parameters")
          tmp[nspace(file,tmp)]='\0';
          float x=atof(tmp)*255;
          EXPCORDIE("expected",',',"to separate model_rotate parameters")
          tmp[nspace(file,tmp)]='\0';
          float y=atof(tmp)*255;
          EXPCORDIE("expected",',',"to separate model_rotate parameters")
          tmp[nspace(file,tmp)]='\0';
          float z=atof(tmp)*255;
          EXPCORDIE("expected",')',"to end model_rotate parameters")
          for(unsigned int i=0;i<out.mesh.tricount;i++){
            if(x){
              mesh::rotate(out.mesh.tris[i].a.y,out.mesh.tris[i].a.z,(signed char)x);
              mesh::rotate(out.mesh.tris[i].b.y,out.mesh.tris[i].b.z,(signed char)x);
              mesh::rotate(out.mesh.tris[i].c.y,out.mesh.tris[i].c.z,(signed char)x);
            }
            if(y){
              mesh::rotate(out.mesh.tris[i].a.z,out.mesh.tris[i].a.x,(signed char)y);
              mesh::rotate(out.mesh.tris[i].b.z,out.mesh.tris[i].b.x,(signed char)y);
              mesh::rotate(out.mesh.tris[i].c.z,out.mesh.tris[i].c.x,(signed char)y);
            }
            if(z){
              mesh::rotate(out.mesh.tris[i].a.x,out.mesh.tris[i].a.y,(char)z);
              mesh::rotate(out.mesh.tris[i].b.x,out.mesh.tris[i].b.y,(char)z);
              mesh::rotate(out.mesh.tris[i].c.x,out.mesh.tris[i].c.y,(char)z);
            }
          }
        
        }else if(!memcmp(tmp,"texbind_fill",12)){
          EXPCORDIE("expected",'(',"to start texbind_fill")
          unsigned char mx=0,my=0,mz=0,px=0,py=0,pz=0,d;
          signed char s=0;
          for(unsigned int i=0;i<6;i++){
            char c;
            hate:
            c=fgetc(file);
            switch(c){
              case '-':s=-1;goto hate;
              case '+':s= 1;goto hate;
              default:
              DO((unsigned char)(c-'x')>2)ORTHENDIE1(printf("%li:\'%c\':",ftell(file),c);fflush(stdout),"expected x, y, or z for texbind_fill")
              EXPCORDIE("expected",':',"to delimit texbind_fill")
              FGETI(d);
              DO(d>=textures.size())ORDIE1("texbind_fill index out of bounds :E")
              if(c=='x'){if(s>=0){px=d;}if(s<=0){mx=d;}}else
              if(c=='y'){if(s>=0){py=d;}if(s<=0){my=d;}}else
              if(c=='z'){if(s>=0){pz=d;}if(s<=0){mz=d;}}else
              ORDIE1("unrecognized texbind_fill character")
              // printf("%c%c:%u,%u\n",(s<0)?'-':((s>0)?'+':' '),c,(c=='x')?mx:((c=='y')?my:mz),(c=='x')?px:((c=='y')?py:pz));
              s=0;
              if((buddyholly=fgetc(file))!=')'){DO(buddyholly!=',')ORTHENDIE1(printf("%li:\'%c\':",ftell(file),c);fflush(stdout),"expected ',' to delimit texbind_fill or ')' to end it")}
              else{goto end;}
            }
          }
          end:;
          for(unsigned int i=0;i<out.mesh.tricount;i++){
            if(uvassignedtris[i]){
              mesh::vec3<float> normal=(out.mesh.tris[i].c-out.mesh.tris[i].a).cross(out.mesh.tris[i].b-out.mesh.tris[i].a);
              mesh::vec3<float> absnor{abs(normal.x),abs(normal.y),abs(normal.z)};
              unsigned char m=(absnor.x>absnor.y)|((absnor.y>absnor.z)<<1)|((absnor.z>absnor.x)<<2);
              switch(m){
                case 0:[[fallthrough]];//'bad' cases - all equal
                case 3:[[fallthrough]];//or circular comparison
                case 7:[[fallthrough]];//x>y>z>x
                case 1://x>y<z<x
                out.tex_binds[i]=(normal.x>=0)?px:mx;break;
                case 2:[[fallthrough]];//x<y>z<x
                case 6://x<y>z>x
                out.tex_binds[i]=(normal.y>=0)?py:my;break;
                case 4:[[fallthrough]];//x<y<z>x
                case 5://x>y<z>x
                out.tex_binds[i]=(normal.z>=0)?pz:mz;break;
              }
            }
          }
        }
      }else if((token_length==13)&&!memcmp(tmp,"texmap_offset",13)){
        EXPCORDIE("expected",'(',"to start texmap_offset parameters")
        tmp[nspace(file,tmp)]='\0';
        float x=atof(tmp);
        EXPCORDIE("expected",',',"to separate texmap_offset parameters")
        tmp[nspace(file,tmp)]='\0';
        float y=atof(tmp);
        EXPCORDIE("expected",')',"to end texmap_offset parameters")
        for(unsigned int i=0;i<out.mesh.tricount;i++){
          if(uvassignedtris[i]){
            out.mesh.tris[i].uv0.x+=x;
            out.mesh.tris[i].uv1.x+=x;
            out.mesh.tris[i].uv2.x+=x;
            out.mesh.tris[i].uv0.y+=y;
            out.mesh.tris[i].uv1.y+=y;
            out.mesh.tris[i].uv2.y+=y;
          }
        }
      }else if((token_length==18)&&!memcmp(tmp,"texmap_filldefault",18)){
        EXPCORDIE("expected",'(',"to start texmap_filldefault parameters")
        wspace(file,tmp);
        switch(fgetc(file)){
          case 'x':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.x=out.mesh.tris[i].a.x;
                out.mesh.tris[i].uv1.x=out.mesh.tris[i].b.x;
                out.mesh.tris[i].uv2.x=out.mesh.tris[i].c.x;
              }
            }break;
          case 'y':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.x=out.mesh.tris[i].a.y;
                out.mesh.tris[i].uv1.x=out.mesh.tris[i].b.y;
                out.mesh.tris[i].uv2.x=out.mesh.tris[i].c.y;
              }
            }break;
          case 'z':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.x=out.mesh.tris[i].a.z;
                out.mesh.tris[i].uv1.x=out.mesh.tris[i].b.z;
                out.mesh.tris[i].uv2.x=out.mesh.tris[i].c.z;
              }
            }break;
          case ')':
            goto noparam;
          default:
            ORDIE1("unrecognized texmap_filldefault parameter")
        }
        wspace(file,tmp);EXPCORDIE("expected",',',"to separate texmap_filldefault parameters")
        switch(fgetc(file)){
          case 'x':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.y=out.mesh.tris[i].a.x;
                out.mesh.tris[i].uv1.y=out.mesh.tris[i].b.x;
                out.mesh.tris[i].uv2.y=out.mesh.tris[i].c.x;
              }
            }break;
          case 'y':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.y=out.mesh.tris[i].a.y;
                out.mesh.tris[i].uv1.y=out.mesh.tris[i].b.y;
                out.mesh.tris[i].uv2.y=out.mesh.tris[i].c.y;
              }
            }break;
          case 'z':
            for(unsigned int i=0;i<out.mesh.tricount;i++){
              if(!uvassignedtris[i]){
                out.mesh.tris[i].uv0.y=out.mesh.tris[i].a.z;
                out.mesh.tris[i].uv1.y=out.mesh.tris[i].b.z;
                out.mesh.tris[i].uv2.y=out.mesh.tris[i].c.z;
              }
            }break;
          default:
            ORDIE1("unrecognized texmap_filldefault parameter")
        }
        wspace(file,tmp);EXPCORDIE("expected",')',"to end texmap_filldefault")
        for(unsigned int i=0;i<out.mesh.tricount;i++){
          uvassignedtris[i]=1;
        }
        continue;
        noparam:
        for(unsigned int i=0;i<out.mesh.tricount;i++){
          if(!uvassignedtris[i]){
            #define tri out.mesh.tris[i]
            mesh::vec3<float> n =(tri.c-tri.a).cross(tri.b-tri.a);
            mesh::vec3<float> n1=n.cross(tri.c-tri.a);
            mesh::vec3<float> n2=tri.c-tri.a;
            //all centered on tri.a
            tri.uv0={0,0};
            tri.uv1={sqrtf(abs((n2*(tri.a-tri.c)).total())),0};
            tri.uv2={sqrtf(abs((n2*(tri.a-tri.b)).total())),sqrtf(abs((n1*(tri.a-tri.b)).total()))};
            #undef tri
            uvassignedtris[i]=1;
          }
        }
      }
    }
    DO(({unsigned i;for(i=0;i<out.mesh.tricount;i++){i+=uvassignedtris[i]?1:0;}i;})!=out.mesh.tricount)ORDIE1("didn't assign enough uv coordinates")
    out.textures=(assets::texture_t*)malloc(sizeof(assets::texture_t)*textures.size());
    memcpy(out.textures,&textures[0],textures.size()*sizeof(assets::texture_t));
    free(tmp);tmp=NULL;
    free(mesh_fp);mesh_fp=NULL;
    free(uvassignedtris);uvassignedtris=NULL;
    DO(file){fclose(file);file=NULL;}else ORDIE("???")
    return out;
#undef ORDIE1
#undef ORTHENDIE1
#undef EXPCORDIE
  }
  font_t readFont(const char* name){//we could probably standardize systems of scanning files because lots of this code is reused
    DO(strlen(name)>=128){perror("file name too long");exit(1);}//but we only have 2 formats so that's not an issue rn
    printf("loading asset %s:",name);
    FILE* file=fopen(name,"r");
    char* tmp=(char*)malloc(256);
    DO(!file)ORDIE("couldn't open asset file for read :(")
    DO(!tmp)ORDIE("couldn't alloc memory for tmp buffer")
    DO(errno)ORDIE("unexpected error when opening file");
    DO(ferror(file))ORDIE("unexpected error when oping file");
    DO(feof(file))ORDIE("file too short. like way too short.");
    font_t out{};
    wspace(file,tmp);
    tmp[readUntil(file,tmp,'x')]='\0';getc(file);
    char x=atoi(tmp);
    out.sizex=(unsigned char*)malloc(256);
    memset(out.sizex,x,256);
    tmp[nspace(file,tmp)]='\0';
    out.sizey=atoi(tmp);
    DO(!out.sizey)ORTHENDIE(printf("error at %li:",ftell(file)),"need to have font height greater than 0")
    printf("%ix%i\n",(*out.sizex?*out.sizex:0),out.sizey);
    out.map=(char**)calloc(256,sizeof(char*));
    size_t amt=0;
    unsigned char readTo=0;
    wspace(file,tmp);
    while(!feof(file)){
      unsigned int token_length=nspace(file,tmp);
      DO(!token_length)ORTHENDIE(printf("\"%s\" at %li:",tmp,ftell(file));fflush(stdout),"bad tokens in asset")
      if(token_length==14){
        if(!memcmp(tmp,"alphabet_upper",14)){
          amt=26;readTo='A';
        }else if(!memcmp(tmp,"alphabet_lower",14)){
          amt=26;readTo='a';
        }
      }else if(token_length==7){
        if(!memcmp(tmp,"special",7)){
          amt=33;readTo=' ';
        }
      }else if(token_length==8){
        if(!memcmp(tmp,"special2",8)){
          amt=6;readTo='[';
        }else if(!memcmp(tmp,"special3",8)){
          amt=4;readTo='{';
        }
      }
      DO(readTo){
        DO(getc(file)!='\n')ORTHENDIE(printf("expected newline at %li after \"%.*s\"!\n",ftell(file),token_length,tmp),"bad read")
        unsigned int total=0;
        if(!out.sizex[readTo]){
          for(unsigned int i=0;i<amt;i++){
            total+=(out.sizex[readTo+i]=readUntil(file,tmp,'.')+1);getc(file);
          }
          DO(getc(file)!='\n')ORTHENDIE(printf("expected newline at %li after width!\n",ftell(file)),"bad read")
        }
        for(unsigned int i=0;i<out.sizey;i++){
          for(unsigned int j=0;j<amt;j++){
            if(!out.map[readTo+j]){out.map[readTo+j]=(char*)malloc(out.sizey*out.sizex[readTo+j]);}
            DO((token_length=fread(&out.map[readTo+j][i*out.sizex[readTo+j]],1,out.sizex[readTo+j],file))!=out.sizex[readTo+j])
            ORTHENDIE(
              printf("did't get enough characters: %i/%hhn at %li:(%i,%i):%lu\n",
                token_length,out.sizex,ftell(file),j,i,amt)
              ,"bad read")
            DO(errno)ORDIE("unexpected error while reading");
      	    DO(ferror(file))ORTHENDIE(printf("error at %li!\n",ftell(file));,"unexpected error while reading");
      	    DO(feof(file))ORTHENDIE(printf("error at %li!\n",ftell(file));,"unexpected end of file");
          }
          getc(file);
        }
      }else ORTHENDIE(printf("\"%.*s\":",token_length,tmp),"unknown token :E")
      wspace(file,tmp);
    }
    if(out.map['a']&&!out.map['A']){
      memcpy(&out.map['A'],&out.map['a'],26*sizeof(char*));
      memcpy(&out.sizex['A'],&out.sizex['a'],26);
    }else if(out.map['A']&&!out.map['a']){
      memcpy(&out.map['a'],&out.map['A'],26*sizeof(char*));
      memcpy(&out.sizex['a'],&out.sizex['A'],26);
    }
    /*memset(&out.map[out.sizex*out.sizey*(unsigned char)' '],' ',out.sizex*out.sizey);
    if((*UPPER(out)!=c)&&(*LOWER(out)==c)){
      memcpy(LOWER(out),UPPER(out),26*out.sizex*out.sizey);
    }else if((*UPPER(out)==c)&&(*LOWER(out)!=c)){
      memcpy(UPPER(out),LOWER(out),26*out.sizex*out.sizey);
    }*/
    DO(file){fclose(file);file=NULL;}else ORDIE("???")
    free(tmp);tmp=NULL;
    return out;
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
