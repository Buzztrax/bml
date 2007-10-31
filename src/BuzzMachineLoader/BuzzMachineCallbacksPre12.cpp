/* $Id: BuzzMachineCallbacksPre12.cpp,v 1.1 2007-10-31 18:02:06 ensonic Exp $
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

#define BUZZ_MACHINE_CALLBACKS_PRE12_CPP
#define BUZZ_MACHINE_LOADER

#ifdef WIN32
#include "stdafx.h"
#include <windows.h>
#else
#include "windows_compat.h"
#endif
#include <stdio.h>
#include "debug.h"

#include "MachineInterface.h"
#include "BuzzMachineCallbacksPre12.h"
#include "BuzzMDKHelper.h"
#include "BuzzMachineLoader.h"
#include "OscTable.h"

CWaveInfo const *BuzzMachineCallbacksPre12::GetWave(int const i) {
	DBG1("(i=%d)\n",i);
    FIXME;
	return(NULL);
}

CWaveLevel const *BuzzMachineCallbacksPre12::GetWaveLevel(int const i, int const level) {
	DBG2("(i=%d,level=%d)\n",i,level);
    FIXME;
	return(NULL);
}

CWaveLevel const *BuzzMachineCallbacksPre12::GetNearestWaveLevel(int const i, int const note) {
	DBG2("(i=%d,note=%d)\n",i,note);

	if((i==-1) && (note==-1)) {
		// the evil MDK hack that Buzz MDK machines rely upon
		if(!mdkHelper) {
			mdkHelper = new BuzzMDKHelper;
		}
		return((CWaveLevel *)mdkHelper);
	}

	return(&defaultWaveLevel);
}

int BuzzMachineCallbacksPre12::GetFreeWave() {
	DBG("()\n");
    FIXME;
	return(0);
}

bool BuzzMachineCallbacksPre12::AllocateWave(int const i, int const size, char const *name) {
	DBG3("(i=%d,size=%d,name=\"%s\")\n",i,size,name);
    FIXME;
	return(FALSE);
}

/* GUI:
 *
 */
void BuzzMachineCallbacksPre12::MessageBox(char const *txt) {
	DBG1("(txt=\"%s\")\n",txt);
	if(txt) puts(txt);
}

/* Misc/Unknown:
 */

void BuzzMachineCallbacksPre12::Lock() {
	DBG("()\n");
    FIXME;
}

void BuzzMachineCallbacksPre12::Unlock() {
	DBG("()\n");
    FIXME;
}

void BuzzMachineCallbacksPre12::ScheduleEvent(int const time, dword const data) {
	DBG2("(time=%d,data=%d)\n",time,data);
    FIXME;
}

void BuzzMachineCallbacksPre12::MidiOut(int const dev, dword const data) {
	DBG2("(dev=%d,data=%d)\n",dev,data);
    FIXME;
}

/* common Oszillators
 */
short const *BuzzMachineCallbacksPre12::GetOscillatorTable(int const waveform) {
	DBG1("(waveform=%d)\n",waveform);
	return OscTable[waveform];
}


/* Application State:
 */
int BuzzMachineCallbacksPre12::GetWritePos() {
	DBG("()\n");
    FIXME;
	return(0);
}

int BuzzMachineCallbacksPre12::GetPlayPos() {
	DBG("()\n");
    FIXME;
	return(0);
}

/* AuxBus:
 *
 */
float *BuzzMachineCallbacksPre12::GetAuxBuffer() {
	DBGO1(machine_info->Name,"()\n=%p\n",auxBuffer);
	return(auxBuffer);
}

void BuzzMachineCallbacksPre12::ClearAuxBuffer() {
	DBGO(machine_info->Name,"()\n");
	for (unsigned int i=0; i<2*BMC_AUXBUFFER_SIZE; i++) {
		auxBuffer[i]=0.0f;
	}
}

/* Envelopes:
 */
int BuzzMachineCallbacksPre12::GetEnvSize(int const wave, int const env) {
	DBG2("(wave=%d,env=%d)\n",wave,env);
    FIXME;
	return(0);
}
bool BuzzMachineCallbacksPre12::GetEnvPoint(int const wave, int const env, int const i, word &x, word &y, int &flags) {
	DBG3("(wave=%d,env=%d,i=%d,&x,&y,&flags)\n",wave,env,i);
    FIXME;
	return(FALSE);
}


/* Pattern editing:
 */
void BuzzMachineCallbacksPre12::SetNumberOfTracks(int const n) {
	DBG1("(n=%d)\n",n);
    FIXME;
}

CPattern *BuzzMachineCallbacksPre12::CreatePattern(char const *name, int const length) {
	DBG2("(name=\"%s\",length=%d)\n",name,length);
    FIXME;
	return(NULL);
}

CPattern *BuzzMachineCallbacksPre12::GetPattern(int const index) {
	DBG1("(index=%d)\n",index);
    FIXME;
	return(NULL);
}

char const *BuzzMachineCallbacksPre12::GetPatternName(CPattern *ppat) {
	DBG1("(ppat=%p)\n",ppat);
    FIXME;
	return(NULL);
}

void BuzzMachineCallbacksPre12::RenamePattern(char const *oldname, char const *newname) {
	DBG2("(oldname=\"%s\",newname=\"%s\")\n",oldname,newname);
    FIXME;
}

void BuzzMachineCallbacksPre12::DeletePattern(CPattern *ppat) {
	DBG1("(ppat=%p)\n",ppat);
    FIXME;
}

int BuzzMachineCallbacksPre12::GetPatternData(CPattern *ppat, int const row, int const group, int const track, int const field) {
	DBG5("(ppat=%p,row=%d,group=%d,track=%d,field=%d)\n",ppat,row,group,track,field);
    FIXME;
	return(NULL);
}

void BuzzMachineCallbacksPre12::SetPatternData(CPattern *ppat, int const row, int const group, int const track, int const field, int const value) {
	DBG6("(ppat=%p,row=%d,group=%d,track=%d,field=%d,value=%d)\n",ppat,row,group,track,field,value);
    FIXME;
}


/* Sequence editing:
 */
CSequence *BuzzMachineCallbacksPre12::CreateSequence() {
	DBG("()\n");
    FIXME;
	return(NULL);
}
void BuzzMachineCallbacksPre12::DeleteSequence(CSequence *pseq) {
	DBG1("(pseq=%p)\n",pseq);
    FIXME;
}

// special ppat values for GetSequenceData and SetSequenceData
// empty = NULL
// <break> = (CPattern *)1
// <mute> = (CPattern *)2
// <thru> = (CPattern *)3
CPattern *BuzzMachineCallbacksPre12::GetSequenceData(int const row) {
	DBG1("(row=%d)\n",row);
    FIXME;
	return(NULL);
}

void BuzzMachineCallbacksPre12::SetSequenceData(int const row, CPattern *ppat) {
	DBG2("(row=%d,ppat=%p)\n",row,ppat);
}
