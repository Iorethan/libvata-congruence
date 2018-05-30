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

#include <iostream>
#include "explicit_tree_bisimulation_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardBisimulation;

BisimulationEquivalence::BisimulationEquivalence(
	const ExplicitTreeAutCore&        _smaller,
	const ExplicitTreeAutCore&        _bigger)
	: BisimulationBase(_smaller, _bigger)
	{
	}


bool BisimulationEquivalence::areLeavesEquivalent(StateSetCoupleSet &todo)
{
	for(auto couple : todo)
	{
		if(!isCoupleFinalStateEquivalent(couple))
		{
			return false;
		}
	}
	return true;
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

StateSetCouple BisimulationEquivalence::selectActual(StateSetCoupleSet& todo)
{
	return *(todo.begin());
}

bool BisimulationEquivalence::isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set)
{
	if(isMember(item, set))
	{
		return true;
	}

	StateSetCouple aux;
	bool changed = true;
	std::vector<bool> used_s(set.size(), false);
	std::vector<bool> used_b(set.size(), false);
	while(changed)
	{
		int i = 0;
		changed = false;
		for(auto set_item : set)
		{
			if(!used_s[i] && isExpandableBy(item.first, aux.second, set_item))
			{
				changed = true;
				used_s[i] = true;
				item.first = set_union(item.first, set_item.first);
				aux.second = set_union(aux.second, set_item.second);
			}
			if(!used_b[i] && isExpandableBy(aux.first, item.second, set_item))
			{
				changed = true;
				used_b[i] = true;
				aux.first = set_union(aux.first, set_item.first);
				item.second = set_union(item.second, set_item.second);
			}
			i++;
		}
	}
	return item.first == aux.first && item.second == aux.second;
}

bool BisimulationEquivalence::isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item)
{
	return intersection(first, item.first).size() != 0 ||
		intersection(second, item.second).size() != 0;
}

bool BisimulationEquivalence::check()
{
	StateSetCoupleSet done, todo, knownPairs;
	StateSetCouple actual;
	getLeafCouples(knownPairs);
	pruneRankedAlphabet();
	todo = knownPairs;

	if(!areLeavesEquivalent(todo))
	{
		return false;
	}

	while(!todo.empty())
	{
		actual = selectActual(todo);

		todo.erase(actual);
		done.insert(actual);
		knownPairs.insert(actual);

		for(auto symbol : rankedAlphabet)
		{
			getPost(symbol, actual, done);
			for(auto next : post)
			{
				if(!isCoupleFinalStateEquivalent(next))
				{
					return false;
				}

				if(!isCongruenceClosureMember(next, knownPairs))
				{
					todo.insert(next);
				}
			}
		}
	}
	return true;
}
