/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
 *
 *  Description:
 *    Upward inclusion for explicitly represented tree automata.
 *
 *****************************************************************************/


#include <iostream>
#include "explicit_tree_congr_incl_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardCongruence;

CongruenceBase::CongruenceBase(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger) 
	: smaller(smaller), bigger(bigger), stateCounter(0)
{}

//-----------------------------------------------------------------------------

void CongruenceBase::setSmaller(
	ExplicitTreeAutCore smaller)
{
	this->smaller = smaller;
}

//-----------------------------------------------------------------------------

void CongruenceBase::setBigger(
	ExplicitTreeAutCore bigger)
{
	this->bigger = bigger;
}

//-----------------------------------------------------------------------------

ExplicitTreeAutCore CongruenceBase::getSmaller()
{
	return this->smaller;
}

//-----------------------------------------------------------------------------

ExplicitTreeAutCore CongruenceBase::getBigger()
{
	return this->bigger;
}

//-----------------------------------------------------------------------------

void CongruenceBase::setStateCounter(
	long number)
{
	this->stateCounter = number;
}

//-----------------------------------------------------------------------------

long CongruenceBase::getStateCounter()
{
	return stateCounter;
}

//-----------------------------------------------------------------------------

void CongruenceBase::incrementStateCounter()
{
	this->stateCounter++;
}

//-----------------------------------------------------------------------------

SymbolSet CongruenceBase::getAlphabet()
{
	SymbolSet alphabet;
	for(auto transition : smaller){
		alphabet.insert(transition.GetSymbol());
	}
	return alphabet;
}

//-----------------------------------------------------------------------------

void CongruenceBase::initBitmap(
	Bitmap *bitmap,
	int size)
{
	bitmap->clear();
	for (int i = 0; i < size; i++){
		bitmap->push_back(false);
	}
}

//-----------------------------------------------------------------------------

bool CongruenceBase::checkBitmap(
	Bitmap bitmap)
{
	for (uint i = 0; i < bitmap.size(); i++){
		if (!bitmap.at(i)) {
			return false;
		}
	}
	return true;
}

//-----------------------------------------------------------------------------
//TODO method in ExplicitTreeAutCore class

bool CongruenceBase::isTerm(
	FinalStateSet set,
	StateType state)
{
	return set.find(state) != set.end();
}

//-----------------------------------------------------------------------------

bool CongruenceBase::isTerm(
	FinalStateSet finalSet,
	StateSet stateSet)
{
	for (auto state : stateSet){
		if (finalSet.find(state) != finalSet.end()){
			return true;
		}
	}
	return false;
}


//-----------------------------------------------------------------------------

void CongruenceBase::getLeaves(
	SymbolToStateSet *autLeaves, 
	const ExplicitTreeAutCore& aut)
{
	if (autLeaves == NULL){
		return;
	} 

	for (const auto& Transition : aut){
		SymbolType symbol = Transition.GetSymbol();
		if (Transition.GetChildren().empty()){
			if (autLeaves->find(symbol) == autLeaves->end()){
				StateSet *leaves = new StateSet;
				leaves->insert(Transition.GetParent());
				autLeaves->insert(std::pair<SymbolType,StateSet*>
									(symbol, leaves)
				);
			}else{
				(*autLeaves).at(symbol)->insert(Transition.GetParent());
			}
		}
	}

	return;
}

//-----------------------------------------------------------------------------

void CongruenceBase::destroyLeaves(
	SymbolToStateSet *autLeaves)
{
	if (autLeaves == NULL){
		return;
	}

	if (autLeaves->empty()) {
		return;
	}

	SymbolToStateSet::iterator it;
	for (it = autLeaves->begin(); it != autLeaves->end(); it++){
		StateSet *leaves = it->second;
		it->second = NULL;
		if (leaves != NULL){
			delete leaves;
		}
	}
	return;
}

//-----------------------------------------------------------------------------
// add all pairs of leaf states with same symbol into todo
//(smaller state , bigger state)

