/* $Id: bmltest_process.c,v 1.10 2007-11-10 19:06:37 ensonic Exp $
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
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ../machines/elak_svf.dll input.raw output1.raw
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ../../buzzmachines/Elak/SVF/libelak_svf.so input.raw output2.raw
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_process ~/buzztard/lib/Gear/Effects/Jeskola\ NiNjA\ dElaY.dll  input.raw output1.raw
 *
 * the dll/so *must* be a buzz-machine, not much error checking ;-)
 *
 * aplay -fS16_LE -r44100 input.raw
 * aplay -fS16_LE -r44100 output1.raw
 *
 * plot [0:] [-35000:35000] 'output1.raw' binary format="%short" using 1 with lines
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "bml.h"

// like MachineInterface.h::MAX_BUFFER_LENGTH
#define BUFFER_SIZE 256

#ifdef HAVE_X86
void test_process_w(char *libpath,const char *infilename,const char *outfilename) {
  // buzz machine handle
  void *bm;
 
  printf("%s(\"%s\")\n",__FUNCTION__,libpath);
  
  if((bm=bmlw_new(libpath))) {
    FILE *infile,*outfile;
    int s_size=BUFFER_SIZE,i_size;
    short int buffer_w[BUFFER_SIZE];
    float buffer_f[BUFFER_SIZE];
    int i,mtype;
    //int ival=0,oval,vs=10;
    const char *type_name[3]={"","generator","effect"};
    int nan=0,inf=0;
    float ma=0.0;
    
    puts("  windows machine created");
    bmlw_init(bm,0,NULL);
    bmlw_get_machine_info(bm,BM_PROP_TYPE,&mtype);
    printf("  %s initialized\n",type_name[mtype]);
    
    //bmlw_stop(bm);
    //bmlw_set_num_tracks(bm,2);
    //bmlw_set_num_tracks(bm,1);
    bmlw_attributes_changed(bm);

    // open raw files
    infile=fopen(infilename,"rb");
    outfile=fopen(outfilename,"wb");
    if(infile && outfile) {
      printf("    processing ");
      if(mtype==1) {
        int num,ptype,pflags;

        // trigger a note for generators
        bmlw_get_machine_info(bm,BM_PROP_NUM_GLOBAL_PARAMS,&num);
        // set value for trigger parameter(s)
        for(i=0;i<num;i++) {
          bmlw_get_global_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&pflags);
          if(!(pflags&2)) {
            bmlw_get_global_parameter_info(bm,i,BM_PARA_TYPE,(void *)&ptype);
            switch(ptype) {
              case 0: // note
                bmlw_set_global_parameter_value(bm,i,32);
                break;
              case 1: // switch
                bmlw_set_global_parameter_value(bm,i,1);
                break;
            }
          }
        }
        bmlw_get_machine_info(bm,BM_PROP_NUM_TRACK_PARAMS,&num);
        for(i=0;i<num;i++) {
          bmlw_get_track_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&pflags);
          if(!(pflags&2)) {
            bmlw_get_track_parameter_info(bm,i,BM_PARA_TYPE,(void *)&ptype);
            switch(ptype) {
              case 0: // note
                bmlw_set_track_parameter_value(bm,i,0,32);
                break;
              case 1: // switch
                bmlw_set_track_parameter_value(bm,i,0,1);
                break;
            }
          }
        }
      }
      while(!feof(infile)) {
        // assumes the first param is of pt_word type 
        //bm_set_global_parameter_value(bm,0,ival);
        //oval=bm_get_global_parameter_value(bm,0);printf("        Value: %d\n",oval);
        //ival+=vs;
        //if(((vs>0) && (ival==1000)) || ((vs<0) && (ival==0))) vs=-vs; 
        
        printf(".");
        // set GlobalVals, TrackVals
        bmlw_tick(bm);
        i_size=fread(buffer_w,2,s_size,infile);
        for(i=0;i<i_size;i++) buffer_f[i]=(float)buffer_w[i]/32768.0f;
        bmlw_work(bm,buffer_f,i_size,3/*WM_READWRITE*/);
        for(i=0;i<i_size;i++) {
          if(isnan(buffer_f[i])) nan=1;
          if(isinf(buffer_f[i])) inf=1;
          if(fabs(buffer_f[i])>ma) ma=buffer_f[i];
          buffer_w[i]=(short int)(buffer_f[i]*32768.0f);
        }
        fwrite(buffer_w,2,i_size,outfile);
      }
      //printf("\n");
    }
    else puts("    file error!");
    if(infile) fclose(infile);
    if(outfile) fclose(outfile);
    puts("  done");
    if(nan) puts("some values are nan");
    if(inf) puts("some values are inf");
    printf("max value : %f\n",ma);
    bmlw_free(bm);
  }
}
#endif  /* HAVE_X86 */

