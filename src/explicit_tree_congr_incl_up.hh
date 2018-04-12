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
#include <queue>

namespace ExplicitTreeUpwardCongruence{ 
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
	typedef std::deque <StateSetCouple> StateSetCoupleQueue;

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


	class ExplicitUpwardCongruence; 
	class CongruenceBase;
	class CongruenceInclusion;
	class CongruenceEquivalence;
}

using namespace VATA;
using namespace ExplicitTreeUpwardCongruence;

namespace ExplicitTreeUpwardCongruence{
	
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

	class ExplicitUpwardCongruence{
		
		public:
			//vstupni funkce
			static bool Check(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger,
				const bool	useInclusion
			);

			static ExplicitTreeAutCore automataUnion(
				ExplicitTreeAutCore        aut1,
				ExplicitTreeAutCore        aut2
			);
	};

	//-----------------------------------------------------------------------------

	class CongruenceBase{
		protected:
			ExplicitTreeAutCore smaller;
			ExplicitTreeAutCore bigger;
			long stateCounter;

		public:
			CongruenceBase(
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

			void setStateCounter(
				long number
			);

			long getStateCounter();

			void incrementStateCounter();

			SymbolSet getAlphabet();

		protected:
			void initBitmap(
				Bitmap *bitmap,
				int size
			);

			bool checkBitmap(
				Bitmap bitmap
			);

			bool isTerm(
				FinalStateSet set,
				StateType state
			);

			bool isTerm(
				FinalStateSet finalSet,
				StateSet stateSet
			);
		

			void getLeaves(
				SymbolToStateSet *autLeaves, 
				const ExplicitTreeAutCore& aut
			);

			void destroyLeaves(
				SymbolToStateSet *autLeaves
			);

			bool getLeafCouples(
				StateSetCoupleSet *couples, 
				SymbolToStateSet smallerLeaves, 
				SymbolToStateSet biggerLeaves
			);

			bool inSet(
				StateSetCoupleSet set,
				StateType state1,
				StateType state2
			);

			bool inSets(
				StateSetCoupleSet set1,
				StateSetCoupleSet set2,
				StateType state1,
				StateType state2
			);

			bool stateInSet(
				StateSetCoupleSet set,
				StateType state,
				bool position
			);

			void findAndDeleteSubset(
				StateSetCoupleSet *set, 
				StateSetCouple couple
			);

			void makeStep(
				ExplicitTreeAutCore automata, 
				StateSetCoupleSet done, 
				StateSetCouple actual, 
				SymbolType symbol,
				TransitionVector* transitions,
				bool position
			);

			bool isSubset(
				StateSetCoupleSet set, 
				StateSetCouple couple
			);

			long getStateCount(
				StateSetCoupleSet coupleSet
			);

			//prints

			void printLeaves(
				SymbolToStateSet leaves
			);

			void printCouples(
				StateSetCoupleSet coupleSet
			);

			void printTransitions(
				const ExplicitTreeAutCore& automaton
			);
	};

	//-----------------------------------------------------------------------------

	class CongruenceInclusion : public CongruenceBase {

		public:
			CongruenceInclusion(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);

			bool check();

		private:
			bool inClosure(
				StateSetCoupleSet set,
				StateSetCoupleQueue &queue,
				StateSet first,
				StateSet second
			);

			StateSet getNormalForm (
				StateSetCoupleSet set,
				StateSet item,
				bool position
			);

			void makeStep2(
				ExplicitTreeAutCore automata, 
				StateSetCoupleSet done, 
				StateSetCouple actual, 
				SymbolType symbol,
				TransitionVector* transitions,
				bool position,
				int i
			);

			void checkTransitions(
				StateSetCoupleSet done,
				//StateSetCoupleSet todo,
				TransitionVector transS,
				TransitionVector transB,
				Bitmap *usedB,
				StateSet *stateSetBigger
				// size_t position,
				// StateSetCouple actual
			);

			void checkBackTransitions(
				StateSetCoupleSet done,
				//StateSetCoupleSet todo,
				TransitionVector transS,
				TransitionVector transB,
				Bitmap *usedS,
				Bitmap usedB,
				StateSet *stateSetSmaller
				// size_t position,
				// StateSetCouple actual 
			);

			bool getLeafCouples(
				StateSetCoupleQueue &couples, 
				SymbolToStateSet &smallerLeaves, 
				SymbolToStateSet &biggerLeaves
			);

			bool checkInclusionOnLeaves(
				StateSetCoupleQueue &todo
			);
	};

	//-----------------------------------------------------------------------------

	class CongruenceEquivalence : public CongruenceBase {

		public:
			CongruenceEquivalence(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);

			bool check();

			RankedAlphabet getRankedAlphabet();
			StateSetCoupleSet getLeafCouples2(const RankedAlphabet &alphabet);
			StateSet getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton);
			StateSetCouple selectActual(StateSetCoupleSet& todo);
			bool isCoupleFinalStateEquivalent(StateSetCouple couple);
			StateSetCoupleSet getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet done);
			TransitionSet getValidTransitionsAtPos(
				SymbolType symbol,
				StateSet actual, 
				const ExplicitTreeAutCore& automaton,
				size_t position);
			StateSetCoupleSet calculatePost(
				TransitionSetCoupleVector &actualTransitions,
				TransitionSetCouple2DVector &doneTransitions,
				size_t rank);

		private:
			bool inClosure(
				StateSetCoupleSet set1,
				StateSetCoupleSet set2,
				StateSet first,
				StateSet second
			);

			StateSet getNormalForm (
				StateSetCoupleSet set,
				StateSet item
			);

			bool compareSets(
				StateSet set1,
				StateSet set2
			);

			void checkTransitions(
				StateSetCoupleSet done,
				TransitionVector transS,
				TransitionVector transB,
				Bitmap *usedB,
				StateSet *stateSetBigger,
				bool direction
			);
	};
	
}
#endif