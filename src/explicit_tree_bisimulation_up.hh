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

namespace ExplicitTreeUpwardBisimulation{ 
	using namespace VATA;

	using StateType      = ExplicitTreeAutCore::StateType;
	using SymbolType     = ExplicitTreeAutCore::SymbolType;
	using FinalStateSet	 = ExplicitTreeAutCore::FinalStateSet;
	using Transition	 = ExplicitTreeAut::Transition;


	//mnozina stavu
	typedef std::set <VATA::ExplicitTreeAutCore::StateType> StateSet;

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


	class BisimulationBase;
	class BisimulationEquivalence;

	
	template <typename type> bool isMember(type item, std::set<type> set)
	{
		return set.find(item) != set.end();
	}

	template <typename type>  std::set<type> intersection(std::set<type> left, std::set<type> right)
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

	template <typename type>  std::set<type> set_union(std::set<type> left, std::set<type> right)
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
			long stateCounter;

		public:
			BisimulationBase(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);

			void setSmaller(
				ExplicitTreeAutCore smaller
			);

			void setBigger(
				ExplicitTreeAutCore bigger
			);

			ExplicitTreeAutCore getSmaller();

			ExplicitTreeAutCore getBigger();
			
			bool check(
				const bool					useCache,
				const bool					useCongruence
			);
	};

	//-----------------------------------------------------------------------------

	class BisimulationEquivalence : public BisimulationBase {

		public:
			BisimulationEquivalence(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check(
				const bool					useCache,
				const bool					useCongruence
			);

			RankedAlphabet getRankedAlphabet();

			StateSetCoupleSet getLeafCouples(const RankedAlphabet &alphabet);

			StateSet getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton);

			StateSetCouple selectActual(StateSetCoupleSet& todo);

			bool isCoupleFinalStateEquivalent(StateSetCouple couple);

			StateSetCoupleSet getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet &done);

			TransitionSet getValidTransitionsAtPos(
				SymbolType symbol,
				StateSet actual, 
				const ExplicitTreeAutCore& automaton,
				size_t position,
				int index);

			StateSetCoupleSet calculatePost(
				TransitionSetCoupleVector &actualTransitions,
				TransitionSetCouple2DVector &doneTransitions,
				size_t rank);

			PostVariantVector generatePostVariants(size_t n, size_t k);

			StateSetCouple statesFromTransitions(TransitionSet &sml, TransitionSet &bgr);

			bool isCongruenceClosureMember(StateSetCouple item, StateSetCoupleSet &set);

			bool isExpandableBy(StateSet &first, StateSet &second, StateSetCouple &item);
	};
	
}
#endif