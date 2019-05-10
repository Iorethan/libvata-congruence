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
	: smaller(smaller), bigger(bigger),				// Input automata.
		ranked_alphabet(), actual(),				// Init data structures.
		todo(), done(), knownPairs(),
		input_size(), successors(),
		set_successors(), set_successors_iter()
{
	for(auto transition : smaller)					// Calculate size of input automata (smaller).
	{
		ranked_alphabet.insert(std::make_pair(transition.GetSymbol(), transition.GetChildrenSize()));
		if(transition.GetSymbol() > input_size.alphabet)
			input_size.alphabet = transition.GetSymbol();
		if(transition.GetChildrenSize() > input_size.arity)
			input_size.arity = transition.GetChildrenSize();
		if(transition.GetParent() > input_size.states)
			input_size.states = transition.GetParent();
		for(auto state : transition.GetChildren())
			if(state > input_size.states)
				input_size.states = state;
	}
	for(auto transition : bigger)					// Calculate size of input automata (bigger).
	{
		ranked_alphabet.insert(std::make_pair(transition.GetSymbol(), transition.GetChildrenSize()));
		if(transition.GetSymbol() > input_size.alphabet)
			input_size.alphabet = transition.GetSymbol();
		if(transition.GetChildrenSize() > input_size.arity)
			input_size.arity = transition.GetChildrenSize();
		if(transition.GetParent() > input_size.states)
			input_size.states = transition.GetParent();
		for(auto state : transition.GetChildren())
			if(state > input_size.states)
				input_size.states = state;
	}
	input_size.alphabet++;			// Size adjustment.
	input_size.states++;

	// Create array to hold matrix representation of transition rules (of appropriate size).
	successors = new StateSet **[input_size.alphabet]();
    for (size_t i = 0; i < input_size.alphabet; i++)
    {
        successors[i] = new StateSet *[input_size.arity]();
        for (size_t j = 0; j < input_size.arity; j++)
            successors[i][j] = new StateSet [input_size.states]();
    }
	
	// Create maps for holding successors of macrostates (for specific symbol)
	set_successors = new std::unordered_map<StateSet, StateSet, StateSetHash> *[input_size.alphabet]();
    for (size_t i = 0; i < input_size.alphabet; i++)
    {
		// Create maps for holding successors of macrostates (for specific position)
        set_successors[i] = new std::unordered_map<StateSet, StateSet, StateSetHash>[input_size.arity]();
    }

	// Fill in matrix representation of transition rules (smaller).
	for(auto transition : smaller)
	{
		if (transition.GetChildrenSize() == 0)
		{
			successors[transition.GetSymbol()][0][0].insert(transition.GetParent());
		}
		for(size_t pos = 0; pos < transition.GetChildrenSize(); pos++)
		{
			successors[transition.GetSymbol()][pos][transition.GetChildren()[pos]].insert(transition.GetParent());
		}
	}
	// Fill in matrix representation of transition rules (bigger).
	for(auto transition : bigger)
	{
		if (transition.GetChildrenSize() == 0)
		{
			successors[transition.GetSymbol()][0][1].insert(transition.GetParent());
		}
		for(size_t pos = 0; pos < transition.GetChildrenSize(); pos++)
		{
			successors[transition.GetSymbol()][pos][transition.GetChildren()[pos]].insert(transition.GetParent());
		}
	}
}


BisimulationBase::~BisimulationBase()
{
	// Free data structures.
	for (size_t i = 0; i < input_size.alphabet; i++)
	{
		for (size_t j = 0; j < input_size.arity; j++)
			delete[] successors[i][j];
		delete[] successors[i];
	}
	delete[] successors;


	for (size_t i = 0; i < input_size.alphabet; i++)
	{
		delete[] set_successors[i];
	}
	delete[] set_successors;
}

// Remove 0-arity symbols from alphabet.
void BisimulationBase::pruneRankedAlphabet()
{
	for(auto symbol : ranked_alphabet)
	{
		if(symbol.second == 0)
		{
			ranked_alphabet.erase(symbol);
		}
	}
}

// Calculate macrostate pairs reachable using leaf rules.
void BisimulationBase::getLeafCouples()
{
	for(auto symbol : ranked_alphabet){
		if(symbol.second == 0)
		{
			todo.insert(std::make_pair(successors[symbol.first][0][0], successors[symbol.first][0][1]));
			done.insert(std::make_pair(successors[symbol.first][0][0], successors[symbol.first][0][1]));
			knownPairs.insert(std::make_pair(successors[symbol.first][0][0], successors[symbol.first][0][1]));
		}
	}
}

