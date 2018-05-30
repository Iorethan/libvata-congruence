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
#include <unordered_map>

using namespace VATA;
using namespace ExplicitTreeUpwardBisimulation;

BisimulationBase::BisimulationBase(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger)
	: smaller(smaller), bigger(bigger), post(),
		smallerTrans(), biggerTrans(), rankedAlphabet(),
		variant_key(), variant_cache(), variant_iter(),
		transition_key(), transition_cache(), transition_iter()
{
	RankedSymbol s;
	for(auto transition : smaller){
		smallerTrans.push_back(transition);
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		rankedAlphabet.insert(s);
	}
	for(auto transition : bigger){
		biggerTrans.push_back(transition);
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		rankedAlphabet.insert(s);
	}
}

void BisimulationBase::pruneRankedAlphabet()
{
	for(auto symbol : rankedAlphabet)
	{
		if(symbol.second == 0)
		{
			rankedAlphabet.erase(symbol);
		}
	}
}

void BisimulationBase::getLeafCouples(StateSetCoupleSet &set)
{
	for(auto symbol : rankedAlphabet){
		if(symbol.second == 0)
		{
			StateSetCouple c;
			c.first = getStateSetBySymbol(symbol.first, smaller);
			c.second = getStateSetBySymbol(symbol.first, bigger);
			set.insert(c);
		}
	}
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

void BisimulationBase::getPost(RankedSymbol &symbol, StateSetCouple &actual, StateSetCoupleSet &done)
{
	std::string keySmall = std::to_string(symbol.first) + "_";
	serialize(keySmall, actual.first);

	std::string keyBig = std::to_string(symbol.first) + "_";
	serialize(keyBig, actual.second);

	TransitionSetKeyCoupleVector actualTransitions;
	for(size_t pos = 0; pos < symbol.second; pos++)
	{
		TransitionSetKeyCouple tmp;

		transition_key = keySmall + std::to_string(pos);
		getValidTransitionsAtPos(smallerTrans, actual.first, symbol.first, pos);
		tmp.first = transition_iter->first;

		transition_key = keyBig + std::to_string(pos);
		getValidTransitionsAtPos(biggerTrans, actual.second, symbol.first, pos);
		tmp.second =  transition_iter->first;

		actualTransitions.push_back(tmp);
	}

	TransitionSetKeyCouple2DVector doneTransitions(done.size());
	size_t i = 0;
	for(auto couple : done)
	{
		keySmall = std::to_string(symbol.first) + "_";
		serialize(keySmall, couple.first);

		keyBig = std::to_string(symbol.first) + "_";
		serialize(keyBig, couple.second);

		for(size_t pos = 0; pos < symbol.second; pos++)
		{
			TransitionSetKeyCouple tmp;

			transition_key = keySmall + std::to_string(pos);
			getValidTransitionsAtPos(smallerTrans, actual.first, symbol.first, pos);
			tmp.first = transition_iter->first;

			transition_key = keyBig + std::to_string(pos);
			getValidTransitionsAtPos(biggerTrans, actual.second, symbol.first, pos);
			tmp.second = transition_iter->first;

			doneTransitions[i].push_back(tmp);
		}
		i++;
	}
	calculatePost(actualTransitions, doneTransitions, symbol.second);
	return;
}

void BisimulationBase::serialize(std::string &key, const StateSet &set)
{		
	for (auto item : set)
	{
		key += std::to_string(item) + ",";
	}
}

void BisimulationBase::getValidTransitionsAtPos(const TransitionVector& transitions, StateSet &actual, SymbolType &symbol, size_t position)
{
	transition_iter = transition_cache.find(transition_key);
	if(transition_iter == transition_cache.end())
	{
		TransitionIdSet set;
		for(size_t i = 0; i < transitions.size(); i++)
		{
			if(transitions[i].GetSymbol() == symbol && isMember(transitions[i].GetChildren()[position], actual))
			{
				set.insert(i);
			}
		}
		transition_cache.emplace(std::pair<std::string, TransitionIdSet>(transition_key, set));
		transition_iter = transition_cache.find(transition_key);
	}
}

void BisimulationBase::calculatePost(
	TransitionSetKeyCoupleVector &actualTransitions,
	TransitionSetKeyCouple2DVector &doneTransitions,
	size_t rank)
{
	post.clear();
	generatePostVariants(rank - 1, doneTransitions.size());
	for(size_t pos = 0; pos < rank; pos++)
	{
		for(auto variant : variant_iter->second)
		{
			int correction = 0;
			TransitionIdSet sml = transition_cache.find(actualTransitions[pos].first)->second;
			TransitionIdSet bgr = transition_cache.find(actualTransitions[pos].second)->second;
			for(size_t i = 0; i < rank; i++)
			{
				if (i != pos)
				{
					sml = intersection(sml, transition_cache.find(doneTransitions[variant[i + correction]][i].first)->second);
					bgr = intersection(bgr, transition_cache.find(doneTransitions[variant[i + correction]][i].second)->second);
				}
				else
				{
					correction = -1;
				}
			}
			post.emplace(statesFromTransitions(sml, smallerTrans), statesFromTransitions(bgr, biggerTrans));
		}
	}
	return;
}

void BisimulationBase::generatePostVariants(size_t n, size_t k)
{
	variant_key = k * 1000 + n;
	variant_iter = variant_cache.find(variant_key);
	if(variant_iter == variant_cache.end())
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
		variant_cache.emplace(std::pair<size_t, PostVariantVector>(variant_key, prev));
		variant_iter = variant_cache.find(variant_key);
	}
}

StateSet BisimulationBase::statesFromTransitions(TransitionIdSet &ids, TransitionVector &transitions)
{
	StateSet set;
	for(auto id : ids)
	{
		set.insert(transitions[id].GetParent());
	}
	return set;
}
