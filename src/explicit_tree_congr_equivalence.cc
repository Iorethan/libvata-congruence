/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
 *
 *  Description:
 *    Upward inclusion for explicitly represented tree automata.
 *
 *****************************************************************************/
//makeStep pro kazdy symbol zvlast.
//

#include <iostream>
#include "explicit_tree_congr_incl_up.hh"

using namespace VATA;
using namespace ExplicitTreeUpwardCongruence;

CongruenceEquivalence::CongruenceEquivalence(
	const ExplicitTreeAutCore&        smaller,
	const ExplicitTreeAutCore&        bigger)
	: CongruenceBase(smaller, bigger)
	{}

RankedAlphabet CongruenceEquivalence::getRankedAlphabet()
{
	RankedAlphabet tmp;
	for(auto transition : smaller){
		RankedSymbol s;
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		tmp.insert(s);
	}
	for(auto transition : bigger){
		RankedSymbol s;
		s.first = transition.GetSymbol();
		s.second = transition.GetChildren().size();
		tmp.insert(s);
	}
	return tmp;
}

StateSetCoupleSet CongruenceEquivalence::getLeafCouples2(const RankedAlphabet &alphabet)
{
	StateSetCoupleSet tmp;
	for(auto symbol : alphabet){
		if(symbol.second == 0)
		{
			StateSetCouple c;
			c.first = getStateSetBySymbol(symbol.first, smaller);
			c.second = getStateSetBySymbol(symbol.first, bigger);
			tmp.insert(c);
		}
	}
	return tmp;
}

StateSet CongruenceEquivalence::getStateSetBySymbol(SymbolType symbol, const ExplicitTreeAutCore& automaton)
{
	StateSet tmp;
	for(auto transition : automaton){
		if(transition.GetSymbol() == symbol)
		{
			tmp.insert(transition.GetParent());
		}
	}
	return tmp;
}

StateSetCouple CongruenceEquivalence::selectActual(StateSetCoupleSet& todo)
{
	return *(todo.begin());
}

bool CongruenceEquivalence::isCoupleFinalStateEquivalent(StateSetCouple couple)
{
	FinalStateSet s = smaller.GetFinalStates();
	FinalStateSet b = bigger.GetFinalStates();
	StateSet ss(s.begin(), s.end());
	StateSet sb(b.begin(), b.end());
	return intersection(couple.first, ss).empty() ==
		intersection(couple.second, sb).empty();
}

StateSetCoupleSet CongruenceEquivalence::getPost(RankedSymbol symbol, StateSetCouple actual, StateSetCoupleSet done)
{
	TransitionSetCoupleVector actualTransitions;
	for(size_t pos = 0; pos < symbol.second; pos++)
	{
		TransitionSetCouple tmp;
		tmp.first = getValidTransitionsAtPos(symbol.first, actual.first, smaller, pos);
		tmp.second = getValidTransitionsAtPos(symbol.first, actual.second, bigger, pos);
		actualTransitions.push_back(tmp);
	}

	TransitionSetCouple2DVector doneTransitions(done.size());
	size_t i = 0;
	for(auto couple : done)
	{
		for(size_t pos = 0; pos < symbol.second; pos++)
		{
			TransitionSetCouple tmp;
			tmp.first = getValidTransitionsAtPos(symbol.first, couple.first, smaller, pos);
			tmp.second = getValidTransitionsAtPos(symbol.first, couple.second, bigger, pos);
			doneTransitions[i].push_back(tmp);
		}
		i++;
	}
	return calculatePost(actualTransitions, doneTransitions, symbol.second);
}

StateSetCoupleSet CongruenceEquivalence::calculatePost(
	TransitionSetCoupleVector &actualTransitions,
	TransitionSetCouple2DVector &doneTransitions,
	size_t rank)
{
	StateSetCoupleSet post;
	for(size_t pos = 0; pos < rank; pos++)
	{

	}
	return post;
}

