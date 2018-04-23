/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
 *
 *  Description:
 *    Upward inclusion for explicitly represented tree automata.
 *
 *****************************************************************************/
//makeStep pro kazdy symbol zvlast.
//

#include <iostream>
#include <map>
#include <tuple>
#include "explicit_tree_congr_incl_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardCongruence;

CongruenceEquivalence::CongruenceEquivalence(
	const ExplicitTreeAutCore&        _smaller,
	const ExplicitTreeAutCore&        _bigger)
	: CongruenceBase(_smaller, _bigger)
	{
		smaller = smaller.RemoveUnreachableStates();
		smaller = smaller.RemoveUselessStates();
		bigger = bigger.RemoveUnreachableStates();
		bigger = bigger.RemoveUselessStates();
	}

RankedAlphabet CongruenceEquivalence::getRankedAlphabet()
{
	RankedAlphabet tmp;
	for(auto transition : smaller){
		RankedSymbol s;
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		tmp.insert(s);
	}
	for(auto transition : bigger){
		RankedSymbol s;
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		tmp.insert(s);
	}
	return tmp;
}

StateSetCoupleSet CongruenceEquivalence::getLeafCouples2(const RankedAlphabet &alphabet)
{
	StateSetCoupleSet tmp;
	for(auto symbol : alphabet){
		if(symbol.second == 0)
		{
			StateSetCouple c;
			c.first = getStateSetBySymbol(symbol.first, smaller);
			c.second = getStateSetBySymbol(symbol.first, bigger);
			tmp.insert(c);
		}
	}
	return tmp;
}

StateSet CongruenceEquivalence::getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton)
{
	StateSet tmp;
	for(auto transition : automaton){
		if(transition.GetSymbol() == symbol)
		{
			tmp.insert(transition.GetParent());
		}
	}
	return tmp;
}

StateSetCouple CongruenceEquivalence::selectActual(StateSetCoupleSet& todo)
{
	return *(todo.begin());
}

bool CongruenceEquivalence::isCoupleFinalStateEquivalent(StateSetCouple couple)
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

StateSetCoupleSet CongruenceEquivalence::getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet &done)
{
	TransitionSetCoupleVector actualTransitions;
	for(size_t pos = 0; pos < symbol.second; pos++)
	{
		TransitionSetCouple tmp;
		tmp.first = getValidTransitionsAtPos(symbol.first, actual.first, smaller, pos, 0);
		tmp.second = getValidTransitionsAtPos(symbol.first, actual.second, bigger, pos, 1);
		actualTransitions.push_back(tmp);
	}

	TransitionSetCouple2DVector doneTransitions(done.size());
	size_t i = 0;
	for(auto couple : done)
	{
		for(size_t pos = 0; pos < symbol.second; pos++)
		{
			TransitionSetCouple tmp;
			tmp.first = getValidTransitionsAtPos(symbol.first, couple.first, smaller, pos, 0);
			tmp.second = getValidTransitionsAtPos(symbol.first, couple.second, bigger, pos, 1);
			doneTransitions[i].push_back(tmp);
		}
		i++;
	}
	return calculatePost(actualTransitions, doneTransitions, symbol.second);
}

StateSetCoupleSet CongruenceEquivalence::calculatePost(
	TransitionSetCoupleVector &actualTransitions,
	TransitionSetCouple2DVector &doneTransitions,
	size_t rank)
{
	StateSetCoupleSet post;
	PostVariantVector variants;
	generatePostVariants(variants, rank - 1, doneTransitions.size());
	for(size_t pos = 0; pos < rank; pos++)
	{
		for(auto variant : variants)
		{
			int correction = 0;
			TransitionSet sml = actualTransitions[pos].first;
			TransitionSet bgr = actualTransitions[pos].second;
			for(size_t i = 0; i < rank; i++)
			{
				if (i != pos)
				{
					sml = intersection(sml, doneTransitions[variant[i + correction]][i].first);
					bgr = intersection(bgr, doneTransitions[variant[i + correction]][i].second);
				}
				else
				{
					correction = -1;
				}
			}
			StateSetCouple tmp = statesFromTransitions(sml, bgr);
			post.insert(tmp);
		}
	}
	return post;
}