bool CongruenceBase::getLeafCouples(
	StateSetCoupleSet *couples, 
	SymbolToStateSet smallerLeaves, 
	SymbolToStateSet biggerLeaves)
{
	SymbolType symbol;
	SymbolToStateSet::iterator itSmaller, itBigger;;
	for (itSmaller = smallerLeaves.begin(); itSmaller != smallerLeaves.end(); itSmaller++){
		symbol = itSmaller->first;
		itBigger = biggerLeaves.find(symbol);
		if (itBigger == biggerLeaves.end()){
			return false;
		}else{
			couples->insert(StateSetCouple(*(itSmaller->second), *(itBigger->second)));
			biggerLeaves.erase(itBigger);
		}
	}

	if (!biggerLeaves.empty()){
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//vrati vsechny prechody, ktere lze provest ze stavu v done pod danym symbolem, 
//ktere obsahuji actual

void CongruenceBase::makeStep(
	ExplicitTreeAutCore automata, 
	StateSetCoupleSet done, 
	StateSetCouple actual, 
	SymbolType symbol,
	TransitionVector* transitions,
	bool position)
{
	bool reachable, relevant;
	for (auto trans : automata){
		if (trans.GetSymbol() == symbol){
			relevant = false;
			reachable = true;
			for (auto child : trans.GetChildren()){
				if (!stateInSet(done, child, position)){
					reachable = false;
					break;
				}
				if (position){
					if (actual.second.find(child) != actual.second.end()){
						relevant = true;
					}
				}else{
					if (actual.first.find(child) != actual.first.end()){
						relevant = true;
					}
				}	
			}

			if (reachable && relevant){
				transitions->push_back(trans);
			}
		}
	}
	return;
}

//-----------------------------------------------------------------------------

bool CongruenceBase::inSet(
	StateSetCoupleSet set,
	StateType state1,
	StateType state2)
{
	StateSetCoupleSet::iterator it;

	for (it = set.begin(); it != set.end(); it++){
		if (it->first.find(state1) != it->first.end()   &&
			it->second.find(state2) != it->second.end() ){
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------

bool CongruenceBase::inSets(
	StateSetCoupleSet set1,
	StateSetCoupleSet set2,
	StateType state1,
	StateType state2)
{
	StateSetCoupleSet::iterator it;

	for (it = set1.begin(); it != set1.end(); it++){
		if (it->first.find(state1) != it->first.end()   &&
			it->second.find(state2) != it->second.end() ){
			return true;
		}
	}

	for (it = set2.begin(); it != set2.end(); it++){
		if (it->first.find(state1) != it->first.end()   &&
			it->second.find(state2) != it->second.end() ){
			return true;
		}
	}
	return false;
}

//----------------------------------------------------------------------------
//find if state is in any couple in set at position (false = 0, true = 1)

bool CongruenceBase::stateInSet(
	StateSetCoupleSet set,
	StateType state,
	bool position)
{
	StateSetCoupleSet::iterator it;

	if (!position){
		for (it = set.begin(); it != set.end(); it++){
			if (it->first.find(state) != it->first.end()){
				return true;
			}
		}

	} else {
		for (it = set.begin(); it != set.end(); it++){
			if (it->second.find(state) != it->second.end()){
				return true;
			}
		}
	}

	return false;
}

//-------------------------------------------------------------------------

void CongruenceBase::findAndDeleteSubset(
	StateSetCoupleSet *set, 
	StateSetCouple couple)
{

	bool subset;
	StateSetCoupleSet::iterator it;
	for(it = set->begin(); it != set->end(); ){
		
		subset = true;

		for (auto state : it->first){
			if (couple.first.find(state) == couple.first.end()){
				subset = false;
				break;
			}
		}

		if (subset){
			for (auto state : it->second){
				if (couple.second.find(state) == couple.second.end()){
					subset = false;
					break;
				}
			}	
		}

		if (subset){
			it = set->erase(it);
		}else{
			it++;
		}
	}
}

//-------------------------------------------------------------------------
//nesmyslna funkce
bool CongruenceBase::isSubset(
	StateSetCoupleSet set, 
	StateSetCouple couple)
{
	bool subset;
	for(auto item : set){
		subset = true;
		for (auto state : couple.first){
			if (item.first.find(state) == item.first.end()){
				subset = false;
				break;
			}
		}

		if (subset){
			for (auto state : couple.second){
				if (item.second.find(state) == item.second.end()){
					subset = false;
					break;
				}
			}	
		}

		if (subset){
			return subset;
		}
	}
	return subset;
}

//-------------------------------------------------------------------------

long CongruenceBase::getStateCount(
	StateSetCoupleSet coupleSet)
{
	SetOfStateSet stateSet;

	for (auto couple : coupleSet){
		stateSet.insert(couple.first);
		stateSet.insert(couple.second);
	}
	return stateSet.size();
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
//Print todo/done

void CongruenceBase::printCouples(
	StateSetCoupleSet coupleSet)
{

	std::cout << "Couples:\n";
	for(auto couple : coupleSet){
		std::cout << "(";
		for (auto state : couple.first){
			std::cout << state << ", ";
		}
		std::cout << "), (";
		for (auto state : couple.second){
			std::cout << state << ", ";
		}
		std::cout << ")\n"; 
	}
	std::cout << "------------------\n";
}

//------------------------------------------------------------------------
//	Print Leaves: symbol -> states 	(smallerLeaves/biggerLeaves)	

void CongruenceBase::printLeaves(
	SymbolToStateSet leaves)
{

	std::cout << "--------BEGIN--------\n";
	std::cout << "Leaves:\n";
	SymbolToStateSet::iterator it;
	for (it = leaves.begin(); it != leaves.end(); it++){
		std::cout << it->first << " -> ";
		if (it->second != NULL){
			for(auto state : *(it->second) ){
				std::cout << state << ", ";
			}
		}
		std::cout << "\n";	
	}
	std::cout << "---------END---------\n";
}



//-----------------------------------------------------------------------------
//	Print transitions (smaller/biger)
	
void CongruenceBase::printTransitions(
	const VATA::ExplicitTreeAutCore& automaton)
{

	std::cout << "--------BEGIN--------\n";
	std::cout << "Transitions (ToString):\n"; 
	for (const auto& Transitions : automaton){
		std::cout << automaton.ToString(Transitions);
		std::cout << "\n";
	}
	std::cout << "---------end---------\n";
	
	std::cout << "Transitions:\n";
	std::cout << "symbol\tparent\tchildrens\n";
	for (const auto& Transitions : automaton){
		std::cout << Transitions.GetSymbol();
		std::cout << "\t";

		std::cout << Transitions.GetParent();
		std::cout << "\t";

		for (auto state : Transitions.GetChildren())
		{
			std::cout << state;
			std::cout << ", ";
		}

		std::cout << "\n";
	}
	std::cout << "---------end---------\n";

	std::cout << "Final states:\n";
	for (auto state : automaton.GetFinalStates()){
			std::cout << state;
			std::cout << ", ";
	}
	std::cout << "\n";
	std::cout << "---------END---------\n";
}
