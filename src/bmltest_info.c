/* $Id: bmltest_info.c,v 1.3 2005-05-26 16:25:55 ensonic Exp $
 * invoke it e.g. as
 *   env LD_LIBRARY_PATH="." ./bmltest_info ../machines/elak_svf.dll
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

void test_info(const char *dllpath) {
  // buzz machine handle
  void *bm;
  char *fulldllpath;
  char *str;
  int val,i,num,maval,mival,noval;
 
  fulldllpath=bml_convertpath((char *)dllpath);

  printf("%s(\"%s\" -> \"%s\")\n",__FUNCTION__,dllpath,fulldllpath);
  
  if(bm=bml_new(fulldllpath)) {
    char *machine_types[]={"MT_MASTER","MT_GENERATOR","MT_EFFECT" };
    char *parameter_types[]={"PT_NOTE","PT_SWITCH","PT_BYTE","PT_WORD" };

	puts("  machine created");
	bml_init(bm);
    puts("  machine initialized");

    if(bml_get_machine_info(bm,BM_PROP_TYPE,(void *)&val))                 printf("    Type: %i -> \"%s\"\n",val,((val<3)?machine_types[val]:"unknown"));
    if(bml_get_machine_info(bm,BM_PROP_VERSION,(void *)&val))              printf("    Version: %3.1f\n",(float)val/10.0);
    if(bml_get_machine_info(bm,BM_PROP_FLAGS,(void *)&val)) {              printf("    Flags: 0x%x\n",val);
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
    if(bml_get_machine_info(bm,BM_PROP_MIN_TRACKS,(void *)&val))           printf("    MinTracks: %i\n",val);
    if(bml_get_machine_info(bm,BM_PROP_MAX_TRACKS,(void *)&val))           printf("    MaxTracks: %i\n",val);
    if(bml_get_machine_info(bm,BM_PROP_NUM_GLOBAL_PARAMS,(void *)&val)) {  printf("    NumGlobalParams: %i\n",val);
      num=val;
      for(i=0;i<num;i++) {
        printf("      GlobalParam=%02i\n",i);
        if(bml_get_global_parameter_info(bm,i,BM_PARA_TYPE,(void *)&val))         printf("        Type: %i -> \"%s\"\n",val,((val<4)?parameter_types[val]:"unknown"));
        if(bml_get_global_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bml_get_global_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
        if(bml_get_global_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
					if(val&(1<<0)) puts("          MPF_WAVE");
					if(val&(1<<1)) puts("          MPF_STATE");
					if(val&(1<<2)) puts("          MPF_TICK_ON_EDIT");
          //if(val&) puts("      ");
        }
        if(bml_get_global_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
           bml_get_global_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
           bml_get_global_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
           bml_get_global_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
        val=bml_get_global_parameter_value(bm,i);printf("        RealValue: %d\n",val);
      }
    }
    if(bml_get_machine_info(bm,BM_PROP_NUM_TRACK_PARAMS,(void *)&val)) {   printf("    NumTrackParams: %i\n",val);
      num=val;
      for(i=0;i<num;i++) {
        printf("      TrackParam=%02i\n",i);
        if(bml_get_track_parameter_info(bm,i,BM_PARA_TYPE,(void *)&val))         printf("        Type: %i -> \"%s\"\n",val,((val<4)?parameter_types[val]:"unknown"));
        if(bml_get_track_parameter_info(bm,i,BM_PARA_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bml_get_track_parameter_info(bm,i,BM_PARA_DESCRIPTION,(void *)&str))  printf("        Description: \"%s\"\n",str);
        if(bml_get_track_parameter_info(bm,i,BM_PARA_FLAGS,(void *)&val)) {      printf("        Flags: 0x%x\n",val);
          /*
          //if(val&) puts("      ");
          */
        }
        if(bml_get_track_parameter_info(bm,i,BM_PARA_MIN_VALUE,(void *)&mival) &&
           bml_get_track_parameter_info(bm,i,BM_PARA_MAX_VALUE,(void *)&maval) &&
           bml_get_track_parameter_info(bm,i,BM_PARA_NO_VALUE,(void *)&noval) &&
           bml_get_track_parameter_info(bm,i,BM_PARA_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d [%d]\n",mival,val,maval,noval);
        val=bml_get_track_parameter_value(bm,0,i);printf("        RealValue: %d\n",val);
      }
    }
    if(bml_get_machine_info(bm,BM_PROP_NUM_ATTRIBUTES,(void *)&val)) {     printf("    NumAttributes: %i\n",val);
      num=val;
      for(i=0;i<num;i++) {
        printf("      Attribute=%02i\n",i);
        if(bml_get_attribute_info(bm,i,BM_ATTR_NAME,(void *)&str))         printf("        Name: \"%s\"\n",str);
        if(bml_get_attribute_info(bm,i,BM_ATTR_MIN_VALUE,(void *)&mival) &&
           bml_get_attribute_info(bm,i,BM_ATTR_MAX_VALUE,(void *)&maval) &&
           bml_get_attribute_info(bm,i,BM_ATTR_DEF_VALUE,(void *)&val))    printf("        Value: %d .. %d .. %d\n",mival,val,maval);
        val=bml_get_attribute_value(bm,i);printf("        RealValue: %d\n",val);
      }
    }
    if(bml_get_machine_info(bm,BM_PROP_NAME,(void *)&str))                 printf("    Name: \"%s\"\n",str);
    if(bml_get_machine_info(bm,BM_PROP_SHORT_NAME,(void *)&str))           printf("    ShortName: \"%s\"\n",str);
    if(bml_get_machine_info(bm,BM_PROP_AUTHOR,(void *)&str))               printf("    Author: \"%s\"\n",str);
    if(bml_get_machine_info(bm,BM_PROP_COMMANDS,(void *)&str))             printf("    Commands: \"%s\"\n",str);

    puts("  done");
    bml_free(bm);
  }
}

int main( int argc, char **argv ) {
  int i;
  
  puts("main beg");

  if(bml_setup(0)) {

    bml_set_master_info(120,4,44100);
    puts("  master info initialized");

    for(i=1;i<argc;i++) {
      test_info(argv[i]);
    }
    bml_finalize();
  }

  puts("main end");
  return 0;
}