TransitionSet CongruenceEquivalence::getValidTransitionsAtPos(SymbolType symbol, StateSet actual, const ExplicitTreeAutCore& automaton, size_t position)
{
	TransitionSet set;
	for(auto transition : automaton)
	{
		if(transition.GetSymbol() == symbol && isMember(transition.GetChildren()[position], actual))
		{
			set.insert(transition);
		}
	}
	return set;
}

bool CongruenceEquivalence::check()
{
	RankedAlphabet rankedAlphabet = getRankedAlphabet();
	StateSetCoupleSet done, todo = getLeafCouples2(rankedAlphabet);
	StateSetCouple actual;
	
	while(!todo.empty())
	{
		actual = selectActual(todo);
		done.insert(actual);
		if(!isCoupleFinalStateEquivalent(actual))
		{
			return false;
		}

		for(auto symbol : rankedAlphabet)
		{
			if(symbol.second != 0)
			{
				StateSetCoupleSet post = getPost(symbol, actual, done);
				for(auto next : post)
				{
					if(!isMember(next, todo))
					{
						todo.insert(next);
					}
				}
			}
		}

		todo.erase(actual);
	}
	return true;




	SymbolToStateSet smallerLeaves, biggerLeaves;
	bool termS, termB;
	StateSet stateSetSmaller, stateSetBigger;
	TransitionVector transS, transB;
	Bitmap usedS, usedB;
	//StateSetCouple actual;
	SymbolSet alphabet;

	//odstrani nadbytecne stavy
	smaller = smaller.RemoveUnreachableStates();
	smaller = smaller.RemoveUselessStates();
	bigger = bigger.RemoveUnreachableStates();
	bigger = bigger.RemoveUselessStates();

	//najde listove stavy
	getLeaves(&smallerLeaves, smaller);
	getLeaves(&biggerLeaves, bigger);

	// nema listove stavy -> abeceda je prazdna mnozina
	if (smallerLeaves.empty() || biggerLeaves.empty()){
		if (smallerLeaves.empty() && biggerLeaves.empty()){
			setStateCounter(0);
			return true;
		}else{
			setStateCounter(0);
			return false;
		}
	}
	
	//nektery ze smallerLeaves nema ekvivalent v biggerLeaves
	if (!getLeafCouples(&todo, smallerLeaves, biggerLeaves)){
		destroyLeaves(&smallerLeaves);
		destroyLeaves(&biggerLeaves);
		setStateCounter(0);
		return false;
	}

	//uz nejsou potreba
	destroyLeaves(&smallerLeaves);
	destroyLeaves(&biggerLeaves);

	//najde vstupni abecedu
	alphabet = getAlphabet();

	while (!todo.empty()){

		actual = *(todo.begin());
		todo.erase(actual);
		if (isSubset(done, actual)){
			continue;
		}
		//findAndDeleteSubset(&done, actual);
		done.insert(actual);
		incrementStateCounter();

		for (SymbolType symbol : alphabet){

			transS.clear();
			transB.clear();
			usedS.clear();
			usedB.clear();

			//najde prechody, do kterych se lze dostat a obsahuji actual
			makeStep(smaller, done, actual, symbol, &transS, false);
			makeStep(bigger, done, actual, symbol, &transB, true);

			//nektery z automatu nema prechody
			if (transS.empty() || transB.empty()){
				if (transS.empty() && transB.empty()){
					continue;
				}else{
					setStateCounter(done.size());
					//std::cout << getStateCount(done) << "\n";
					return false;
				}
			}

			initBitmap(&usedS, transS.size());
			initBitmap(&usedB, transB.size());

			stateSetSmaller.clear();
			stateSetBigger.clear();

			checkTransitions(done, transS, transB, &usedB, &stateSetBigger, true);
			checkTransitions(done, transB, transS, &usedS, &stateSetSmaller, false);

			if (!inClosure(done, todo, stateSetSmaller, stateSetBigger)){
				
				termS = isTerm(smaller.GetFinalStates(), stateSetSmaller);
				termB = isTerm(bigger.GetFinalStates(), stateSetBigger);
				
				if (termS == termB){	
					StateSetCouple item = StateSetCouple(stateSetSmaller, stateSetBigger);
					todo.insert(item);
				} else {
					setStateCounter(done.size());
					//std::cout << getStateCount(done) << "\n";
					return false;
				}
			}

		}	

		//check bitmap
		if (!checkBitmap(usedS) || !checkBitmap(usedB)){
			setStateCounter(done.size());
			//std::cout << getStateCount(done) << "\n";
			return false;
		}
	}

	setStateCounter(done.size());
	//std::cout << getStateCount(done) << "\n";
	return true;
}

