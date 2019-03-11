/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Inclusion using upward bisimulation up to congruence for explicitly
 *    represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_bisimulation_incl.hh"

using namespace ExplicitTreeUpwardBisimulation;

BisimulationInclusion::BisimulationInclusion(
	const ExplicitTreeAutCore&        _smaller,
	const ExplicitTreeAutCore&        _bigger)
	: BisimulationBase(_smaller, _bigger)
	{
	}

bool BisimulationInclusion::isCoupleFinalStateEquivalent(StateSetCouple &couple)
{
	if(couple.first.empty())
	{
		return true;
	}

	if(couple.second.empty())
	{
		return false;
	}

	StateSet set_s, set_b;
	set_intersection
	(
		couple.first.begin(), couple.first.end(),
		smaller.GetFinalStates().begin(), smaller.GetFinalStates().end(),
		std::inserter(set_s, set_s.begin())
	);
	
	set_intersection
	(
		couple.second.begin(), couple.second.end(),
		bigger.GetFinalStates().begin(), bigger.GetFinalStates().end(),
		std::inserter(set_b, set_b.begin())
	);
	return set_s.empty() || !set_b.empty();
}

bool BisimulationInclusion::check()
{
	return true;
}
