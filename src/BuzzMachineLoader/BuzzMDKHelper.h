/* $Id: BuzzMDKHelper.h,v 1.1 2007-10-31 18:02:06 ensonic Exp $
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

#ifndef BUZZ_MDK_HELPER_H
#define BUZZ_MDK_HELPER_H

#include "MachineInterface.h"

#ifdef __cplusplus
extern "C" {
#endif

class BuzzMDKHelper {
public:
	BuzzMDKHelper(void);
	virtual ~BuzzMDKHelper();
	virtual void AddInput(const char *szMachine, bool bStereo);
	virtual void DeleteInput(const char *szMachine);
	virtual void RenameInput(const char *szMachine, const char *szMachineNewName);
	virtual void SetInputChannels(const char *szMachine, bool bStereo);
	virtual void Input(float *psamples, int numsamples, float fAmp);
	virtual bool Work(float *psamples, int numsamples, int wm);
	virtual bool WorkMonoToStereo(float *pin, float *pout, int numsamples, int wm);
	virtual void Init(CMachineDataInput * const pi);
	virtual void Save(CMachineDataOutput * const po);
	virtual void SetOutputMode(bool bStereo);
	virtual void SetMode();
	virtual void MidiControlChange(int const ctrl, int const channel, int const value);

public:
};

#ifdef __cplusplus
}
#endif


#endif // BUZZ_MDK_HELPER_H

