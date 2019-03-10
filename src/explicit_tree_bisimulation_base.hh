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

	using StateType      = ExplicitTreeAutCore::StateType;
	using SymbolType     = ExplicitTreeAutCore::SymbolType;
	using Transition	 = ExplicitTreeAut::Transition;

	typedef std::set <StateType> StateSet;
	typedef std::pair <StateSet, StateSet> StateSetCouple;
	typedef std::set <StateSetCouple> StateSetCoupleSet;

	typedef std::vector<bool> Bitmap;

	typedef std::pair<SymbolType, size_t> RankedSymbol;
	typedef std::set<RankedSymbol> RankedAlphabet;

	// (symbol, position, state)
	typedef std::tuple<size_t, size_t, size_t> PostKey;

	struct PostKeyHash
	{
		std::size_t operator () (const std::tuple<size_t, size_t, size_t> &p) const
		{
			return std::get<1>(p) + (std::get<0>(p) << 4) + (std::get<2>(p) << 16);
		}
	};
	
	struct SetPostKeyHash
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

	template <typename type> bool isSubset(std::set<type> &left, std::set<type> &right)
	{
		std::set<type> difference;
		set_difference(
			left.begin(),
			left.end(),
			right.begin(),
			right.end(),
			std::inserter(difference, difference.begin())
		);
		return difference.size() == 0;
	}

	class BisimulationBase{	
		protected:
			ExplicitTreeAutCore smaller;
			ExplicitTreeAutCore bigger;

			RankedAlphabet rankedAlphabet;

			StateSetCoupleSet post;

			// cache
			PostKey successors_key;
			std::unordered_map<PostKey, StateSet, PostKeyHash>::iterator successors_iter;
			std::unordered_map<PostKey, StateSet, PostKeyHash> s_successors_cache;
			std::unordered_map<PostKey, StateSet, PostKeyHash> b_successors_cache;

		public:
			BisimulationBase(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			virtual ~BisimulationBase() {}
			
			virtual bool check() = 0;

			void pruneRankedAlphabet();

			void getLeafCouples(StateSetCoupleSet &set);
			bool areLeavesEquivalent(StateSetCoupleSet &todo);
			virtual bool isCoupleFinalStateEquivalent(StateSetCouple &couple) = 0;

			void getPost(RankedSymbol &symbol, StateSetCouple &actual, StateSetCoupleSet &done);
			void getPostAtFixedPos(StateSetCouple &next, StateSetCouple &pair);

			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);
			Expandable isExpandableBy(StateSet &expandee, StateSetCouple &expander);

	};
}
#endif