// Check if leaf pairs are equivalent
bool BisimulationBase::areLeavesEquivalent()
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

bool BisimulationBase::getPost(RankedSymbol &symbol)
{
	StateSetCouple next;
	// Calculate successors for 1-arity symbols.
	if(symbol.second == 1)
	{
		getPostAtFixedPos(next, actual, symbol.first, 0);
		if(!todoInsert(next))
			return false;
	}
	// Calculate successors for 2-arity symbols.
	else if (symbol.second == 2)
	{
		StateSet empty;
		// Actual at the first position.
		getPostAtFixedPos(next, actual, symbol.first, 0);
		if(!next.first.empty() || !next.second.empty())
			for(auto pair : done)
			{
				StateSetCouple context;
				// Context at second position.
				getPostAtFixedPos(context, pair, symbol.first, 1);
				context.first = intersection(context.first, next.first);
				context.second = intersection(context.second, next.second);
				if(!todoInsert(context))
					return false;
			}

		next.first.clear();
		next.second.clear();
		// Actual at second position.
		getPostAtFixedPos(next, actual, symbol.first, 1);
		if(!next.first.empty() || !next.second.empty())
			for(auto pair : done)
			{
				StateSetCouple context;
				// Context at the first position.
				getPostAtFixedPos(context, pair, symbol.first, 0);
				context.first = intersection(context.first, next.first);
				context.second = intersection(context.second, next.second);
				if(!todoInsert(context))
					return false;
			}
	}
	// Calculate successors for 3+-arity symbols.
	else
	{
		// Currently not supported.
		throw NotImplementedException(__func__);
	}	
	return true;
}

// Calculate possible successors for given pair at given position.
void BisimulationBase::getPostAtFixedPos(StateSetCouple &next, StateSetCouple &pair, size_t symbol, size_t pos)
{
	// Check if successor for this macrostate at this position was already calculated (in smaller).
	set_successors_iter = set_successors[symbol][pos].find(pair.first);
	if(set_successors_iter != set_successors[symbol][pos].end())
	{
		// Use found successor.
		for(auto parent : set_successors_iter->second)
			next.first.insert(parent);
	}
	else
	{
		// Compute successor and store it for later use.
		auto iter = set_successors[symbol][pos].emplace(std::make_pair(pair.first, StateSet()));
		for(auto state : pair.first)
		{
			for(auto parent : successors[symbol][pos][state])
			{
				next.first.insert(parent);
				iter.first->second.insert(parent);
			}
		}
	}

	// Check if successor for this macrostate at this position was already calculated (in bigger).
	set_successors_iter = set_successors[symbol][pos].find(pair.second);
	if(set_successors_iter != set_successors[symbol][pos].end())
	{
		// Use found successor.
		for(auto parent : set_successors_iter->second)
			next.second.insert(parent);
	}
	else
	{
		// Compute successor and store it for later use.
		auto iter = set_successors[symbol][pos].emplace(std::make_pair(pair.second, StateSet()));
		for(auto state : pair.second)
		{
			for(auto parent : successors[symbol][pos][state])
			{
				next.second.insert(parent);
				iter.first->second.insert(parent);
			}
		}
	}
}

// Insert successor into Todo.
bool BisimulationBase::todoInsert(StateSetCouple &next)
{
	// Check for duplicates.
	if(knownPairs.find(next) == knownPairs.end())
	{
		// Add to KnownPairs.
		knownPairs.emplace(next);
		// Check equivalence.
		if(!isCoupleFinalStateEquivalent(next))
		{
			return false;
		}
		// Add to Todo.
		todo.emplace(next);
	}
	return true;
}

// Check if expandee can be expanded by expander.
Expandable BisimulationBase::isExpandableBy(StateSet &expandee, StateSetCouple &expander)
{
	// If a macrostate is a subset of other macrostate it can be expanded.
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

// Check if expandee can be expanded by expander.
Expandable BisimulationBase::isExpandableBy(StateSet &expandee, StateSetCoupleIter expander)
{
	bool first_subset = isSubset(expander->first, expandee);
	bool second_subset = isSubset(expander->second, expandee);
	if (first_subset && second_subset)
		return Both;
	if (first_subset)
		return Second;
	if (second_subset)
		return First;
	return None;
}
