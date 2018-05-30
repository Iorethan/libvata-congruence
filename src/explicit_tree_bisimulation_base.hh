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

	typedef std::vector<Transition> TransitionVector;
	typedef std::set<size_t> TransitionIdSet;

	typedef std::string TransitionSetKey;
	typedef std::pair<TransitionSetKey, TransitionSetKey> TransitionSetKeyCouple;
	typedef std::vector<TransitionSetKeyCouple> TransitionSetKeyCoupleVector;
	typedef std::vector<TransitionSetKeyCoupleVector> TransitionSetKeyCouple2DVector;

	typedef std::vector<bool> Bitmap;

	typedef std::pair<SymbolType, size_t> RankedSymbol;
	typedef std::set<RankedSymbol> RankedAlphabet;

	typedef std::vector<size_t> PostVariant;
	typedef std::vector<PostVariant> PostVariantVector;

	class BisimulationBase;
	class BisimulationEquivalence;

	
	template <typename type> bool isMember(type item, std::set<type> &set)
	{
		return set.find(item) != set.end();
	}

	template <typename type> std::set<type> intersection(std::set<type> &left, std::set<type> &right)
	{
		std::set<type> intersect;
		set_intersection(	left.begin(),
							left.end(),
							right.begin(),
							right.end(),
							std::inserter(intersect, intersect.begin())
						);
		return intersect;
	}

	class BisimulationBase{	
		protected:
			ExplicitTreeAutCore smaller;
			ExplicitTreeAutCore bigger;

			StateSetCoupleSet post;

			TransitionVector smallerTrans;
			TransitionVector biggerTrans;

			RankedAlphabet rankedAlphabet;
			
			// cache
			size_t variant_key;
			std::unordered_map<size_t, PostVariantVector> variant_cache;
			std::unordered_map<size_t, PostVariantVector>::iterator variant_iter;

			std::string transition_key;
			std::unordered_map<std::string, TransitionIdSet> transition_cache;
			std::unordered_map<std::string, TransitionIdSet>::iterator transition_iter;

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

			void getPost(
				RankedSymbol &symbol,
				StateSetCouple &actual,
				StateSetCoupleSet &done
			);

			void serialize(std::string &key, const StateSet &set);
			void getValidTransitionsAtPos(
				const TransitionVector& automaton,
				StateSet &actual,
				SymbolType &symbol,
				size_t position
			);
			void calculatePost(
				TransitionSetKeyCoupleVector &actualTransitions,
				TransitionSetKeyCouple2DVector &doneTransitions,
				size_t rank
			);

			void generatePostVariants(size_t n, size_t k);
			static StateSet statesFromTransitions(TransitionIdSet &ids, TransitionVector &transitions);


			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);
			bool isExpandableBy(StateSet &expandee, StateSet &expandee2, StateSetCouple &expander);

	};
}
#endif