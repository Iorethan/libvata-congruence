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
	if((couple.first.size() == 0) != (couple.second.size() == 0))
	{
		return false;
	}

	FinalStateSet s = smaller.GetFinalStates();
	FinalStateSet b = bigger.GetFinalStates();
	StateSet ss(s.begin(), s.end());
	StateSet sb(b.begin(), b.end());
	return intersection(couple.first, ss).empty() ==
		intersection(couple.second, sb).empty();
}

StateSetCoupleSet CongruenceEquivalence::getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet done)
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

TransitionSet CongruenceEquivalence::getValidTransitionsAtPos(SymbolType symbol, StateSet actual, const ExplicitTreeAutCore& automaton, size_t position)
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

	while(!todo.empty())
	{
		actual = selectActual(todo);
		done.insert(actual);

		for(auto symbol : rankedAlphabet)
		{
			if(symbol.second != 0)
			{
				StateSetCoupleSet post = getPost(symbol, actual, done);
				for(auto next : post)
				{
					if(!isCoupleFinalStateEquivalent(next))
					{
						return false;
					}
					
					if(!isMember(next, done))
					{
						todo.insert(next);
					}
				}
			}
		}
		todo.erase(actual);
	}
	return true;



}
