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
	: smaller(smaller), bigger(bigger), post(),
		variant_cache(), variant_iter(),
		transition_cache(), transition_iter()
{}

RankedAlphabet BisimulationBase::getRankedAlphabet()
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

void BisimulationBase::pruneRankedAlphabet(RankedAlphabet &rankedAlphabet)
{
	for(auto symbol : rankedAlphabet)
	{
		if(symbol.second == 0)
		{
			rankedAlphabet.erase(symbol);
		}
	}
}

StateSetCoupleSet BisimulationBase::getLeafCouples(const RankedAlphabet &alphabet)
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

StateSet BisimulationBase::getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton)
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

void BisimulationBase::getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet &done)
{
	TransitionSetCoupleVector actualTransitions;
	for(size_t pos = 0; pos < symbol.second; pos++)
	{
		TransitionSetCouple tmp;
		tmp.first = getValidTransitionsAtPos(symbol.first, actual.first, smaller, pos);
		tmp.second = getValidTransitionsAtPos(symbol.first, actual.second, bigger, pos);
		actualTransitions.push_back(tmp);
	}

	TransitionSetCouple2DVector doneTransitions(done.size());
	size_t i = 0;
	for(auto couple : done)
	{
		for(size_t pos = 0; pos < symbol.second; pos++)
		{
			TransitionSetCouple tmp;
			tmp.first = getValidTransitionsAtPos(symbol.first, couple.first, smaller, pos);
			tmp.second = getValidTransitionsAtPos(symbol.first, couple.second, bigger, pos);
			doneTransitions[i].push_back(tmp);
		}
		i++;
	}
	calculatePost(actualTransitions, doneTransitions, symbol.second);
	return;
}

void BisimulationBase::getPostCached(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet &done)
{
	TransitionSetCoupleVector actualTransitions;
	for(size_t pos = 0; pos < symbol.second; pos++)
	{
		TransitionSetCouple tmp;
		getValidTransitionsAtPosCached(symbol.first, actual.first, smaller, pos, 0);
		tmp.first = transition_iter->second;
		getValidTransitionsAtPosCached(symbol.first, actual.second, bigger, pos, 1);
		tmp.second =  transition_iter->second;
		actualTransitions.push_back(tmp);
	}

	TransitionSetCouple2DVector doneTransitions(done.size());
	size_t i = 0;
	for(auto couple : done)
	{
		for(size_t pos = 0; pos < symbol.second; pos++)
		{
			TransitionSetCouple tmp;
			getValidTransitionsAtPosCached(symbol.first, couple.first, smaller, pos, 0);
			tmp.first = transition_iter->second;
			getValidTransitionsAtPosCached(symbol.first, couple.second, bigger, pos, 1);
			tmp.second = transition_iter->second;
			doneTransitions[i].push_back(tmp);
		}
		i++;
	}
	calculatePost(actualTransitions, doneTransitions, symbol.second);
	return;
}

void BisimulationBase::calculatePost(
	TransitionSetCoupleVector &actualTransitions,
	TransitionSetCouple2DVector &doneTransitions,
	size_t rank)
{
	post.clear();
	generatePostVariants(rank - 1, doneTransitions.size());
	for(size_t pos = 0; pos < rank; pos++)
	{
		for(auto variant : variant_iter->second)
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
			post.emplace(statesFromTransitions(sml), statesFromTransitions(bgr));
		}
	}
	return;
}

StateSet BisimulationBase::statesFromTransitions(TransitionSet &transitions)
{
	StateSet set;
	for(auto transition : transitions)
	{
		set.insert(transition.GetParent());
	}
	return set;
}

void BisimulationBase::generatePostVariants(size_t n, size_t k)
{
	std::tuple<size_t, size_t> tuple(n, k);
	if((variant_cache.find(tuple)) == variant_cache.end())
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
		variant_cache.emplace(std::pair<std::tuple<size_t, size_t>, PostVariantVector>(tuple, prev));
	}
	variant_iter = variant_cache.find(std::make_tuple(n, k));
}

TransitionSet BisimulationBase::getValidTransitionsAtPos(SymbolType symbol, StateSet actual, const ExplicitTreeAutCore& automaton, size_t position)
{
	TransitionSet set;
	for(auto transition : automaton)
	{
		if(transition.GetSymbol() == symbol && isMember(transition.GetChildren()[position], actual))
		{
			set.insert(transition);
		}
	}
	return set;
}

void BisimulationBase::getValidTransitionsAtPosCached(SymbolType symbol, StateSet actual, const ExplicitTreeAutCore& automaton, size_t position, int index)
{
	std::tuple<SymbolType, StateSet, size_t, int> tuple(symbol, actual, position, index);
	if(transition_cache.find(tuple) == transition_cache.end())
	{
		TransitionSet set;
		for(auto transition : automaton)
		{
			if(transition.GetSymbol() == symbol && isMember(transition.GetChildren()[position], actual))
			{
				set.insert(transition);
			}
		}
		transition_cache.emplace(std::pair<std::tuple<SymbolType, StateSet, size_t, int>, TransitionSet>(tuple, set));
	}
	transition_iter = transition_cache.find(tuple);
}