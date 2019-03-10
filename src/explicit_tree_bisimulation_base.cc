/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Bisimulation base for explicitly represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_bisimulation_base.hh"

// unsigned long pair_cnt1, true_cnt1;

using namespace ExplicitTreeUpwardBisimulation;

BisimulationBase::BisimulationBase(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger)
	: smaller(smaller), bigger(bigger),
		rankedAlphabet(), post(),
		successors_key(), successors_iter(),
		s_successors_cache(), b_successors_cache()
{
	RankedSymbol s;
	for(auto transition : smaller)
	{
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		rankedAlphabet.insert(s);
		
		std::get<0>(successors_key) =  s.first;	
		for(size_t pos = 0; pos < s.second; pos++)
		{
			std::get<1>(successors_key) =  pos;
			std::get<2>(successors_key) =  transition.GetChildren()[pos];
			successors_iter = s_successors_cache.find(successors_key);
			if(successors_iter == s_successors_cache.end())
			{
				StateSet set;
				s_successors_cache.emplace(successors_key, set);
				successors_iter = s_successors_cache.find(successors_key);
			}
			if (!isMember(transition.GetParent(), successors_iter->second))
				successors_iter->second.insert(transition.GetParent());
		}
	}
	for(auto transition : bigger)
	{
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		rankedAlphabet.insert(s);
		
		std::get<0>(successors_key) =  s.first;	
		for(size_t pos = 0; pos < s.second; pos++)
		{
			std::get<1>(successors_key) =  pos;
			std::get<2>(successors_key) =  transition.GetChildren()[pos];

			successors_iter = b_successors_cache.find(successors_key);
			if(successors_iter == b_successors_cache.end())
			{
				StateSet set;
				b_successors_cache.emplace(successors_key, set);
				successors_iter = b_successors_cache.find(successors_key);
			}
			if (!isMember(transition.GetParent(), successors_iter->second))
				successors_iter->second.insert(transition.GetParent());
		}
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
			for(auto transition : smaller){
				if(transition.GetSymbol() == symbol.first)
				{
					c.first.insert(transition.GetParent());
				}
			}
			for(auto transition : bigger){
				if(transition.GetSymbol() == symbol.first)
				{
					c.second.insert(transition.GetParent());
				}
			}
			set.insert(c);
		}
	}
}

bool BisimulationBase::areLeavesEquivalent(StateSetCoupleSet &todo)
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

void BisimulationBase::getPost(RankedSymbol &symbol, StateSetCouple &actual, StateSetCoupleSet &done)
{
	post.clear();
	std::get<0>(successors_key) = symbol.first;
	StateSetCouple next;
	if(symbol.second == 1)
	{
		std::get<1>(successors_key) = 0;
		getPostAtFixedPos(next, actual);
		post.emplace(next);
	}
	else if (symbol.second == 2)
	{
		StateSet empty;
		//actual at first position
		std::get<1>(successors_key) = 0;
		getPostAtFixedPos(next, actual);
		for(auto pair : done)
		{
			StateSetCouple context;
			//context at second position
			std::get<1>(successors_key) = 1;
			getPostAtFixedPos(context, pair);
			context.first = intersection(context.first, next.first);
			context.second = intersection(context.second, next.second);
			post.emplace(context);
		}

		next.first.clear();
		next.second.clear();
		//actual at second position
		std::get<1>(successors_key) = 1;
		getPostAtFixedPos(next, actual);
		for(auto pair : done)
		{
			StateSetCouple context;
			//context at first position
			std::get<1>(successors_key) = 0;
			getPostAtFixedPos(context, pair);
			context.first = intersection(context.first, next.first);
			context.second = intersection(context.second, next.second);
			post.emplace(context);
		}
	}
}

void BisimulationBase::getPostAtFixedPos(StateSetCouple &next, StateSetCouple &pair)
{
	for(auto state : pair.first)
	{
		std::get<2>(successors_key) = state;
		successors_iter = s_successors_cache.find(successors_key); 
		if(successors_iter != s_successors_cache.end())
			for (auto parent: successors_iter->second)
				next.first.insert(parent);
	}
	for(auto state : pair.second)
	{
		std::get<2>(successors_key) = state;
		successors_iter = b_successors_cache.find(successors_key); 
		if(successors_iter != b_successors_cache.end())
			for (auto parent: successors_iter->second)
				next.second.insert(parent);
	}
}

bool BisimulationBase::isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set)
{
	if(isMember(item, set))
	{
		return true;
	}

	// pair_cnt1++;

	bool changed = true;
	std::vector<bool> used_s(set.size(), false);
	std::vector<bool> used_b(set.size(), false);
	while(changed)
	{
		int i = 0;
		changed = false;
		for(auto set_item : set)
		{
			if(!used_s[i])
			{
				Expandable exp = isExpandableBy(item.first, set_item);
				if(exp == First || exp == Second)
				{
					changed = true;
					used_s[i] = true;
					for(auto si : (exp == First ? (set_item.first) : (set_item.second)))
					{
						item.first.insert(si);
					}
				}
			}
			if(!used_b[i])
			{
				Expandable exp = isExpandableBy(item.second, set_item);
				if(exp == First || exp == Second)
				{
					changed = true;
					used_b[i] = true;
					for(auto si : (exp == First ? (set_item.first) : (set_item.second)))
					{
						item.second.insert(si);
					}
				}
			}
			i++;
		}
	}
	if (item.first == item.second)
	{
		// true_cnt1++;
		return true;
	}
	return false;
}

Expandable BisimulationBase::isExpandableBy(StateSet &expandee, StateSetCouple &expander)
{
	bool first_subset = isSubset(expander.first, expandee);
	bool second_subset = isSubset(expander.second, expandee);
	if (first_subset && second_subset)
		return Both;
	if (first_subset)
		return Second;
	if (second_subset)
		return First;
	return None;
}
