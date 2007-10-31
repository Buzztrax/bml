/* $Id: BuzzMDKHelper.cpp,v 1.1 2007-10-31 18:02:06 ensonic Exp $
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

/* This file implements the bizarre object used by all Buzz machines that
   are statically linked to MDK.LIB.  This is necessary because, as part
   of its initialisation process, MDK.LIB makes a call to buzz with
   GetNearestWaveLevel(-1,-1), and it expects the return to be a pointer
   to some kind of 'helper' class that handles the core MDK functionality.

   The implementation below **SEEMS TO** work.  Note that definition of
   CBuzzMDKHelper, CBuzzMDKHelperInterface and CBuzzMDKHelperInterfaceEx
   *MUST NOT UNDER ANY CIRCUMSTANCES* change, because the Buzz plugins that
   make use of MDK.LIB will already assume binary compatibility with whatever
   GetNearestWaveLevel(-1,-1) has returned to it.

   Any problems with MDK-built buzz machines, this file is the most likely
   cause

*/


#include "MachineInterface.h"
#include "BuzzMDKHelper.h"

BuzzMDKHelper::BuzzMDKHelper(void) {
}

BuzzMDKHelper::~BuzzMDKHelper() {
	//
}

void BuzzMDKHelper::AddInput(char const *szMachine, bool bStereo) {
	printf("BuzzMDKHelper::%s(szMachine=%s,bStereo=%d)\n",__FUNCTION__,szMachine,bStereo);fflush(stdout);
/*
	if (szMachine != NULL)
	{
		Inputs.push_back(CMachineInputInfo(szMachine, bStereo));
		SetMode();
	}
	InputIterator = Inputs.begin();
*/
}

void BuzzMDKHelper::DeleteInput(char const *szMachine) {
	printf("BuzzMDKHelper::%s(szMachine=%s)\n",__FUNCTION__,szMachine);fflush(stdout);
/*
	for (INPUTITERATOR i = Inputs.begin(); i != Inputs.end(); ++i)
	{
		if (i->strName.compare(szMachine) == 0)
		{
			Inputs.erase(i);
			SetMode();
			break;
		}
	}
	InputIterator = Inputs.begin();
*/
}

void BuzzMDKHelper::RenameInput(char const *szMachine, char const *szMachineNewName) {
	printf("BuzzMDKHelper::%s(szMachine=%s,szMachineNewName=%s)\n",__FUNCTION__,szMachine,szMachineNewName);fflush(stdout);
/*
	for (INPUTITERATOR i = Inputs.begin(); i != Inputs.end(); ++i)
	{
		if (i->strName.compare(szMachine) == 0)
		{
			i->strName = szMachineNewName;
			break;
		}
	}
*/
}

void BuzzMDKHelper::SetInputChannels(char const *szMachine, bool bStereo) {
	printf("BuzzMDKHelper::%s(szMachine=%s,bStereo=%d)\n",__FUNCTION__,szMachine,bStereo);fflush(stdout);
/*
	CMachine * pMachine = pmi->pCB->GetMachine(szMachine);
	if (!pMachine)
	{
		// machine doesn't exist!
		return;
	}

//	if (!(pMachine->cMachineInfo->Flags & MIF_MONO_TO_STEREO) )
//	{
//		// machine can't do stereo so leave this input channel as mono
//		bStereo = false;
//	}
//	else
	{
		for (INPUTITERATOR i = Inputs.begin(); i != Inputs.end(); ++i)
		{
			if (i->strName.compare(szMachine) == 0)
			{
				i->bStereo = bStereo;
				break;
			}
		}
	}

	// change from mono output to stereo output if this input is itself stereo
	// and the machine (up until now) would've been mono-output
	if (MachineWantsChannels==1)
	{
		SetOutputMode(bStereo);
	}
*/
}

void BuzzMDKHelper::Input(float *psamples, int numsamples, float fAmp) {
	printf("BuzzMDKHelper::%s(psamples=%p,numsamples=%d,fAmp=%f)\n",__FUNCTION__,psamples,numsamples,fAmp);fflush(stdout);
/*
	if (InputIterator==NULL)
	{
		return;
	}

	const double dfAmp = static_cast<double>(fAmp);
	
	if (psamples != NULL)
	{
		if (numChannels == 1)
		{
			if (HaveInput == 0)
			{
				if (InputIterator->bStereo)
				{
					CopyStereoToMono(Buffer, psamples, numsamples, dfAmp);
				}
				else
				{
					Copy(Buffer, psamples, numsamples, dfAmp);
				}
			}
			else
			{
				if (InputIterator->bStereo)
				{
					AddStereoToMono(Buffer, psamples, numsamples, dfAmp);
				}
				else
				{
					Add(Buffer, psamples, numsamples, dfAmp);
				}
			}
		}
		else
		{
			if (HaveInput == 0)
			{
				if (InputIterator->bStereo)
				{
					Copy(Buffer, psamples, numsamples*2, dfAmp);
				}
				else
				{
					CopyMonoToStereo(Buffer, psamples, numsamples, dfAmp);
				}
			}
			else
			{
				if (InputIterator->bStereo) 
				{
					Add(Buffer, psamples, numsamples*2, dfAmp);
				}
				else
				{
					AddMonoToStereo(Buffer, psamples, numsamples, dfAmp);
				}
			}
		}

		++HaveInput;
	}

	++InputIterator;
*/
}