StateSetCouple CongruenceEquivalence::statesFromTransitions(TransitionSet &sml, TransitionSet &bgr)
{
	StateSetCouple tmp;
	StateSet s, b;
	for(auto transition : sml)
	{
		s.insert(transition.GetParent());
	}
	for(auto transition : bgr)
	{
		b.insert(transition.GetParent());
	}
	tmp.first = s;
	tmp.second = b;
	return tmp;
}

void CongruenceEquivalence::generatePostVariants(PostVariantVector &result, size_t n, size_t k)
{
	PostVariantVector current, prev;
	PostVariant subresult;
	prev.push_back(subresult);
	for(size_t i = 0; i < n; i++)
	{
		for(size_t j = 0; j < k; j++)
		{
			for(auto item : prev)
			{
				std::vector<size_t> tmp = item;
				tmp.push_back(j);
				current.push_back(tmp);
			}
		}
		prev = current;
		current.clear();
	}
	for(auto item : prev)
	{
		result.push_back(item);
	}
	return;
}

TransitionSet CongruenceEquivalence::getValidTransitionsAtPos(SymbolType symbol, StateSet actual, const ExplicitTreeAutCore& automaton, size_t position, int index)
{
	static std::map<std::tuple<SymbolType, StateSet, size_t, int>, TransitionSet> transition_cache;
	std::map<std::tuple<SymbolType, StateSet, size_t, int>, TransitionSet>::iterator iter;	

	if((iter = transition_cache.find(std::make_tuple(symbol, actual, position, index))) != transition_cache.end())
	{
		return iter->second;
	}
	else
	{	
		std::tuple<SymbolType, StateSet, size_t, int> tuple(symbol, actual, position, index);
		TransitionSet set;
		for(auto transition : automaton)
		{
			if(transition.GetSymbol() == symbol && isMember(transition.GetChildren()[position], actual))
			{
				set.insert(transition);
			}
		}
		transition_cache.emplace(std::pair<std::tuple<SymbolType, StateSet, size_t, int>, TransitionSet>(tuple, set));
		return set;
	}

	// TransitionSet set;
	// for(auto transition : automaton)
	// {
	// 	if(transition.GetSymbol() == symbol && isMember(transition.GetChildren()[position], actual))
	// 	{
	// 		set.insert(transition);
	// 	}
	// }
	// return set;
}

bool CongruenceEquivalence::isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set)
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

bool CongruenceEquivalence::isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item)
{
	static std::map<std::tuple<StateSet, StateSet, StateSetCouple>, bool> cache;
	std::map<std::tuple<StateSet, StateSet, StateSetCouple>, bool>::iterator iter;
	if((iter = cache.find(std::make_tuple(first, second, item))) != cache.end())
	{
		return iter->second;
	}
	else
	{	
		std::tuple<StateSet, StateSet, StateSetCouple> tuple(first, second, item);
		bool result = intersection(first, item.first).size() != 0 || intersection(second, item.second).size() != 0;
		cache.insert(std::pair<std::tuple<StateSet, StateSet, StateSetCouple>, bool>(tuple, result));
		return result;
	}
	// return intersection(first, item.first).size() != 0 ||
	// 	intersection(second, item.second).size() != 0;
}

bool CongruenceEquivalence::check()
{
	RankedAlphabet rankedAlphabet = getRankedAlphabet();
	StateSetCoupleSet done, todo = getLeafCouples2(rankedAlphabet);
	StateSetCouple actual;
	
	for(auto couple : todo)
	{
		if(!isCoupleFinalStateEquivalent(couple))
		{
			return false;
		}
	}

	for(auto symbol : rankedAlphabet)
	{
		if(symbol.second == 0)
		{
			rankedAlphabet.erase(symbol);
		}
	}

	// StateSetCoupleSet congruence1 = todo;
	while(!todo.empty())
	{
		StateSetCoupleSet congruence = set_union(todo, done);
		actual = selectActual(todo);
		
		if(!isCongruenceClosureMember(actual, congruence))
		{
			todo.erase(actual);
			done.insert(actual);
			continue;
		}

		todo.erase(actual);
		done.insert(actual);

		for(auto symbol : rankedAlphabet)
		{
			StateSetCoupleSet post = getPost(symbol, actual, done);
			for(auto next : post)
			{
				if(!isCoupleFinalStateEquivalent(next))
				{
					return false;
				}
				
				if(!isCongruenceClosureMember(next, congruence))
				{
					// congruence1.insert(next);
					todo.insert(next);
				}
			}
		}
	}
	return true;
}
