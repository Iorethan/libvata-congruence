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

#ifndef _EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_EQUIV_HH_
#define	_EXPLICIT_TREE_UPWARD_BISIMULATION_EXPLICIT_TREE_BISIMULATION_EQUIV_HH_


#include "explicit_tree_aut_core.hh"
#include "explicit_tree_bisimulation_base.hh"
#include <vata/explicit_tree_aut.hh>

namespace ExplicitTreeUpwardBisimulation{ 
	class BisimulationEquivalence : public BisimulationBase {
		public:
			BisimulationEquivalence(
				const ExplicitTreeAutCore&        smaller,
				const ExplicitTreeAutCore&        bigger
			);
			
			bool check();												// Perform equivalence check.		
			bool isCoupleFinalStateEquivalent(StateSetCouple &couple);	// Check if both or neither of macrostates contain final states.
			bool isCongruenceClosureMember(StateSetCouple item);		// Check if pair is in congruence closure.
	};
}
#endif