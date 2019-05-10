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

// Check if pairs are final state equivalent (first is not accepting or second is).
bool BisimulationInclusion::isCoupleFinalStateEquivalent(StateSetCouple &couple)
{
	// If first is empty they are equivalent.
	if(couple.first.empty())
	{
		return true;
	}

	// If second is empty (ant first not) they are not equivalent.
	if(couple.second.empty())
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
	// Return true if first intersections is empty or second is not, false othervise.
	return set_s.empty() || !set_b.empty();
}

// Check if pair is in congruence closure.
bool BisimulationInclusion::isCongruenceClosureMember(StateSetCouple item)
{
	bool changed = true;
	std::vector<bool> used_b(knownPairs.size(), false);

	// Prepare data structure to keep track which pairs were already used for expansion.
	auto self_iter = knownPairs.find(item);
	int ps = 0;
	for(auto set_iter = knownPairs.begin(); set_iter != knownPairs.end(); set_iter++)
	{
		if (self_iter == set_iter)
		{
			used_b[ps] = true;
			break;
		}
		ps++;
	}

	// Compute fix point by expanding second macrostate of the given pair.
	while(changed)
	{
		int i = 0;
		changed = false;
		for(auto set_iter = knownPairs.begin(); set_iter != knownPairs.end(); set_iter++)
		{
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
			// Check if both fixed point of second macrostate contains the first (they are in closure).
			if (isSubset(item.first,  item.second))
				return true;
		}
	}
	// Fixed point not found (not in closure).
	return false;
}

// Inclusion check.
bool BisimulationInclusion::check()
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
