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

	struct TodoComparator;

	using StateType      = ExplicitTreeAutCore::StateType;
	using SymbolType     = ExplicitTreeAutCore::SymbolType;
	using Transition	 = ExplicitTreeAut::Transition;

	typedef std::set <StateType> StateSet;
	typedef std::pair <StateSet, StateSet> StateSetCouple;
	typedef std::set <StateSetCouple> StateSetCoupleSet;
	typedef std::set <StateSetCouple, TodoComparator> TodoSet;

	typedef StateSetCoupleSet::iterator StateSetCoupleIter;

	typedef std::vector<bool> Bitmap;

	typedef std::pair<SymbolType, size_t> RankedSymbol;
	typedef std::set<RankedSymbol> RankedAlphabet;

	struct InputSize {
		size_t alphabet;
		size_t arity;
		size_t states;
	};

	struct TodoComparator {
		bool operator() (const StateSetCouple& lhs, const StateSetCouple& rhs) const {
			if (lhs.first.size() + lhs.second.size() < rhs.first.size() + rhs.second.size())
				return true;
			if (lhs.first.size() + lhs.second.size() > rhs.first.size() + rhs.second.size())
				return false;
			return lhs < rhs;
		}
	};

	struct StateSetHash
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

	class BisimulationBase;
	class BisimulationEquivalence;

	enum Expandable {
		None,
		First,
		Second,
		Both
	};
	
	template <typename type> bool isMember(type item, std::set<type> &set)
	{
		return set.find(item) != set.end();
	}

	template <typename type> std::set<type> intersection(std::set<type> &left, std::set<type> &right)
	{
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
	{
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
			ExplicitTreeAutCore smaller;
			ExplicitTreeAutCore bigger;

			RankedAlphabet ranked_alphabet;

			StateSetCouple actual;

			TodoSet todo;
			StateSetCoupleSet done, knownPairs;

			InputSize input_size;
			StateSet*** successors;
			std::unordered_map<StateSet, StateSet, StateSetHash>** set_successors;
			std::unordered_map<StateSet, StateSet, StateSetHash>::iterator set_successors_iter;

		public:
			BisimulationBase(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			virtual ~BisimulationBase();
			
			virtual bool check() = 0;

			void pruneRankedAlphabet();

			void getLeafCouples();
			bool areLeavesEquivalent();
			virtual bool isCoupleFinalStateEquivalent(StateSetCouple &couple) = 0;

			bool getPost(RankedSymbol &symbol);
			void getPostAtFixedPos(StateSetCouple &next, StateSetCouple &pair, size_t symbol, size_t pos);

			bool todoInsert(StateSetCouple &next);

			virtual bool isCongruenceClosureMember(StateSetCouple item) = 0;
			Expandable isExpandableBy(StateSet &expandee, StateSetCouple &expander);
			Expandable isExpandableBy(StateSet &expandee, StateSetCoupleIter expander);

	};
}
#endif