/* $Id: bmltest_process.c,v 1.1 2005-05-18 12:47:25 ensonic Exp $
 * invoke it e.g. as
 *   env LD_LIBRARY_PATH="." ./bmltest_process ../machines/elak_svf.dll input.raw output.raw
 *
 * the dll *must* be a buzz-machine, no error checking ;-)
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "bml.h"

#define BUFFER_SIZE 1024

void test_process(const char *dllpath,const char *infilename,const char *outfilename) {
  // buzz machine handle
  void *bm;
  char *str;
  char *fulldllpath;
 
  fulldllpath=bml_convertpath(dllpath);

  printf("\n"__FUNCTION__"(\"%s\" -> \"%s\")\n",dllpath,fulldllpath);
  
  if(bm=bm_new(fulldllpath)) {
    FILE *infile,*outfile;
    int s_size=BUFFER_SIZE,i_size;
    short int buffer_w[BUFFER_SIZE];
    float buffer_f[BUFFER_SIZE];
    int i,ival=0,oval,vs=10;
    
		puts("  machine created");
		bm_init(bm);
    puts("  machine initialized");

    // open raw files
    infile=fopen(infilename,"rb");
    outfile=fopen(outfilename,"wb");
    if(infile && outfile) {
      printf("    processing ");
      while(!feof(infile)) {
        // assumes the first param is of pt_word type 
        //bm_set_global_parameter_value(bm,0,ival);
        //oval=bm_get_global_parameter_value(bm,0);printf("        Value: %d\n",oval);
        //ival+=vs;
        //if(((vs>0) && (ival==1000)) || ((vs<0) && (ival==0))) vs=-vs; 
        
        printf(".");
        // set GlobalVals, TrackVals
        bm_tick(bm);
        i_size=fread(buffer_w,2,s_size,infile);
        for(i=0;i<i_size;i++) buffer_f[i]=(float)buffer_w[i]/32768.0;
        bm_work(bm,buffer_f,i_size,3/*WM_READWRITE*/);
        for(i=0;i<i_size;i++) buffer_w[i]=(short int)(buffer_f[i]*32768.0);
        fwrite(buffer_w,2,i_size,outfile);
      }
      //printf("\n");
    }
    else puts("    file error!");
    if(infile) fclose(infile);
    if(outfile) fclose(outfile);
    puts("  done");
    bm_free(bm);
  }
}

int main( int argc, char **argv ) {
  int i;
  
  puts("main beg");

  if(bml_init(0)) {

    bm_set_master_info(120,4,44100);
    puts("  master info initialized");

    if(argc>2) {
      test_process(argv[1],argv[2],argv[3]);
    }
    else puts("    not enough args!");
    bml_done();
  }

  puts("main end");
  return 0;
}

