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

// Check if pairs are final state equivalent (both or neither contain final state).
bool BisimulationEquivalence::isCoupleFinalStateEquivalent(StateSetCouple &couple)
{
	// If only one is nonempty their are not equivalent.
	if(couple.first.empty() != couple.second.empty())
	{
		return false;
	}

	// Calculate intersection with set of final states.
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
	// Return true if intersections are empty or nonempty at the same time, false othervise.
	return set_s.empty() == set_b.empty();
}

// Check if pair is in congruence closure.
bool BisimulationEquivalence::isCongruenceClosureMember(StateSetCouple item)
{
	bool changed = true;
	std::vector<bool> used_s(knownPairs.size(), false);
	std::vector<bool> used_b(knownPairs.size(), false);

	// Prepare data structure to keep track which pairs were already used for expansion.
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

	// Compute fix point by expanding both macrostates of the given pair.
	while(changed)
	{
		int i = 0;
		changed = false;
		for(auto set_iter = knownPairs.begin(); set_iter != knownPairs.end(); set_iter++)
		{
			// Try to expand given macroste with pair from knownPairs.
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
			// Try to expand given macroste with pair from knownPairs.
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
			// Check if both macrostates have the same fixed point (they are in closure).
			if (item.first == item.second)
			{
				return true;
			}
		}
	}
	// Fixed point not found (not in closure).
	return false;
}

// Equivalence check.
bool BisimulationEquivalence::check()
{
	// Calculate macrostate pairs for leaf rules and remove leaf symbols from alphabet (no longer needed).
	getLeafCouples();
	pruneRankedAlphabet();

	// Check macrostate leaf pairs equivalence.
	if(!areLeavesEquivalent())
	{
		return false;
	}

	// Loop over pairs to be processed and calculate their successors.
	while(!todo.empty())
	{
		actual = *todo.begin();			// Select pair to be proccessed.
		todo.erase(actual);				// Remove it from todo.

		if(isCongruenceClosureMember(actual))		// Check if actual is in congruence closure and skip it if it is.
		{
			continue;
		}

		done.insert(actual);						// Add actual to processed pairs.

		for(auto symbol : ranked_alphabet)			// Calculate successors of actual.
		{
			if(!getPost(symbol))					// If non matching pair (counterexample) was generated return false.
				return false;
		}
	}
	return true;									// All pairs were processed and no counterexample found, return true.
}
