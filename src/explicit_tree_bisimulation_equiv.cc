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
	: BisimulationBase(_smaller, _bigger), expandable_cache()
	{
		smaller = smaller.RemoveUnreachableStates();
		smaller = smaller.RemoveUselessStates();
		bigger = bigger.RemoveUnreachableStates();
		bigger = bigger.RemoveUselessStates();
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

bool BisimulationEquivalence::isCongruenceClosureMemberCachedStrict(StateSetCouple item, StateSetCoupleSet &set)
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
			if(!used_s[i] && isExpandableByCached(item.first, aux.second, set_item))
			{
				changed = true;
				used_s[i] = true;
				item.first = set_union(item.first, set_item.first);
				aux.second = set_union(aux.second, set_item.second);
			}
			if(!used_b[i] && isExpandableByCached(aux.first, item.second, set_item))
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

bool BisimulationEquivalence::isCongruenceClosureMemberCachedLax(StateSetCouple item, StateSetCoupleSet &set)
{
	if(isMember(item, set))
	{
		return true;
	}

	StateSetCouple aux;
	bool changed = true;
	std::vector<bool> used_s(set.size(), false);
	while(changed)
	{
		unsigned i = 0;
		changed = false;
		for(auto set_item : set)
		{
			if(!used_s[i] && isExpandableByCached(item.first, item.second, set_item))
			{
				changed = true;
				used_s[i] = true;
				item.first = set_union(item.first, set_item.first);
				item.second = set_union(item.second, set_item.second);
			}
			i++;
		}
	}
	return item.first == item.second;
}

bool BisimulationEquivalence::isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item)
{
	return intersection(first, item.first).size() != 0 ||
		intersection(second, item.second).size() != 0;
}

bool BisimulationEquivalence::isExpandableByCached(StateSet &first, StateSet &second, StateSetCouple &item)
{
	std::map<std::tuple<StateSet, StateSet, StateSetCouple>, bool>::iterator iter;
	if((iter = expandable_cache.find(std::make_tuple(first, second, item))) != expandable_cache.end())
	{
		return iter->second;
	}
	else
	{
		std::tuple<StateSet, StateSet, StateSetCouple> tuple(first, second, item);
		bool result = intersection(first, item.first).size() != 0 || intersection(second, item.second).size() != 0;
		expandable_cache.insert(std::pair<std::tuple<StateSet, StateSet, StateSetCouple>, bool>(tuple, result));
		return result;
	}
}

// bool BisimulationEquivalence::check(const bool useCache, const bool	useCongruence, const bool beLax)
// {
// 	RankedAlphabet rankedAlphabet = getRankedAlphabet();
// 	StateSetCoupleSet all, done, todo, knownPairs;
// 	StateSetCouple actual;
// 	getLeafCouples(rankedAlphabet, all);
// 	pruneRankedAlphabet(rankedAlphabet);
// 	todo = all;
// 	knownPairs = all;
	
// 	if(!areLeavesEquivalent(todo))
// 	{
// 		return false;
// 	}

// 	while(!todo.empty())
// 	{
// 		actual = selectActual(todo);

// 		todo.erase(actual);
// 		done.insert(actual);
// 		knownPairs.insert(actual);

// 		for(auto symbol : rankedAlphabet)
// 		{
// 			getPostCached(symbol, actual, done);
// 			for(auto next : post)
// 			{
// 				if(!isCoupleFinalStateEquivalent(next))
// 				{
// 					return false;
// 				}

// 				if(!isCongruenceClosureMemberCachedStrict(next, knownPairs))
// 				{
// 					todo.insert(next);
// 				}
// 			}
// 		}
// 	}
// 	return true;
// }

bool BisimulationEquivalence::check(const bool useCache, const bool	useCongruence, const bool beLax)
{
	RankedAlphabet rankedAlphabet = getRankedAlphabet();
	StateSetCoupleSet all, done, todo, knownPairs;
	StateSetCouple actual;
	getLeafCouples(rankedAlphabet, all);
	pruneRankedAlphabet(rankedAlphabet);
	todo = all;
	knownPairs = all;
	
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
			if(useCache)
			{
				getPostCached(symbol, actual, done);
			}
			else
			{
				getPost(symbol, actual, done);
			}
			for(auto next : post)
			{
				if(!isCoupleFinalStateEquivalent(next))
				{
					return false;
				}
				
				if(useCongruence)
				{
					if(useCache)
					{
						if(!isCongruenceClosureMemberCachedStrict(next, knownPairs))
						{
							todo.insert(next);
						}
					}
					else
					{
						if(!isCongruenceClosureMember(next, knownPairs))
						{
							todo.insert(next);
						}
					}
				}
				else
				{
					if(!isMember(next, knownPairs))
					{
						todo.insert(next);
					}
				}
			}
		}
	}
	return true;
}
