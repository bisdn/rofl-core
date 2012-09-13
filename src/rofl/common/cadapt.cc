/*
 * cadapt.cc
 *
 *  Created on: 30.08.2012
 *      Author: andreas
 */

#include <rofl/common/cadapt.h>


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
};


void
cadapt_ctl::unbind(
		cadapt_dpt *dpt)
{
	if ((0 == dpt) || (this->dpt != dpt))
	{
		return;
	}
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
}


void
cadapt_dpt::unbind(
		cadapt_ctl *ctl)
{
	if ((0 == ctl) || (this->ctl != ctl))
	{
		return;
	}
	ctl->unbind(this);
	this->ctl = 0;
}