//-----------------------------------------------------------------------------

void CongruenceEquivalence::checkTransitions(
	StateSetCoupleSet done,
	TransitionVector transS,
	TransitionVector transB,
	Bitmap *usedB,
	StateSet *stateSetBigger,
	bool direction)
{
	bool equivalent;
	for (uint counterS = 0; counterS < transS.size(); counterS++){
		auto transitionS = transS.at(counterS);

		for (uint counterB = 0; counterB < transB.size(); counterB++){
			auto transitionB = transB.at(counterB);

			if (!usedB->at(counterB) ){

				// check if transitions are equivalent
				equivalent = true;
				for (uint i = 0; i < transitionS.GetChildren().size(); i++){
					if (!inSet(	done, 
								direction ? transitionS.GetChildren().at(i) : transitionB.GetChildren().at(i),
								direction ? transitionB.GetChildren().at(i) : transitionS.GetChildren().at(i)
								)){
						equivalent = false;
						break;
					}
				}

				if (equivalent){
					usedB->at(counterB) = true;
					stateSetBigger->insert(transitionB.GetParent());
				}
			}
		}
	}						
}

//-----------------------------------------------------------------------------

bool CongruenceEquivalence::inClosure(
	StateSetCoupleSet set1,
	StateSetCoupleSet set2,
	StateSet first,
	StateSet second)
{
	set1.insert(set2.begin(), set2.end());
	return compareSets(getNormalForm(set1, first), getNormalForm(set1, second));
}

//-----------------------------------------------------------------------------

StateSet CongruenceEquivalence::getNormalForm (
	StateSetCoupleSet set,
	StateSet item)
{

	StateSetCoupleSet deletingSet = set;
	StateSet normalForm = item;
	bool found;
	while (!deletingSet.empty()){
		for (auto couple : deletingSet){ //pro vsechny pary v mnozine
			found = true;
			for (auto state : couple.first){ //pro vsechny stavy v prvnim z paru
				if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
					found = false;
					break;
				}
			}

			if (found){	//pokud odpovidal
				normalForm.insert(couple.second.begin(), couple.second.end());
				deletingSet.erase(couple);
				break;

			}else{ //pokud ne, hledej v druhem z paru
				found = true;
				for (auto state : couple.second){ //pro vsechny stavy v druhem z paru
					if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
						found = false;
						break;
					}
				}

				if (found){
					normalForm.insert(couple.first.begin(), couple.first.end());
					deletingSet.erase(couple);
					break;
				}
			}
		}

		if (!found){
			break;
		}
	}
	return normalForm;

}

//-----------------------------------------------------------------------------

bool CongruenceEquivalence::compareSets(
	StateSet set1,
	StateSet set2)
{
	if (set1.size() != set2.size()){
		return false;
	}

	StateSet::iterator iterator1, iterator2;
	iterator1 = set1.begin();
	iterator2 = set2.begin();
	while (iterator1 != set1.end()){
		if (*iterator1 != *iterator2){
			return false;
		}
		iterator1++;
		iterator2++;
	}

	return true;
}