bool BuzzMDKHelper::Work(float *psamples, int numsamples, int const wm) {
	printf("BuzzMDKHelper::%s(psamples=%p,numsamples=%d,wm=%d)\n",__FUNCTION__,psamples,numsamples,wm);fflush(stdout);
/*
	if ((wm & WM_READ) && HaveInput)
	{
		Copy(psamples, Buffer, numsamples);
	}
	else
	{
		// shouldn't need to do this, but .. some Buzz machines are crap and need it
		Zero(psamples, numsamples);
	}

	bool bOutputValid = pmi->MDKWork(psamples, numsamples, wm);

	InputIterator = Inputs.begin();
	HaveInput = 0;

	return bOutputValid;
*/
	return(true);
}

bool BuzzMDKHelper::WorkMonoToStereo(float *pin, float *pout, int numsamples, int const wm) {
	printf("BuzzMDKHelper::%s(pin=%p,pout=%p,numsamples=%d,wm=%d)\n",__FUNCTION__,pin,pout,numsamples,wm);fflush(stdout);
/*
	if ((wm & WM_READ) && HaveInput)
	{
		Copy(pout, Buffer, 2*numsamples);
	}
	else
	{
		// shouldn't need to do this, but .. some Buzz machines are crap and need it
		Zero(pout, 2*numsamples);
	}

	bool bOutputValid = pmi->MDKWorkStereo(pout, numsamples, wm);

	InputIterator = Inputs.begin();
	HaveInput = 0;
	return bOutputValid;
*/
	return(true);
}

void BuzzMDKHelper::Init(CMachineDataInput * const pi) {
	printf("BuzzMDKHelper::%s(pi=%p)\n",__FUNCTION__,pi);fflush(stdout);
/*
	ThisMachine = pmi->pCB->GetThisMachine();
	
	numChannels = 1;

	Inputs.clear();
	InputIterator = Inputs.begin();
	HaveInput = 0;
	MachineWantsChannels = 1;

	// Buzz seems to store a dummy initial byte here - maybe
	// some kind of version tag?
	byte byDummy;
	pi->Read(byDummy);

	pmi->MDKInit(pi);
	pInnerEx = pmi->GetEx();
*/
}

void BuzzMDKHelper::Save(CMachineDataOutput * const po) {
	printf("BuzzMDKHelper::%s(po=%p)\n",__FUNCTION__,po);fflush(stdout);
/*
	pmi->MDKSave(po);
*/
}

void BuzzMDKHelper::SetOutputMode(bool bStereo) {
	printf("BuzzMDKHelper::%s(bStereo=%d)\n",__FUNCTION__,bStereo);fflush(stdout);
/*
//	if ( !(ThisMachine->cMachineInfo->Flags & MIF_MONO_TO_STEREO) )
//	{
//		// even MDK machines need to specify MIF_MONO_TO_STEREO if they 1are stereo!
//		// Otherwise SetOutputMode(true) will be ignored by Buzz
//		bStereo = false;
//		return;
//	}
	numChannels = bStereo ? 2 : 1;
	MachineWantsChannels = numChannels;

	pmi->pCB->SetnumOutputChannels(ThisMachine, numChannels);
	pmi->OutputModeChanged(bStereo);
*/
}

void BuzzMDKHelper::SetMode() {
	printf("BuzzMDKHelper::%s()\n",__FUNCTION__);fflush(stdout);
/*
	InputIterator = Inputs.begin();
	HaveInput = 0;
	
	if (MachineWantsChannels > 1)
	{
		numChannels = MachineWantsChannels;
		pmi->pCB->SetnumOutputChannels(ThisMachine, numChannels);
		pmi->OutputModeChanged(numChannels > 1 ? true : false);
		return;
	}


	for (INPUTITERATOR i = Inputs.begin(); i != Inputs.end(); ++i)
	{
		if (i->bStereo)
		{
			numChannels = 2;
			pmi->pCB->SetnumOutputChannels(ThisMachine, numChannels);
			pmi->OutputModeChanged(numChannels > 1 ? true : false);
			return;
		}
	}

	numChannels = 1;
	pmi->pCB->SetnumOutputChannels(ThisMachine, numChannels);
	pmi->OutputModeChanged(numChannels > 1 ? true : false);
*/
}

void BuzzMDKHelper::MidiControlChange(const int ctrl, const int channel, const int value ) {
	printf("BuzzMDKHelper::%s(ctrl=%d,channel=%d,value=%d)\n",__FUNCTION__,ctrl,channel,value);fflush(stdout);
/*
	if( pInnerEx != NULL )
		pInnerEx->MidiControlChange( ctrl, channel, value );
*/
}
