/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
 *
 *  Description:
 *    Upward inclusion for explicitly represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_congr_incl_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardCongruence;

	bool ExplicitUpwardCongruence::Check(
			const ExplicitTreeAutCore&	smaller,
			const ExplicitTreeAutCore&	bigger,
			const bool	useInclusion)
	{
		bool result;
		
		if (useInclusion){
			//Inclusion Algorithm
			CongruenceInclusion inclusion(smaller, bigger);
			result = inclusion.check();
			//generated pairs:
			std::cout << inclusion.getStateCounter() << "\n";

		} else {
			//Equivalence Algorithm
			CongruenceEquivalence equivalence(smaller, bigger);
			result = equivalence.check();
			//generated pairs:
			std::cout << equivalence.getStateCounter() << "\n";
		}

		return result;
	}


	ExplicitTreeAutCore ExplicitUpwardCongruence::automataUnion(
		ExplicitTreeAutCore        aut1,
		ExplicitTreeAutCore        aut2)
	{
		ExplicitTreeAutCore automat;

		for (auto trans : aut1)
		{
			automat.AddTransition(trans);
		}

		for (auto trans : aut2)
		{
			automat.AddTransition(trans);
		}

		std::set<long unsigned int> finals;
		finals.insert(aut1.GetFinalStates().begin(), aut1.GetFinalStates().end());
		finals.insert(aut2.GetFinalStates().begin(), aut2.GetFinalStates().end());
		automat.SetStatesFinal(finals);
		
		return automat;
	}