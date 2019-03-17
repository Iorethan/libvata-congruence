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

bool BisimulationEquivalence::isCongruenceClosureMember(StateSetCouple item)
{
	// pair_cnt1++;

	bool changed = true;
	std::vector<bool> used_s(knownPairs.size(), false);
	std::vector<bool> used_b(knownPairs.size(), false);

	auto self_iter = knownPairs.find(item);
	int ps = 0;
	for(auto set_iter = knownPairs.begin(); set_iter != knownPairs.end(); set_iter++)
	{
		if (self_iter == set_iter)
		{
			used_s[ps] = true;
			used_b[ps] = true;
			break;
		}
		ps++;
	}

	while(changed)
	{
		int i = 0;
		changed = false;
		for(auto set_iter = knownPairs.begin(); set_iter != knownPairs.end(); set_iter++)
		{
			if(!used_s[i])
			{
				Expandable exp = isExpandableBy(item.first, set_iter);
				if(exp == First || exp == Second)
				{
					changed = true;
					used_s[i] = true;
					for(auto si : (exp == First ? (set_iter->first) : (set_iter->second)))
					{
						item.first.insert(si);
					}
				}
			}
			if(!used_b[i])
			{
				Expandable exp = isExpandableBy(item.second, set_iter);
				if(exp == First || exp == Second)
				{
					changed = true;
					used_b[i] = true;
					for(auto si : (exp == First ? (set_iter->first) : (set_iter->second)))
					{
						item.second.insert(si);
					}
				}
			}
			i++;
			if (item.first == item.second)
				return true;
		}
	}
	return false;
}

bool BisimulationEquivalence::check()
{
	getLeafCouples();
	pruneRankedAlphabet();

	if(!areLeavesEquivalent())
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

		if(isCongruenceClosureMember(actual))
		{
			continue;
		}

		done.insert(actual);

		for(auto symbol : ranked_alphabet)
		{
			if(!getPost(symbol))
				return false;
		}
	}
	return true;
}
