/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Equivalence using upward bisimulation up to congruence for explicitly
 *    represented tree automata.
 *
 *****************************************************************************/

#include "explicit_tree_bisimulation_equiv.hh"

unsigned long pair_cnt1, true_cnt1;

using namespace VATA;
using namespace ExplicitTreeUpwardBisimulation;

BisimulationEquivalence::BisimulationEquivalence(
	const ExplicitTreeAutCore&        _smaller,
	const ExplicitTreeAutCore&        _bigger)
	: BisimulationBase(_smaller, _bigger)
	{
	}

bool BisimulationEquivalence::isCoupleFinalStateEquivalent(StateSetCouple &couple)
{
	// if(couple.first.empty() != couple.second.empty())
	// {
	// 	return false;
	// }

	StateSet set_s, set_b;
	set_intersection
	(
		couple.first.begin(), couple.first.end(),
		smaller.GetFinalStates().begin(), smaller.GetFinalStates().end(),
		std::inserter(set_s, set_s.begin())
	);
	
	set_intersection
	(
		couple.second.begin(), couple.second.end(),
		bigger.GetFinalStates().begin(), bigger.GetFinalStates().end(),
		std::inserter(set_b, set_b.begin())
	);
	return set_s.empty() == set_b.empty();
}

std::string set_to_string(StateSet set)
{
	std::string result = "";
	for (auto item : set)
	{
		result += std::to_string(item) + ",";
	}
	if(result.length() > 0)
	{
		result.pop_back();
	}
	return "(" + result + ")";
}

std::string set_couple_to_string(StateSetCouple couple)
{
	std::string first = set_to_string(couple.first);
	std::string second = set_to_string(couple.second);
	return "[" + first + "," + second + "]";
}

std::string set_couple_set_to_string(StateSetCoupleSet set)
{
	std::string result = "";
	for (auto couple : set)
	{
		result += set_couple_to_string(couple) + ",";
	}
	if(result.length() > 0)
	{
		result.pop_back();
	}
	return "{" + result + "}";
}

void print_set_couple(StateSetCouple couple)
{
	std::cout << set_couple_to_string(couple) << std::endl;
}

void print_set_couple_set(StateSetCoupleSet set)
{
	std::cout << set_couple_set_to_string(set) << std::endl;
}

bool BisimulationEquivalence::check()
{
	StateSetCoupleSet done, todo, knownPairs, superPost;
	StateSetCouple actual;
	getLeafCouples(todo);
	pruneRankedAlphabet();
	
	for(auto transition : smaller){
		std::cout << smaller.ToString(transition) << std::endl;
	}	
	for(auto transition : bigger){
		std::cout << bigger.ToString(transition) << std::endl;
	}

	if(!areLeavesEquivalent(todo))
	{
		return false;
	}

	int i = 0;
	while(!todo.empty())
	{
		i++;
		pair_cnt1++;
		actual = *todo.begin();

		std::cout << "STEP " << i << std::endl;

		std::cout << "todo:";
		print_set_couple_set(todo);

		std::cout << "actual:";
		print_set_couple(actual);
		todo.erase(actual);
		knownPairs.insert(actual);

		if(!isCoupleFinalStateEquivalent(actual))
		{
			return false;
		}

		if(isCongruenceClosureMember(actual, done))
		{
			std::cout << "in closure, skipping: ";
			print_set_couple(actual);
			std::cout << std::endl << std::endl;
			continue;
		}
		done.insert(actual);

		std::cout << "done:";
		print_set_couple_set(done);

		superPost.clear();
		for(auto symbol : rankedAlphabet)
		{
			getPost(symbol, actual, done);
			std::cout << symbol.first << ": ";
			print_set_couple_set(post);
			for(auto next : post)
			{
				superPost.insert(next);
				if(knownPairs.find(next) == knownPairs.end())
					todo.insert(next);
			}
		}

		std::cout << "post:";
		print_set_couple_set(superPost);

		std::cout << std::endl << std::endl;
	}
	return true;
}
