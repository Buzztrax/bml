/* $Id$
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
 * display machine info block
 *
 * invoke it e.g. as
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_info ../machines/elak_svf.dll
 *   LD_LIBRARY_PATH=".:./BuzzMachineLoader/.libs" ./bmltest_info ../../buzzmachines/Elak/SVF/liblibelak_svf.so
 *
 * the dll/so *must* be a buzz-machine, not much error checking ;-)
 */

#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "bml.h"

#ifdef HAVE_X86
void test_info_w(char *libpath) {
  // buzz machine handle
  void *bm;
  char *str;
  int type,val,i,num,tracks;
  int maval,mival,noval,ptrval=0;
 
  printf("%s(\"%s\")\n",__FUNCTION__,libpath);
  
  if((bm=bmlw_new(libpath))) {
    char *machine_types[]={"MT_MASTER","MT_GENERATOR","MT_EFFECT" };
    char *parameter_types[]={"PT_NOTE","PT_SWITCH","PT_BYTE","PT_WORD" };
    void *addr;

    puts("  windows machine created");
    bmlw_init(bm,0,NULL);
    puts("  windows machine initialized");

    if(bmlw_get_machine_info(bm,BM_PROP_SHORT_NAME,(void *)&str))           printf("    Short Name: \"%s\"\n",str);
    if(bmlw_get_machine_info(bm,BM_PROP_NAME,(void *)&str))                 printf("    Name: \"%s\"\n",str);
    if(bmlw_get_machine_info(bm,BM_PROP_AUTHOR,(void *)&str))               printf("    Author: \"%s\"\n",str);
    if(bmlw_get_machine_info(bm,BM_PROP_COMMANDS,(void *)&str))             printf("    Commands: \"%s\"\n",str);
    if(bmlw_get_machine_info(bm,BM_PROP_TYPE,(void *)&val))                 printf("    Type: %i -> \"%s\"\n",val,((val<3)?machine_types[val]:"unknown"));
    if(bmlw_get_machine_info(bm,BM_PROP_VERSION,(void *)&val))              printf("    Version: %3.1f\n",(float)val/10.0);
    if(bmlw_get_machine_info(bm,BM_PROP_FLAGS,(void *)&val)) {              printf("    Flags: 0x%x\n",val);
      if(val&(1<<0)) puts("      MIF_MONO_TO_STEREO");
      if(val&(1<<1)) puts("      MIF_PLAYS_WAVES");
      if(val&(1<<2)) puts("      MIF_USES_LIB_INTERFACE");
      if(val&(1<<3)) puts("      MIF_USES_INSTRUMENTS");
      if(val&(1<<4)) puts("      MIF_DOES_INPUT_MIXING");
      if(val&(1<<5)) puts("      MIF_NO_OUTPUT");
      if(val&(1<<6)) puts("      MIF_CONTROL_MACHINE");
      if(val&(1<<7)) puts("      MIF_INTERNAL_AUX");
      //if(val&) puts("      ");
    }
    if(bmlw_get_machine_info(bm,BM_PROP_MIN_TRACKS,(void *)&val))           printf("    MinTracks: %i\n",val);
    tracks=val;
    if(bmlw_get_machine_info(bm,BM_PROP_MAX_TRACKS,(void *)&val))           printf("    MaxTracks: %i\n",val);
    if(bmlw_get_machine_info(bm,BM_PROP_NUM_INPUT_CHANNELS,(void *)&val))   printf("    InputChannels: %d\n",val);
    if(bmlw_get_machine_info(bm,BM_PROP_NUM_OUTPUT_CHANNELS,(void *)&val))  printf("    OutputChannels: %d\n",val);
    fflush(stdout);
    if(bmlw_get_machine_info(bm,BM_PROP_NUM_GLOBAL_PARAMS,(void *)&val)) {  printf("    NumGlobalParams: %i\n",val);fflush(stdout);
      num=val;
      for(i=0;i<num;i++) {
        printf("      GlobalParam=%02i\n",i);
        if(bmlw_get_global_parameter_info(bm,i,BM_PARA_TYPE,(void *)&type))        printf("        Type: %i -> \"%s\"\n",type,((type<4)?parameter_types[type]:"unknown"));
        if(bmlw_get_global_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bmlw_get_global_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
        if(bmlw_get_global_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
          if(val&(1<<0)) puts("          MPF_WAVE");
          if(val&(1<<1)) puts("          MPF_STATE");
          if(val&(1<<2)) puts("          MPF_TICK_ON_EDIT");
          //if(val&) puts("      ");
        }
        if(bmlw_get_global_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
           bmlw_get_global_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
           bmlw_get_global_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
           bmlw_get_global_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
        val=bmlw_get_global_parameter_value(bm,i);
		addr=bmlw_get_global_parameter_location(bm,i);
	    str=(char *)bmlw_describe_global_value(bm,i,val);
		switch(type) {
		  case 0: //PT_NOTE:
		  case 1: //PT_SWITCH:
		  case 2: //PT_BYTE:
			ptrval=(int)(*(char *)addr);
			break;
		  case 3: //PT_WORD:
			ptrval=(int)(*(short *)addr);
			break;
		}
	    printf("        RealValue: %d %s (%p -> %d)\n",val,str,addr,ptrval);
      }
    }
    fflush(stdout);
    if(bmlw_get_machine_info(bm,BM_PROP_NUM_TRACK_PARAMS,(void *)&val)) {   printf("    NumTrackParams: %i\n",val);fflush(stdout);
      num=val;
      if(tracks) {
        for(i=0;i<num;i++) {
          printf("      TrackParam=%02i\n",i);
          if(bmlw_get_track_parameter_info(bm,i,BM_PARA_TYPE,(void *)&type))        printf("        Type: %i -> \"%s\"\n",type,((type<4)?parameter_types[type]:"unknown"));
          if(bmlw_get_track_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
          if(bmlw_get_track_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
          if(bmlw_get_track_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
            if(val&(1<<0)) puts("          MPF_WAVE");
            if(val&(1<<1)) puts("          MPF_STATE");
            if(val&(1<<2)) puts("          MPF_TICK_ON_EDIT");
            //if(val&) puts("      ");
          }
          if(bmlw_get_track_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
             bmlw_get_track_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
             bmlw_get_track_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
             bmlw_get_track_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
          val=bmlw_get_track_parameter_value(bm,0,i);
          addr=bmlw_get_track_parameter_location(bm,0,i);
          str=(char *)bmlw_describe_track_value(bm,i,val);
          switch(type) {
            case 0: //PT_NOTE:
            case 1: //PT_SWITCH:
            case 2: //PT_BYTE:
              ptrval=(int)(*(char *)addr);
              break;
            case 3: //PT_WORD:
              ptrval=(int)(*(short *)addr);
              break;
          }
          printf("        RealValue: %d %s (%p -> %d)\n",val,str,addr,ptrval);
        }
      }
      else {
        printf("      WARNING but tracks=0..0\n");fflush(stdout);
      }        
    }
    fflush(stdout);
    if(bmlw_get_machine_info(bm,BM_PROP_NUM_ATTRIBUTES,(void *)&val)) {     printf("    NumAttributes: %i\n",val);fflush(stdout);
      num=val;
      for(i=0;i<num;i++) {
        printf("      Attribute=%02i\n",i);
        if(bmlw_get_attribute_info(bm,i,BM_ATTR_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bmlw_get_attribute_info(bm,i,BM_ATTR_MIN_VALUE,(void *)&mival) &&
           bmlw_get_attribute_info(bm,i,BM_ATTR_MAX_VALUE,(void *)&maval) &&
           bmlw_get_attribute_info(bm,i,BM_ATTR_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d\n",mival,val,maval);
        val=bmlw_get_attribute_value(bm,i);printf("        RealValue: %d\n",val);
      }
    }

    puts("  done");
    bmlw_free(bm);
  }
}
#endif  /* HAVE_X86 */

void test_info_n(char *libpath) {
  // buzz machine handle
  void *bm;
  char *str;
  int type,val,i,num,tracks;
  int maval,mival,noval,ptrval=0;
 
  printf("%s(\"%s\")\n",__FUNCTION__,libpath);
  
  if((bm=bmln_new(libpath))) {
    char *machine_types[]={"MT_MASTER","MT_GENERATOR","MT_EFFECT" };
    char *parameter_types[]={"PT_NOTE","PT_SWITCH","PT_BYTE","PT_WORD" };
    void *addr;

    puts("  native machine created");
    bmln_init(bm,0,NULL);
    puts("  native machine initialized");

    if(bmln_get_machine_info(bm,BM_PROP_SHORT_NAME,(void *)&str))           printf("    Short Name: \"%s\"\n",str);
    if(bmln_get_machine_info(bm,BM_PROP_NAME,(void *)&str))                 printf("    Name: \"%s\"\n",str);
    if(bmln_get_machine_info(bm,BM_PROP_AUTHOR,(void *)&str))               printf("    Author: \"%s\"\n",str);
    if(bmln_get_machine_info(bm,BM_PROP_COMMANDS,(void *)&str))             printf("    Commands: \"%s\"\n",str);
    if(bmln_get_machine_info(bm,BM_PROP_TYPE,(void *)&val))                 printf("    Type: %i -> \"%s\"\n",val,((val<3)?machine_types[val]:"unknown"));
    if(bmln_get_machine_info(bm,BM_PROP_VERSION,(void *)&val))              printf("    Version: %3.1f\n",(float)val/10.0);
    if(bmln_get_machine_info(bm,BM_PROP_FLAGS,(void *)&val)) {              printf("    Flags: 0x%x\n",val);
      if(val&(1<<0)) puts("      MIF_MONO_TO_STEREO");
      if(val&(1<<1)) puts("      MIF_PLAYS_WAVES");
      if(val&(1<<2)) puts("      MIF_USES_LIB_INTERFACE");
      if(val&(1<<3)) puts("      MIF_USES_INSTRUMENTS");
      if(val&(1<<4)) puts("      MIF_DOES_INPUT_MIXING");
      if(val&(1<<5)) puts("      MIF_NO_OUTPUT");
      if(val&(1<<6)) puts("      MIF_CONTROL_MACHINE");
      if(val&(1<<7)) puts("      MIF_INTERNAL_AUX");
      //if(val&) puts("      ");
    }
    if(bmln_get_machine_info(bm,BM_PROP_MIN_TRACKS,(void *)&val))           printf("    MinTracks: %i\n",val);
    tracks=val;
    if(bmln_get_machine_info(bm,BM_PROP_MAX_TRACKS,(void *)&val))           printf("    MaxTracks: %i\n",val);
    if(bmln_get_machine_info(bm,BM_PROP_NUM_INPUT_CHANNELS,(void *)&val))   printf("    InputChannels: %d\n",val);
    if(bmln_get_machine_info(bm,BM_PROP_NUM_OUTPUT_CHANNELS,(void *)&val))  printf("    OutputChannels: %d\n",val);
    fflush(stdout);   
    if(bmln_get_machine_info(bm,BM_PROP_NUM_GLOBAL_PARAMS,(void *)&val)) {  printf("    NumGlobalParams: %i\n",val);fflush(stdout);
      num=val;
      for(i=0;i<num;i++) {
        printf("      GlobalParam=%02i\n",i);
        if(bmln_get_global_parameter_info(bm,i,BM_PARA_TYPE,(void *)&type))        printf("        Type: %i -> \"%s\"\n",type,((type<4)?parameter_types[type]:"unknown"));
        if(bmln_get_global_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bmln_get_global_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
        if(bmln_get_global_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
          if(val&(1<<0)) puts("          MPF_WAVE");
          if(val&(1<<1)) puts("          MPF_STATE");
          if(val&(1<<2)) puts("          MPF_TICK_ON_EDIT");
          //if(val&) puts("      ");
        }
        if(bmln_get_global_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
           bmln_get_global_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
           bmln_get_global_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
           bmln_get_global_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
        val=bmln_get_global_parameter_value(bm,i);
		addr=bmln_get_global_parameter_location(bm,i);
	    str=(char *)bmln_describe_global_value(bm,i,val);
		switch(type) {
		  case 0: //PT_NOTE:
		  case 1: //PT_SWITCH:
		  case 2: //PT_BYTE:
			ptrval=(int)(*(char *)addr);
			break;
		  case 3: //PT_WORD:
			ptrval=(int)(*(short *)addr);
			break;
		}
	    printf("        RealValue: %d %s (%p -> %d)\n",val,str,addr,ptrval);
      }
    }
    fflush(stdout);
    if(bmln_get_machine_info(bm,BM_PROP_NUM_TRACK_PARAMS,(void *)&val)) {   printf("    NumTrackParams: %i\n",val);fflush(stdout);
      num=val;
      if(tracks) {
        for(i=0;i<num;i++) {
          printf("      TrackParam=%02i\n",i);
          if(bmln_get_track_parameter_info(bm,i,BM_PARA_TYPE,(void *)&type))        printf("        Type: %i -> \"%s\"\n",type,((type<4)?parameter_types[type]:"unknown"));
          if(bmln_get_track_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
          if(bmln_get_track_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
          if(bmln_get_track_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
            if(val&(1<<0)) puts("          MPF_WAVE");
            if(val&(1<<1)) puts("          MPF_STATE");
            if(val&(1<<2)) puts("          MPF_TICK_ON_EDIT");
            //if(val&) puts("      ");
          }
          if(bmln_get_track_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
             bmln_get_track_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
             bmln_get_track_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
             bmln_get_track_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
          val=bmln_get_track_parameter_value(bm,0,i);
          addr=bmln_get_track_parameter_location(bm,0,i);
          str=(char *)bmln_describe_track_value(bm,i,val);
          switch(type) {
            case 0: //PT_NOTE:
            case 1: //PT_SWITCH:
            case 2: //PT_BYTE:
              ptrval=(int)(*(char *)addr);
              break;
            case 3: //PT_WORD:
              ptrval=(int)(*(short *)addr);
              break;
          }
          printf("        RealValue: %d %s (%p -> %d)\n",val,str,addr,ptrval);
        }
      }
      else {
        printf("      WARNING but tracks=0..0\n");fflush(stdout);
      }        
    }
    fflush(stdout);
    if(bmln_get_machine_info(bm,BM_PROP_NUM_ATTRIBUTES,(void *)&val)) {     printf("    NumAttributes: %i\n",val);fflush(stdout);
      num=val;
      for(i=0;i<num;i++) {
        printf("      Attribute=%02i\n",i);
        if(bmln_get_attribute_info(bm,i,BM_ATTR_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bmln_get_attribute_info(bm,i,BM_ATTR_MIN_VALUE,(void *)&mival) &&
           bmln_get_attribute_info(bm,i,BM_ATTR_MAX_VALUE,(void *)&maval) &&
           bmln_get_attribute_info(bm,i,BM_ATTR_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d\n",mival,val,maval);
        val=bmln_get_attribute_value(bm,i);printf("        RealValue: %d\n",val);
      }
    }

    puts("  done");
    bmln_free(bm);
  }
}

int main( int argc, char **argv ) {
  setlinebuf(stdout);
  setlinebuf(stderr); 
  puts("main beg");

  if(bml_setup(0)) {
    char *lib_name;
    int sl;
    int i;

#ifdef HAVE_X86
    bmlw_set_master_info(120,4,44100);
#endif
    bmln_set_master_info(120,4,44100);
    puts("  master info initialized");

    for(i=1;i<argc;i++) {
      lib_name=argv[i];
      sl=strlen(lib_name);
      if(sl>4) {
        if(!strcmp(&lib_name[sl-4],".dll")) {
#ifdef HAVE_X86
          test_info_w(lib_name);
#else
          puts("no dll emulation on non x86 platforms");
#endif  /* HAVE_X86 */
        }
        else {
          test_info_n(lib_name);
        }
      }
    }
    bml_finalize();
  }

  puts("main end");
  return 0;
}

