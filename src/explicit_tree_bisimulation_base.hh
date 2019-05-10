/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Upward bisimulation up to congruence for explicitly represented tree
 *    automata.
 *
 *****************************************************************************/

#ifndef _EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_BASE_HH_
#define	_EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_BASE_HH_


#include "explicit_tree_aut_core.hh"
#include <vata/explicit_tree_aut.hh>

namespace ExplicitTreeUpwardBisimulation{ 
	using namespace VATA;

	struct TodoComparator;										// Defines order of pairs stored in TodoSet.

	using StateType      = ExplicitTreeAutCore::StateType;
	using SymbolType     = ExplicitTreeAutCore::SymbolType;
	using Transition	 = ExplicitTreeAut::Transition;

	typedef std::set <StateType> StateSet;						// Macrostate.
	typedef std::pair <StateSet, StateSet> StateSetCouple;		// Macrostate pair.
	typedef std::set <StateSetCouple> StateSetCoupleSet;		// Set of macrostate pairs.
	typedef std::set <StateSetCouple, TodoComparator> TodoSet;	// Ordered set of macrostate pairs.

	typedef StateSetCoupleSet::iterator StateSetCoupleIter;

	typedef std::vector<bool> Bitmap;

	typedef std::pair<SymbolType, size_t> RankedSymbol;			// Symbol + arity.
	typedef std::set<RankedSymbol> RankedAlphabet;				// Set of ranked symbols.

	struct InputSize {											// Automaton size description.
		size_t alphabet;
		size_t arity;
		size_t states;
	};

	struct TodoComparator {										// Ordering on macrostate pairs.
		bool operator() (const StateSetCouple& lhs, const StateSetCouple& rhs) const {
			if (lhs.first.size() + lhs.second.size() < rhs.first.size() + rhs.second.size())
				return true;
			if (lhs.first.size() + lhs.second.size() > rhs.first.size() + rhs.second.size())
				return false;
			return lhs < rhs;
		}
	};

	struct StateSetHash											// Macrostate hashing function.
	{
		std::size_t operator () (const StateSet &p) const
		{
			size_t hash = 0;
			for(auto item : p)
			{
				boost::hash_combine(hash, item);
			}
			return hash;
		}
	};

	enum Expandable {											// Expandability for fixpoint calculation, used in congruence closure calculation.
		None,													// Neither macrostate from given pair can be expanded.
		First,													// First macrostate can be expanded.
		Second,													// Second macrostate can be expanded.
		Both													// Both can be expanded.
	};
	
	template <typename type> bool isMember(type item, std::set<type> &set)		// Membership checking function.
	{
		return set.find(item) != set.end();
	}

	template <typename type> std::set<type> intersection(std::set<type> &left, std::set<type> &right)
	{																			// Compute set intersection.
		std::set<type> intersect;
		set_intersection(
			left.begin(),
			left.end(),
			right.begin(),
			right.end(),
			std::inserter(intersect, intersect.begin())
		);
		return intersect;
	}

	template <typename type> bool isSubset(const std::set<type> &left, const std::set<type> &right)
	{																			// Inclusion checking function.
		auto r = right.begin();
		auto re = right.end();
		for(auto l = left.begin(); l != left.end(); l++)
		{
			while(r != re && *r < *l){
				r++;
			}
			if(r == re || *r > *l)
			{
				return false;
			}	
		}	
		return true;
	}

GCC_DIAG_OFF(effc++)
	class BisimulationBase{	
GCC_DIAG_ON(effc++)
		protected:
			ExplicitTreeAutCore smaller;				// Input automata.
			ExplicitTreeAutCore bigger;

			RankedAlphabet ranked_alphabet;				// Alphabet of input automata.

			StateSetCouple actual;						// Currently processed pair.

			TodoSet todo;								// Set uf pairs to be processed.
			StateSetCoupleSet done, knownPairs;			// Expanded and visited pairs.

			InputSize input_size;						// Automata size.
			StateSet*** successors;						// Array holding matrix representation of transition rules.
			std::unordered_map<StateSet, StateSet, StateSetHash>** set_successors;		// Map holding successors of known macrostates.
			std::unordered_map<StateSet, StateSet, StateSetHash>::iterator set_successors_iter;

		public:
			BisimulationBase(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			virtual ~BisimulationBase();
			
			virtual bool check() = 0;			// Inclusion/equivalence check function.

			void pruneRankedAlphabet();			// Removal of 0-arity symbols from alphabet.

			void getLeafCouples();				// Create macrostate pairs from leaf rules.
			bool areLeavesEquivalent();			// Check if pairs from leaf rules are equal.
			virtual bool isCoupleFinalStateEquivalent(StateSetCouple &couple) = 0;		// Check if pair is equivalent.

			bool getPost(RankedSymbol &symbol);	// Calculate successors for currently proccessed pair.
			void getPostAtFixedPos(StateSetCouple &next, StateSetCouple &pair, size_t symbol, size_t pos);

			bool todoInsert(StateSetCouple &next);	// Insert successors into Todo.

			virtual bool isCongruenceClosureMember(StateSetCouple item) = 0;		// Check if pair is in congruence closure of Done.
			Expandable isExpandableBy(StateSet &expandee, StateSetCouple &expander);		// Check if pair can be expanded by expandee.
			Expandable isExpandableBy(StateSet &expandee, StateSetCoupleIter expander);

	};
}
#endif