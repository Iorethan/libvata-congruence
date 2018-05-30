/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2013  Ondrej Lengal <ilengal@fit.vutbr.cz>
 *
 *  Description:
 *    Implementation of inclusion checking functions of ExplicitTreeAut.
 *
 *****************************************************************************/

// VATA headers
#include <vata/equiv_param.hh>

#include "explicit_tree_aut_core.hh"
#include "explicit_tree_incl_up.hh"
#include "explicit_tree_incl_down.hh"
#include "tree_incl_down.hh"
#include "down_tree_incl_fctor.hh"
#include "down_tree_opt_incl_fctor.hh"
#include "explicit_tree_bisimulation_equiv.hh"

#include <iostream>

using VATA::ExplicitTreeAutCore;
using VATA::ExplicitUpwardInclusion;

bool ExplicitTreeAutCore::CheckEquivalence(
	const ExplicitTreeAutCore&             smaller,
	const ExplicitTreeAutCore&             bigger,
	const VATA::EqParam&                 params)
{
	ExplicitTreeAutCore newSmaller;
	ExplicitTreeAutCore newBigger;
	typename AutBase::StateType states = static_cast<typename AutBase::StateType>(-1);

	newSmaller = smaller;
	newBigger = bigger;

	states = AutBase::SanitizeAutsForInclusion(newSmaller, newBigger);

	switch (params.GetOptions())
	{
		case EqParam::ANTICHAINS_UP:
		{
			assert(static_cast<typename AutBase::StateType>(-1) != states);
			return ExplicitUpwardInclusion::Check(newSmaller, newBigger,
				Util::Identity(states)) &&
                ExplicitUpwardInclusion::Check(newBigger, newSmaller,
				Util::Identity(states));
		}

		case EqParam::ANTICHAINS_DOWN:
		{
			assert(static_cast<typename AutBase::StateType>(-1) != states);
			return ExplicitUpwardInclusion::Check(newBigger, newSmaller,
				Util::Identity(states)) &&
                ExplicitUpwardInclusion::Check(newSmaller, newBigger,
				Util::Identity(states));
		}

		case EqParam::CONGRUENCE_UP:
		{
			ExplicitTreeUpwardBisimulation::BisimulationEquivalence equivalence(newSmaller, newBigger);
			return equivalence.check();
		}

		default:
		{
			throw NotImplementedException("Equivalence:\n" +
				params.toString());
		}
	}
}
