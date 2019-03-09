/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Equivalence using upward bisimulation up to congruence for explicitly
 *    represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_bisimulation_equiv.hh"

unsigned long pair_cnt1, true_cnt1;

using namespace VATA;
using namespace ExplicitTreeUpwardBisimulation;

BisimulationEquivalence::BisimulationEquivalence(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger)
	: BisimulationBase(smaller, bigger)
	{
	}

bool BisimulationEquivalence::isCoupleFinalStateEquivalent(StateSetCouple &couple)
{
	if(couple.first.empty() != couple.second.empty())
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
	return set_s.empty() == set_b.empty();
}

bool BisimulationEquivalence::check()
{
	StateSetCoupleSet done, todo, knownPairs;
	StateSetCouple actual;
	getLeafCouples(todo);
	pruneRankedAlphabet();

	if(!areLeavesEquivalent(todo))
	{
		return false;
	}

	int i = 0;
	while(!todo.empty())
	{
		i++;
		// pair_cnt1++;
		actual = *todo.begin();
		todo.erase(actual);
		knownPairs.insert(actual);

		if(isCongruenceClosureMember(actual, done))
		{
			done.insert(actual);
			continue;
		}
		done.insert(actual);

		for(auto symbol : rankedAlphabet)
		{
			getPost(symbol, actual, knownPairs);
			for(auto next : post)
			{
				if(knownPairs.find(next) == knownPairs.end())
				{
					if(!isCoupleFinalStateEquivalent(next))
					{
						return false;
					}
					todo.insert(next);
				}
			}
		}
	}
	return true;
}
