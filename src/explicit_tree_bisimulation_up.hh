/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
 *
 *  Description:
 *    Upward inclusion for explicitly represented tree automata.
 *
 *****************************************************************************/

#ifndef _EXPLICIT_TREE_CONGR_INCL_UP_EXPLICIT_TREE_CONGR_INCL_UP_HH_
#define	_EXPLICIT_TREE_CONGR_INCL_UP_EXPLICIT_TREE_CONGR_INCL_UP_HH_


#include "explicit_tree_aut_core.hh"
#include <vata/explicit_tree_aut.hh>

// #include <map>
// #include <tuple>

namespace ExplicitTreeUpwardBisimulation{ 
	using namespace VATA;

	using StateType      = ExplicitTreeAutCore::StateType;
	using SymbolType     = ExplicitTreeAutCore::SymbolType;
	using FinalStateSet	 = ExplicitTreeAutCore::FinalStateSet;
	using Transition	 = ExplicitTreeAut::Transition;


	//mnozina stavu
	typedef std::set <StateType> StateSet;

	//dvojice mnozin stavu
	typedef std::pair <StateSet, StateSet> StateSetCouple;

	//mnozina dvojic mnozin stavu
	typedef std::set <StateSetCouple> StateSetCoupleSet;

	//mapa symbol na mnozinu stavu
	typedef std::map <VATA::ExplicitTreeAutCore::SymbolType, 
					 StateSet*> SymbolToStateSet;

	//vector prechodu
	typedef std::vector<Transition> TransitionVector;
	typedef std::set<Transition> TransitionSet;
	typedef std::pair<TransitionSet, TransitionSet> TransitionSetCouple;
	typedef std::vector<TransitionSetCouple> TransitionSetCoupleVector;
	typedef std::vector<TransitionSetCoupleVector> TransitionSetCouple2DVector;

	//bitmap
	typedef std::vector<bool> Bitmap;

	//alphabet
	typedef std::set<SymbolType> SymbolSet;

	//mnozina mnozin stavu
	typedef std::set<StateSet> SetOfStateSet;


	typedef std::pair<SymbolType, size_t> RankedSymbol;
	typedef std::set<RankedSymbol> RankedAlphabet;

	typedef std::vector<size_t> PostVariant;
	typedef std::vector<PostVariant> PostVariantVector;

	struct s1 {
		SymbolType t;
		StateSet s;
		size_t sz;
		int i;
	};

	class BisimulationBase;
	class BisimulationEquivalence;

	
	template <typename type> bool isMember(type item, std::set<type> &set)
	{
		return set.find(item) != set.end();
	}

	template <typename type>  std::set<type> intersection(std::set<type> &left, std::set<type> &right)
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

	template <typename type>  std::set<type> set_union(std::set<type> &left, std::set<type> &right)
	{
		std::set<type> intersect;
		set_union(	left.begin(),
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
			
			size_t variant_key;
			std::unordered_map<size_t, PostVariantVector> variant_cache;
			std::unordered_map<size_t, PostVariantVector>::iterator variant_iter;

			std::string transition_key;
			std::unordered_map<std::string, TransitionSet> transition_cache;
			std::unordered_map<std::string, TransitionSet>::iterator transition_iter;

		public:
			BisimulationBase(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			virtual ~BisimulationBase() {}
			
			virtual bool check(
				const bool					useCache,
				const bool					useCongruence,
				const bool					beLax
			) = 0;

			RankedAlphabet getRankedAlphabet();
			static void pruneRankedAlphabet(RankedAlphabet &alphabet);

			void getLeafCouples(const RankedAlphabet &alphabet, StateSetCoupleSet &set);
			static StateSet getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton);

			void getPost(
				RankedSymbol symbol,
				StateSetCouple actual,
				StateSetCoupleSet &done);
			void getPostCached(
				RankedSymbol &symbol,
				StateSetCouple &actual,
				StateSetCoupleSet &done);

			void calculatePost(
				TransitionSetCoupleVector &actualTransitions,
				TransitionSetCouple2DVector &doneTransitions,
				size_t rank);
			static StateSet statesFromTransitions(TransitionSet &transitions);
			void generatePostVariants(size_t n, size_t k);

			TransitionSet getValidTransitionsAtPos(
				SymbolType symbol,
				StateSet actual, 
				const ExplicitTreeAutCore& automaton,
				size_t position);
			void getValidTransitionsAtPosCached(
				const ExplicitTreeAutCore& automaton,
				StateSet &actual,
				SymbolType &symbol,
				size_t position
			);
	};

	//-----------------------------------------------------------------------------

	class BisimulationEquivalence : public BisimulationBase {
		private:
			std::unordered_map<std::string, bool> expandable_cache;
			// std::map<std::tuple<StateSet, StateSet, StateSetCouple>, bool> expandable_cache;

		public:
			BisimulationEquivalence(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check(
				const bool					useCache,
				const bool					useCongruence,
				const bool					beLax
			);

			bool areLeavesEquivalent(StateSetCoupleSet &todo);
			bool isCoupleFinalStateEquivalent(StateSetCouple &couple);

			StateSetCouple selectActual(StateSetCoupleSet& todo);

			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);
			bool isCongruenceClosureMemberCachedStrict(StateSetCouple item, StateSetCoupleSet &set);
			bool isCongruenceClosureMemberCachedLax(StateSetCouple item, StateSetCoupleSet &set);

			bool isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item);
			bool isExpandableByCached(StateSet &first, StateSet &second, StateSetCouple &item);
	};
	
}
#endif