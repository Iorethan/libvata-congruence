/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2018  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Bisimulation base for explicitly represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_bisimulation_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardBisimulation;

BisimulationBase::BisimulationBase(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger) 
	: smaller(smaller), bigger(bigger), stateCounter(0)
{}

//-----------------------------------------------------------------------------

void BisimulationBase::setSmaller(
	ExplicitTreeAutCore smaller)
{
	this->smaller = smaller;
}

//-----------------------------------------------------------------------------

void BisimulationBase::setBigger(
	ExplicitTreeAutCore bigger)
{
	this->bigger = bigger;
}

//-----------------------------------------------------------------------------

ExplicitTreeAutCore BisimulationBase::getSmaller()
{
	return this->smaller;
}

//-----------------------------------------------------------------------------

ExplicitTreeAutCore BisimulationBase::getBigger()
{
	return this->bigger;
}

bool BisimulationBase::check(
		const bool					useCache,
		const bool					useCongruence)
{
	assert(false);			// caling purely virtual method
}
