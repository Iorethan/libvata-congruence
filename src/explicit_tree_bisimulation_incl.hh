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

#ifndef _EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_INCL_HH_
#define	_EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_INCL_HH_


#include "explicit_tree_aut_core.hh"
#include "explicit_tree_bisimulation_base.hh"
#include <vata/explicit_tree_aut.hh>

namespace ExplicitTreeUpwardBisimulation{ 
	class BisimulationInclusion : public BisimulationBase {
		private:

		public:
			BisimulationInclusion(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check();												 // Perform inclusion check.		
			bool isCoupleFinalStateEquivalent(StateSetCouple &couple);	 // Check if first macrostate is not accepting or second is accepting.
			bool isCongruenceClosureMember(StateSetCouple item);		 // Check if pair is in congruence closure.
	};

}
#endif