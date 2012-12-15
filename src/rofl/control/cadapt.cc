/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*
 * cadapt.cc
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#include "cadapt.h"

using namespace rofl;

void
cadapt_ctl::bind(
		cadapt_dpt *dpt)
{
	if ((0 == dpt) || (this->dpt == dpt))
	{
		return;
	}

	this->dpt = dpt;
	dpt->bind(this);
	bound(dpt);
};


void
cadapt_ctl::unbind(
		cadapt_dpt *dpt)
{
	if ((0 == dpt) || (this->dpt != dpt))
	{
		return;
	}
	unbound(dpt);
	dpt->unbind(this);
	this->dpt = 0;
}


void
cadapt_dpt::bind(
		cadapt_ctl *ctl)
{
	if ((0 == ctl) || (this->ctl == ctl))
	{
		return;
	}

	this->ctl = ctl;
	ctl->bind(this);
	bound(ctl);
}


void
cadapt_dpt::unbind(
		cadapt_ctl *ctl)
{
	if ((0 == ctl) || (this->ctl != ctl))
	{
		return;
	}
	unbound(ctl);
	ctl->unbind(this);
	this->ctl = 0;
}

