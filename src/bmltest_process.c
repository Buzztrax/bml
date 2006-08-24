/* $Id: bmltest_process.c,v 1.5 2006-08-24 19:24:22 ensonic Exp $
 *
 * Buzz Machine Loader
 * Copyright (C) 2006 Buzztard team <buzztard-devel@lists.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
/**
 * process data with a buzz machine
 *
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
  char *fulldllpath;
 
  fulldllpath=bml_convertpath((char *)dllpath);

  printf("%s(\"%s\" -> \"%s\")\n",__FUNCTION__,dllpath,fulldllpath);
  
  if((bm=bml_new(fulldllpath))) {
    FILE *infile,*outfile;
    int s_size=BUFFER_SIZE,i_size;
    short int buffer_w[BUFFER_SIZE];
    float buffer_f[BUFFER_SIZE];
    int i;
    //int ival=0,oval,vs=10;
    
    puts("  machine created");
    bml_init(bm);
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
        bml_tick(bm);
        i_size=fread(buffer_w,2,s_size,infile);
        for(i=0;i<i_size;i++) buffer_f[i]=(float)buffer_w[i]/32768.0;
        bml_work(bm,buffer_f,i_size,3/*WM_READWRITE*/);
        for(i=0;i<i_size;i++) buffer_w[i]=(short int)(buffer_f[i]*32768.0);
        fwrite(buffer_w,2,i_size,outfile);
      }
      //printf("\n");
    }
    else puts("    file error!");
    if(infile) fclose(infile);
    if(outfile) fclose(outfile);
    puts("  done");
    bml_free(bm);
  }
}

int main( int argc, char **argv ) {

  puts("main beg");

  if(bml_setup(0)) {

    bml_set_master_info(120,4,44100);
    puts("  master info initialized");

    if(argc>2) {
      test_process(argv[1],argv[2],argv[3]);
    }
    else puts("    not enough args!");
    bml_finalize();
  }

  puts("main end");
  return 0;
}
