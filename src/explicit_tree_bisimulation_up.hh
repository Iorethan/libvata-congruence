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
	typedef std::set<size_t> TransitionIdSet;
	typedef std::string TransitionSetKey;

	typedef std::pair<TransitionSet, TransitionSet> TransitionSetCouple;
	typedef std::pair<std::string, std::string> TransitionSetKeyCouple;

	typedef std::vector<TransitionSetCouple> TransitionSetCoupleVector;
	typedef std::vector<TransitionSetCoupleVector> TransitionSetCouple2DVector;
	typedef std::vector<TransitionSetKeyCouple> TransitionSetKeyCoupleVector;
	typedef std::vector<TransitionSetKeyCoupleVector> TransitionSetKeyCouple2DVector;

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

			TransitionVector smallerTrans;
			TransitionVector biggerTrans;

			RankedAlphabet rankedAlphabet;
			
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
			static StateSet getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton);

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

	};

	//-----------------------------------------------------------------------------

	class BisimulationEquivalence : public BisimulationBase {
		public:
			BisimulationEquivalence(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check();

			bool areLeavesEquivalent(StateSetCoupleSet &todo);
			bool isCoupleFinalStateEquivalent(StateSetCouple &couple);

			StateSetCouple selectActual(StateSetCoupleSet& todo);

			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);

			bool isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item);
	};
	
	class BisimulationInclusion : public BisimulationBase {
		private:

		public:
			BisimulationInclusion(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check();

			bool areLeavesEquivalent(StateSetCoupleSet &todo);
			bool isCoupleFinalStateEquivalent(StateSetCouple &couple);

			StateSetCouple selectActual(StateSetCoupleSet& todo);

			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);

			bool isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item);
	};

}
#endif