void test_process_n(char *libpath,const char *infilename,const char *outfilename) {
  // buzz machine handle
  void *bm;

  printf("%s(\"%s\")\n",__FUNCTION__,libpath);
  
  if((bm=bmln_new(libpath))) {
    FILE *infile,*outfile;
    int s_size=BUFFER_SIZE,i_size;
    short int buffer_w[BUFFER_SIZE];
    float buffer_f[BUFFER_SIZE];
    int i,mtype;
    //int ival=0,oval,vs=10;
    const char *type_name[3]={"","generator","effect"};
    int nan=0,inf=0;
    float ma=0.0;
    
    puts("  native machine created");
    bmln_init(bm,0,NULL);
    bmln_get_machine_info(bm,BM_PROP_TYPE,&mtype);
    printf("  %s initialized\n",type_name[mtype]);
    

    // open raw files
    infile=fopen(infilename,"rb");
    outfile=fopen(outfilename,"wb");
    if(infile && outfile) {
      printf("    processing ");
      if(mtype==1) {
        int num,ptype,pflags;

        // trigger a note for generators
        bmln_get_machine_info(bm,BM_PROP_NUM_GLOBAL_PARAMS,&num);
        // set value for trigger parameter(s)
        for(i=0;i<num;i++) {
          bmln_get_global_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&pflags);
          if(!(pflags&2)) {
            bmln_get_global_parameter_info(bm,i,BM_PARA_TYPE,(void *)&ptype);
            switch(ptype) {
              case 0: // note
                bmln_set_global_parameter_value(bm,i,32);
                break;
              case 1: // switch
                bmln_set_global_parameter_value(bm,i,1);
                break;
            }
          }
        }
        bmln_get_machine_info(bm,BM_PROP_NUM_TRACK_PARAMS,&num);
        for(i=0;i<num;i++) {
          bmln_get_track_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&pflags);
          if(!(pflags&2)) {
            bmln_get_track_parameter_info(bm,i,BM_PARA_TYPE,(void *)&ptype);
            switch(ptype) {
              case 0: // note
                bmln_set_track_parameter_value(bm,i,0,32);
                break;
              case 1: // switch
                bmln_set_track_parameter_value(bm,i,0,1);
                break;
            }
          }
        }
      }
      while(!feof(infile)) {
        // assumes the first param is of pt_word type 
        //bm_set_global_parameter_value(bm,0,ival);
        //oval=bm_get_global_parameter_value(bm,0);printf("        Value: %d\n",oval);
        //ival+=vs;
        //if(((vs>0) && (ival==1000)) || ((vs<0) && (ival==0))) vs=-vs; 
        
        printf(".");
        // set GlobalVals, TrackVals
        bmln_tick(bm);
        i_size=fread(buffer_w,2,s_size,infile);
        for(i=0;i<i_size;i++) buffer_f[i]=(float)buffer_w[i]/32768.0f;
        bmln_work(bm,buffer_f,i_size,3/*WM_READWRITE*/);
        for(i=0;i<i_size;i++) {
          if(isnan(buffer_f[i])) nan=1;
          if(isinf(buffer_f[i])) inf=1;
          if(fabs(buffer_f[i])>ma) ma=buffer_f[i];
          buffer_w[i]=(short int)(buffer_f[i]*32768.0f);
        }
        fwrite(buffer_w,2,i_size,outfile);
      }
      //printf("\n");
    }
    else puts("    file error!");
    if(infile) fclose(infile);
    if(outfile) fclose(outfile);
    puts("  done");
    if(nan) puts("some values are nan");
    if(inf) puts("some values are inf");
    printf("max value : %f\n",ma);
    bmln_free(bm);
  }
}

int main( int argc, char **argv ) {

  puts("main beg");

  if(bml_setup(0)) {
    char *lib_name;
    int sl;

    bmlw_set_master_info(120,4,44100);
    bmln_set_master_info(120,4,44100);
    puts("  master info initialized");

    if(argc>2) {
      lib_name=argv[1];
      sl=strlen(lib_name);
      if(sl>4) {
        if(!strcmp(&lib_name[sl-4],".dll")) {
#ifdef HAVE_X86
          test_process_w(lib_name,argv[2],argv[3]);
#else
          puts("no dll emulation on non x86 platforms");
#endif  /* HAVE_X86 */
        }
        else {
          test_process_n(lib_name,argv[2],argv[3]);
        }
      }
    }
    else puts("    not enough args!");
    bml_finalize();
  }

  puts("main end");
  return 0;
}
