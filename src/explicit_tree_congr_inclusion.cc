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

CongruenceInclusion::CongruenceInclusion(
	const ExplicitTreeAutCore&        _smaller,
	const ExplicitTreeAutCore&        _bigger)
	: CongruenceBase(_smaller, _bigger)
	{
		//odstrani nadbytecne stavy
		smaller = smaller.RemoveUnreachableStates();
		smaller = smaller.RemoveUselessStates();
		bigger = bigger.RemoveUnreachableStates();
		bigger = bigger.RemoveUselessStates();
	}

bool CongruenceInclusion::check()
{
	StateSetCoupleSet done;
	StateSetCoupleQueue todo;
	bool termS, termB;
	StateSet stateSetSmaller, stateSetBigger;
	TransitionVector transS, transB;
	Bitmap usedS, usedB;
	StateSetCouple actual;
	SymbolSet alphabet;
	
	if (!checkInclusionOnLeaves(todo))
	{
		setStateCounter(0);
		return false;
	}

	//najde vstupni abecedu
	alphabet = getAlphabet();

	while (!todo.empty()){

		actual = todo.back();
		todo.pop_back();

		if (isSubset(done, actual)){
			continue;
		}
		
		done.insert(actual);
		incrementStateCounter();

		for (SymbolType symbol : alphabet){

			transS.clear();
			transB.clear();

			//najde prechody, do kterych se lze dostat a obsahuji actual
			makeStep(smaller, done, actual, symbol, &transS, false);
			makeStep(bigger, done, actual, symbol, &transB, true);

			//nektery z automatu nema prechody
			if (transS.empty()){
				continue;
			} else {
				if (transB.empty()){
					setStateCounter(done.size());
					//std::cout << getStateCount(done) << "\n";
					return false;
				}
			}
			
			initBitmap(&usedS, transS.size());
			initBitmap(&usedB, transB.size());

			stateSetSmaller.clear();
			stateSetBigger.clear();

			checkTransitions(done, transS, transB, &usedB, &stateSetBigger);
			checkBackTransitions(done, transS, transB, &usedS, usedB, &stateSetSmaller);

			if (!inClosure(done, todo, stateSetSmaller, stateSetBigger)){
				termS = isTerm(smaller.GetFinalStates(), stateSetSmaller);
				termB = isTerm(bigger.GetFinalStates(), stateSetBigger);
				
				if (termS == termB){	
					StateSetCouple item = StateSetCouple(stateSetSmaller, stateSetBigger);
					todo.push_back(item);
				} else {
					setStateCounter(done.size());
					//std::cout << getStateCount(done) << "\n";
					return false;
				}
			}

			//check bitmap
			if (!checkBitmap(usedS)){
				setStateCounter(done.size());
				//std::cout << getStateCount(done);
				return false;
			}
		}	
	}

	setStateCounter(done.size());
	//std::cout << getStateCount(done) << "\n";
	return true;
}

//-----------------------------------------------------------------------------

void CongruenceInclusion::checkTransitions(
	StateSetCoupleSet done,
	TransitionVector transS,
	TransitionVector transB,
	Bitmap *usedB,
	StateSet *stateSetBigger)
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
								transitionS.GetChildren().at(i),
								transitionB.GetChildren().at(i)
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

void CongruenceInclusion::checkBackTransitions(
	StateSetCoupleSet done,
	TransitionVector transS,
	TransitionVector transB,
	Bitmap *usedS,
	Bitmap usedB,
	StateSet *stateSetSmaller)
{
	bool equivalent;
	for (uint counterB = 0; counterB < transB.size(); counterB++){
		auto transitionB = transB.at(counterB);
		
		if (usedB.at(counterB)){

			for (uint counterS = 0; counterS < transS.size(); counterS++){
				auto transitionS = transS.at(counterS);			

				if (!usedS->at(counterS)){

					// check if transitions are equivalent
					equivalent = true;
					for (uint i = 0; i < transitionS.GetChildren().size(); i++){
						if (!inSet(	done, 
									transitionS.GetChildren().at(i),
									transitionB.GetChildren().at(i)
									)){
							equivalent = false;
							break;
						}
					}

					if (equivalent){
						usedS->at(counterS) = true;
						stateSetSmaller->insert(transitionS.GetParent());
					}
				}
			}
		}
	}						
}

//-----------------------------------------------------------------------------

bool CongruenceInclusion::inClosure(
	StateSetCoupleSet set,
	StateSetCoupleQueue &queue,
	StateSet first,
	StateSet second)
{
	set.insert(queue.begin(), queue.end());
	return getNormalForm(set, first, false) == getNormalForm(set, second, true);
}

//-----------------------------------------------------------------------------

StateSet CongruenceInclusion::getNormalForm (
	StateSetCoupleSet set,
	StateSet item,
	bool position)
{
	bool found;
	while (!set.empty()){
		for (auto couple : set){ //pro vsechny pary v mnozine
			found = true;

			for (auto state : couple.second){ //pro vsechny stavy v prvnim z paru
				if (item.find(state) == item.end()){ //kdyz item neobsahuje stav
					found = false;
					break;
				}
			}
			if (found){	//pokud odpovidal
				item.insert(couple.first.begin(), couple.first.end());
				set.erase(couple);
				break;
			}
			
			for (auto state : couple.first){ //pro vsechny stavy v prvnim z paru
				if (item.find(state) == item.end()){ //kdyz item neobsahuje stav
					found = false;
					break;
				}
			}
			if (found){	//pokud odpovidal
				item.insert(couple.second.begin(), couple.second.end());
				set.erase(couple);
				break;
			}
		}
			
		if (!found){
			break;
		}
	}
	return item;
}

//-----------------------------------------------------------------------------
// add all pairs of leaf states with same symbol into todo
//(smaller state , bigger state)

bool CongruenceInclusion::getLeafCouples(
	StateSetCoupleQueue &couples, 
	SymbolToStateSet &smallerLeaves, 
	SymbolToStateSet &biggerLeaves)
{
	SymbolToStateSet::iterator itBigger;;
	for (auto smallerLeaf : smallerLeaves){
		itBigger = biggerLeaves.find(smallerLeaf.first);
		if (itBigger == biggerLeaves.end()){
			return false;
		}else{
			couples.push_back(StateSetCouple(*(smallerLeaf.second), *(itBigger->second)));
		}
	}

	return true;
}

bool CongruenceInclusion::checkInclusionOnLeaves(
	StateSetCoupleQueue &todo)
{	
	SymbolToStateSet smallerLeaves, biggerLeaves;

	//najde listove stavy
	getLeaves(&smallerLeaves, smaller);
	getLeaves(&biggerLeaves, bigger);

	//nektery ze smallerLeaves nema ekvivalent v biggerLeaves
	return getLeafCouples(todo, smallerLeaves, biggerLeaves);
}












// /*****************************************************************************
//  *  VATA Tree Automata Library
//  *
//  *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
//  *
//  *  Description:
//  *    Upward inclusion for explicitly represented tree automata.
//  *
//  *****************************************************************************/


// #include <iostream>
// #include "zufan_explicit_tree_congr_incl_up.hh"

// using namespace VATA;
// using namespace ExplicitTreeUpwardCongruence;

// CongruenceInclusion::CongruenceInclusion(
// 	const ExplicitTreeAutCore&        smaller,
// 	const ExplicitTreeAutCore&        bigger)
// 	: CongruenceBase(smaller, bigger)
// 	{}

// bool CongruenceInclusion::check()
// {
// 	StateSetCoupleSet done, todo, checked;
// 	SymbolToStateSet smallerLeaves, biggerLeaves;
// 	bool termS, termB;
// 	StateSet stateSetSmaller, stateSetBigger;
// 	TransitionVector transS, transB;
// 	Bitmap usedS, usedB;
// 	StateSetCouple actual;
// 	SymbolSet alphabet;

// 	//odstrani nadbytecne stavy
// 	smaller = smaller.RemoveUnreachableStates();
// 	smaller = smaller.RemoveUselessStates();
// 	bigger = bigger.RemoveUnreachableStates();
// 	bigger = bigger.RemoveUselessStates();

// 	//najde listove stavy
// 	getLeaves(&smallerLeaves, smaller);
// 	getLeaves(&biggerLeaves, bigger);

// 	// nema listove stavy -> abeceda je prazdna mnozina
// 	if (smallerLeaves.empty()){
// 		destroyLeaves(&smallerLeaves);
// 		destroyLeaves(&biggerLeaves);
// 		setStateCounter(0);
// 		return true;
// 	}

// 	//nektery ze smallerLeaves nema ekvivalent v biggerLeaves
// 	if (!getLeafCouples(&todo, smallerLeaves, biggerLeaves)){
// 		destroyLeaves(&smallerLeaves);
// 		destroyLeaves(&biggerLeaves);
// 		setStateCounter(0);
// 		return false;
// 	}

// 	//uz nejsou potreba
// 	destroyLeaves(&smallerLeaves);
// 	destroyLeaves(&biggerLeaves);

// 	//najde vstupni abecedu
// 	alphabet = getAlphabet();

// 	while (!todo.empty()){

// 		actual = *(todo.begin());
// 		todo.erase(actual);
// 		if (isSubset(done, actual)){
// 			continue;
// 		}
// 		//findAndDeleteSubset(&done, actual);
// 		done.insert(actual);
// 		incrementStateCounter();



// 			// std::cout << "#######################################\ncounter:" << getStateCounter() << "\n";
// 			// std::cout << "todo:\n";
// 			// printCouples(todo);
// 			// std::cout << "done:\n";
// 			// printCouples(done);
// 			// std::cout << "actual:\n";
// 			// std::cout << "(";
// 			// for (auto state : actual.first){
// 				// std::cout << state << ", ";
// 			// }
// 			// std::cout << "), (";
// 			// for (auto state : actual.second){
// 				// std::cout << state << ", ";
// 			// }
// 			// std::cout << ")\n"; 


// 		for (SymbolType symbol : alphabet){

// 			transS.clear();
// 			transB.clear();

// 			//najde prechody, do kterych se lze dostat a obsahuji actual
// 			makeStep(smaller, done, actual, symbol, &transS, false);
// 			makeStep(bigger, done, actual, symbol, &transB, true);

// 			//nektery z automatu nema prechody
// 			if (transS.empty()){
// 				continue;
// 			} else {
// 				if (transB.empty()){
// 					setStateCounter(done.size());
// 					//std::cout << getStateCount(done) << "\n";
// 					return false;
// 				}
// 			}
			
// 			size_t arity = transB[0].GetChildrenSize();
// 			for(uint position = 0; position < arity; position++)
// 			{
// 				transS.clear();
// 				transB.clear();

// 				//najde prechody, do kterych se lze dostat a obsahuji actual
// 				makeStep2(smaller, done, actual, symbol, &transS, false, position);
// 				makeStep2(bigger, done, actual, symbol, &transB, true, position);

// 				//nektery z automatu nema prechody
// 				if (transS.empty()){
// 					continue;
// 				} else {
// 					if (transB.empty()){
// 						setStateCounter(done.size());
// 						//std::cout << getStateCount(done) << "\n";
// 						return false;
// 					}
// 				}

// 				initBitmap(&usedS, transS.size());
// 				initBitmap(&usedB, transB.size());

// 				stateSetSmaller.clear();
// 				stateSetBigger.clear();

// 				checkTransitions(done, todo, transS, transB, &usedB, &stateSetBigger);
// 				checkBackTransitions(done, todo, transS, transB, &usedS, usedB, &stateSetSmaller);

// 				if (!inClosure(done, todo, stateSetSmaller, stateSetBigger)){
// 					termS = isTerm(smaller.GetFinalStates(), stateSetSmaller);
// 					termB = isTerm(bigger.GetFinalStates(), stateSetBigger);
					
// 					if (termS == termB){	
// 						StateSetCouple item = StateSetCouple(stateSetSmaller, stateSetBigger);
// 						todo.insert(item);
// 					} else {
// 						setStateCounter(done.size());
// 						//std::cout << getStateCount(done) << "\n";
// 						return false;
// 					}
// 				}

// 				//check bitmap
// 				if (!checkBitmap(usedS)){
// 					setStateCounter(done.size());
// 					//std::cout << getStateCount(done);
// 					return false;
// 				}

// 			}
// 		}	
// 	}

// 	setStateCounter(done.size());
// 	//std::cout << getStateCount(done) << "\n";
// 	return true;
// }

// //-----------------------------------------------------------------------------

// //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// void CongruenceInclusion::makeStep2(
// 	ExplicitTreeAutCore automata, 
// 	StateSetCoupleSet done, 
// 	StateSetCouple actual, 
// 	SymbolType symbol,
// 	TransitionVector* transitions,
// 	bool position,
// 	int i)
// {
// 	for (auto trans : automata){
// 		if (trans.GetSymbol() == symbol){
// 			StateSetCoupleSet st;
// 			st.insert(actual);
// 			if (stateInSet(st, trans.GetChildren()[i], position)){
// 				transitions->push_back(trans);
// 			}
// 		}
// 	}
// 	return;
// }
// //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// //-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

// void CongruenceInclusion::checkTransitions(
// 	StateSetCoupleSet done,
// 	StateSetCoupleSet todo,
// 	TransitionVector transS,
// 	TransitionVector transB,
// 	Bitmap *usedB,
// 	StateSet *stateSetBigger)
// {
// 	bool equivalent;
// 	for (uint counterS = 0; counterS < transS.size(); counterS++){
// 		auto transitionS = transS.at(counterS);

// 		for (uint counterB = 0; counterB < transB.size(); counterB++){
// 			auto transitionB = transB.at(counterB);

// 			if (!usedB->at(counterB) ){

// 				// check if transitions are equivalent
// 				equivalent = true;
// 				for (uint i = 0; i < transitionS.GetChildren().size(); i++){
// 						StateSet fst, snd;
// 						fst.insert(transitionS.GetChildren().at(i));
// 						snd.insert(transitionB.GetChildren().at(i));
// 						if (!inClosure(	done, todo,
// 									fst,
// 									snd
// 									)){
// 						equivalent = false;
// 						break;
// 					}
// 				}

// 				if (equivalent){
// 					usedB->at(counterB) = true;
// 					stateSetBigger->insert(transitionB.GetParent());
// 				}
// 			}
// 		}
// 	}						
// }

// //-----------------------------------------------------------------------------

// void CongruenceInclusion::checkBackTransitions(
// 	StateSetCoupleSet done,
// 	StateSetCoupleSet todo,
// 	TransitionVector transS,
// 	TransitionVector transB,
// 	Bitmap *usedS,
// 	Bitmap usedB,
// 	StateSet *stateSetSmaller)
// {
// 	bool equivalent;
// 	for (uint counterB = 0; counterB < transB.size(); counterB++){
// 		auto transitionB = transB.at(counterB);
		
// 		if (usedB.at(counterB)){

// 			for (uint counterS = 0; counterS < transS.size(); counterS++){
// 				auto transitionS = transS.at(counterS);			

// 				if (!usedS->at(counterS)){

// 					// check if transitions are equivalent
// 					equivalent = true;
// 					for (uint i = 0; i < transitionS.GetChildren().size(); i++){
// 						StateSet fst, snd;
// 						fst.insert(transitionS.GetChildren().at(i));
// 						snd.insert(transitionB.GetChildren().at(i));
// 						if (!inClosure(	done, todo,
// 									fst,
// 									snd
// 									)){
// 							equivalent = false;
// 							break;
// 						}
// 					}

// 					if (equivalent){
// 						usedS->at(counterS) = true;
// 						stateSetSmaller->insert(transitionS.GetParent());
// 					}
// 				}
// 			}
// 		}
// 	}						
// }

// //-----------------------------------------------------------------------------

// bool CongruenceInclusion::inClosure(
// 	StateSetCoupleSet set1,
// 	StateSetCoupleSet set2,
// 	StateSet first,
// 	StateSet second)
// {

// 	set1.insert(set2.begin(), set2.end());
// 	return getNormalForm(set1, first, false) == getNormalForm(set1, second, true);
// }

// //-----------------------------------------------------------------------------

// StateSet CongruenceInclusion::getNormalForm (
// 	StateSetCoupleSet set,
// 	StateSet item,
// 	bool position)
// {

// 	StateSetCoupleSet deletingSet = set;
// 	StateSet normalForm = item;
// 	bool found;
// 	while (!deletingSet.empty()){
// 		for (auto couple : deletingSet){ //pro vsechny pary v mnozine
// 			found = true;
// 			if (position){
// 				for (auto state : couple.second){ //pro vsechny stavy v prvnim z paru
// 					if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
// 						found = false;
// 						break;
// 					}
// 				}
// 				if (found){	//pokud odpovidal
// 					normalForm.insert(couple.first.begin(), couple.first.end());
// 					deletingSet.erase(couple);
// 					break;
// 				}
// 			} else {
// 				for (auto state : couple.first){ //pro vsechny stavy v prvnim z paru
// 					if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
// 						found = false;
// 						break;
// 					}
// 				}
// 				if (found){	//pokud odpovidal
// 					normalForm.insert(couple.second.begin(), couple.second.end());
// 					deletingSet.erase(couple);
// 					break;
// 				}
// 			}
// 		}
			
// 		if (!found){
// 			break;
// 		}
// 	}
// 	return normalForm;
// }

// //-----------------------------------------------------------------------------

// bool CongruenceInclusion::compareSets(
// 	StateSet set1,
// 	StateSet set2)
// {
// 	if (set1.size() != set2.size()){
// 		return false;
// 	}

// 	StateSet::iterator iterator1, iterator2;
// 	iterator1 = set1.begin();
// 	iterator2 = set2.begin();
// 	while (iterator1 != set1.end()){
// 		if (*iterator1 != *iterator2){
// 			return false;
// 		}
// 		iterator1++;
// 		iterator2++;
// 	}

// 	return true;
// }

// //-----------------------------------------------------------------------------
// // add all pairs of leaf states with same symbol into todo
// //(smaller state , bigger state)

// bool CongruenceInclusion::getLeafCouples(
// 	StateSetCoupleSet *couples, 
// 	SymbolToStateSet smallerLeaves, 
// 	SymbolToStateSet biggerLeaves)
// {
// 	SymbolType symbol;
// 	SymbolToStateSet::iterator itSmaller, itBigger;;
// 	for (itSmaller = smallerLeaves.begin(); itSmaller != smallerLeaves.end(); itSmaller++){
// 		symbol = itSmaller->first;
// 		itBigger = biggerLeaves.find(symbol);
// 		if (itBigger == biggerLeaves.end()){
// 			return false;
// 		}else{
// 			couples->insert(StateSetCouple(*(itSmaller->second), *(itBigger->second)));
// 		}
// 	}

// 	return true;
// }
























// /*****************************************************************************
//  *  VATA Tree Automata Library
//  *
//  *  Copyright (c) 2017  Petr Zufan <xzufan00@fit.vutbr.cz>
//  *
//  *  Description:
//  *    Upward inclusion for explicitly represented tree automata.
//  *
//  *****************************************************************************/


// #include <iostream>
// #include "zufan_explicit_tree_congr_incl_up.hh"

// using namespace VATA;
// using namespace ExplicitTreeUpwardCongruence;

// CongruenceInclusion::CongruenceInclusion(
// 	const ExplicitTreeAutCore&        smaller,
// 	const ExplicitTreeAutCore&        bigger)
// 	: CongruenceBase(smaller, bigger)
// 	{}

// bool CongruenceInclusion::check()
// {
// 	StateSetCoupleSet done, todo, checked;
// 	SymbolToStateSet smallerLeaves, biggerLeaves;
// 	bool termS, termB;
// 	StateSet stateSetSmaller, stateSetBigger;
// 	TransitionVector transS, transB;
// 	Bitmap usedS, usedB;
// 	StateSetCouple actual;
// 	SymbolSet alphabet;

// 	//odstrani nadbytecne stavy
// 	smaller = smaller.RemoveUnreachableStates();
// 	smaller = smaller.RemoveUselessStates();
// 	bigger = bigger.RemoveUnreachableStates();
// 	bigger = bigger.RemoveUselessStates();

// 	//najde listove stavy
// 	getLeaves(&smallerLeaves, smaller);
// 	getLeaves(&biggerLeaves, bigger);

// 	// nema listove stavy -> abeceda je prazdna mnozina
// 	if (smallerLeaves.empty()){
// 		destroyLeaves(&smallerLeaves);
// 		destroyLeaves(&biggerLeaves);
// 		setStateCounter(0);
// 		return true;
// 	}

// 	//nektery ze smallerLeaves nema ekvivalent v biggerLeaves
// 	if (!getLeafCouples(&todo, smallerLeaves, biggerLeaves)){
// 		destroyLeaves(&smallerLeaves);
// 		destroyLeaves(&biggerLeaves);
// 		setStateCounter(0);
// 		return false;
// 	}

// 	//uz nejsou potreba
// 	destroyLeaves(&smallerLeaves);
// 	destroyLeaves(&biggerLeaves);

// 	//najde vstupni abecedu
// 	alphabet = getAlphabet();

// 	while (!todo.empty()){

// 		actual = *(todo.begin());
// 		todo.erase(actual);
// 		if (isSubset(done, actual)){
// 			continue;
// 		}
// 		//findAndDeleteSubset(&done, actual);
// 		done.insert(actual);
// 		incrementStateCounter();



// 			// std::cout << "#######################################\ncounter:" << getStateCounter() << "\n";
// 			// std::cout << "todo:\n";
// 			// printCouples(todo);
// 			// std::cout << "done:\n";
// 			// printCouples(done);
// 			// std::cout << "actual:\n";
// 			// std::cout << "(";
// 			// for (auto state : actual.first){
// 				// std::cout << state << ", ";
// 			// }
// 			// std::cout << "), (";
// 			// for (auto state : actual.second){
// 				// std::cout << state << ", ";
// 			// }
// 			// std::cout << ")\n"; 


// 		for (SymbolType symbol : alphabet){

// 			transS.clear();
// 			transB.clear();

// 			//najde prechody, do kterych se lze dostat a obsahuji actual
// 			makeStep(smaller, done, actual, symbol, &transS, false);
// 			makeStep(bigger, done, actual, symbol, &transB, true);

// 			//nektery z automatu nema prechody
// 			if (transS.empty()){
// 				continue;
// 			} else {
// 				if (transB.empty()){
// 					setStateCounter(done.size());
// 					//std::cout << getStateCount(done) << "\n";
// 					return false;
// 				}
// 			}

// 			initBitmap(&usedS, transS.size());
// 			initBitmap(&usedB, transB.size());

// 			size_t arity = transB[0].GetChildrenSize();
// 			for(uint position = 0; position < arity; position++)
// 			{
// 				stateSetSmaller.clear();
// 				stateSetBigger.clear();

// 				checkTransitions(done, transS, transB, &usedB, &stateSetBigger, position, actual);
// 				checkBackTransitions(done, transS, transB, &usedS, usedB, &stateSetSmaller, position, actual);

// 				if (!inClosure(done, todo, stateSetSmaller, stateSetBigger)){
// 					termS = isTerm(smaller.GetFinalStates(), stateSetSmaller);
// 					termB = isTerm(bigger.GetFinalStates(), stateSetBigger);
					
// 					if (termS == termB){	
// 						StateSetCouple item = StateSetCouple(stateSetSmaller, stateSetBigger);
// 						todo.insert(item);
// 					} else {
// 						setStateCounter(done.size());
// 						//std::cout << getStateCount(done) << "\n";
// 						return false;
// 					}
// 				}
// 			}

// 			//check bitmap
// 			if (!checkBitmap(usedS)){
// 				setStateCounter(done.size());
// 				//std::cout << getStateCount(done);
// 				return false;
// 			}
// 		}	
// 	}

// 	setStateCounter(done.size());
// 	//std::cout << getStateCount(done) << "\n";
// 	return true;
// }

// //-----------------------------------------------------------------------------


// void CongruenceInclusion::checkTransitions(
// 	StateSetCoupleSet done,
// 	TransitionVector transS,
// 	TransitionVector transB,
// 	Bitmap *usedB,
// 	StateSet *stateSetBigger,
// 	size_t position,
// 	StateSetCouple actual)
// {
// 	bool equivalent;
// 	StateSetCoupleSet st;
// 	st.insert(actual);
// 	for (uint counterS = 0; counterS < transS.size(); counterS++){
// 		auto transitionS = transS.at(counterS);

// 		if(!inSet(st, transitionS.GetChildren()[position], false))
// 		{
// 			continue;
// 		}

// 		for (uint counterB = 0; counterB < transB.size(); counterB++){
// 			auto transitionB = transB.at(counterB);

// 			if(!inSet(st, transitionB.GetChildren()[position], true))
// 			{
// 				continue;
// 			}

// 			if (true || !usedB->at(counterB) ){

// 				// check if transitions are equivalent
// 				equivalent = true;
// 				for (uint i = 0; i < transitionS.GetChildren().size(); i++){
// 					if (!inSet(	done, 
// 								transitionS.GetChildren().at(i),
// 								transitionB.GetChildren().at(i)
// 								)){
// 						equivalent = false;
// 						break;
// 					}
// 				}

// 				if (equivalent){
// 					usedB->at(counterB) = true;
// 					stateSetBigger->insert(transitionB.GetParent());
// 				}
// 			}
// 		}
// 	}						
// }

// //-----------------------------------------------------------------------------

// void CongruenceInclusion::checkBackTransitions(
// 	StateSetCoupleSet done,
// 	TransitionVector transS,
// 	TransitionVector transB,
// 	Bitmap *usedS,
// 	Bitmap usedB,
// 	StateSet *stateSetSmaller,
// 	size_t position,
// 	StateSetCouple actual)
// {
// 	bool equivalent;
// 	StateSetCoupleSet st;
// 	st.insert(actual);
// 	for (uint counterB = 0; counterB < transB.size(); counterB++){
// 		auto transitionB = transB.at(counterB);
		
// 		if(!inSet(st, transitionB.GetChildren()[position], true))
// 		{
// 			continue;
// 		}

// 		if (true || usedB.at(counterB)){

// 			for (uint counterS = 0; counterS < transS.size(); counterS++){
// 				auto transitionS = transS.at(counterS);		

// 				if(!inSet(st, transitionS.GetChildren()[position], false))
// 				{
// 					continue;
// 				}	

// 				if (true || !usedS->at(counterS)){

// 					// check if transitions are equivalent
// 					equivalent = true;
// 					for (uint i = 0; i < transitionS.GetChildren().size(); i++){
// 						if (!inSet(	done, 
// 									transitionS.GetChildren().at(i),
// 									transitionB.GetChildren().at(i)
// 									)){
// 							equivalent = false;
// 							break;
// 						}
// 					}

// 					if (equivalent){
// 						usedS->at(counterS) = true;
// 						stateSetSmaller->insert(transitionS.GetParent());
// 					}
// 				}
// 			}
// 		}
// 	}						
// }

// //-----------------------------------------------------------------------------

// bool CongruenceInclusion::inClosure(
// 	StateSetCoupleSet set1,
// 	StateSetCoupleSet set2,
// 	StateSet first,
// 	StateSet second)
// {

// 	set1.insert(set2.begin(), set2.end());
// 	return getNormalForm(set1, first, false) == getNormalForm(set1, second, true);
// }

// //-----------------------------------------------------------------------------

// StateSet CongruenceInclusion::getNormalForm (
// 	StateSetCoupleSet set,
// 	StateSet item,
// 	bool position)
// {

// 	StateSetCoupleSet deletingSet = set;
// 	StateSet normalForm = item;
// 	bool found;
// 	while (!deletingSet.empty()){
// 		for (auto couple : deletingSet){ //pro vsechny pary v mnozine
// 			found = true;
// 			if (position){
// 				for (auto state : couple.second){ //pro vsechny stavy v prvnim z paru
// 					if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
// 						found = false;
// 						break;
// 					}
// 				}
// 				if (found){	//pokud odpovidal
// 					normalForm.insert(couple.first.begin(), couple.first.end());
// 					deletingSet.erase(couple);
// 					break;
// 				}
// 			} else {
// 				for (auto state : couple.first){ //pro vsechny stavy v prvnim z paru
// 					if (normalForm.find(state) == normalForm.end()){ //kdyz normalForm neobsahuje stav
// 						found = false;
// 						break;
// 					}
// 				}
// 				if (found){	//pokud odpovidal
// 					normalForm.insert(couple.second.begin(), couple.second.end());
// 					deletingSet.erase(couple);
// 					break;
// 				}
// 			}
// 		}
			
// 		if (!found){
// 			break;
// 		}
// 	}
// 	return normalForm;
// }

// //-----------------------------------------------------------------------------

// bool CongruenceInclusion::compareSets(
// 	StateSet set1,
// 	StateSet set2)
// {
// 	if (set1.size() != set2.size()){
// 		return false;
// 	}

// 	StateSet::iterator iterator1, iterator2;
// 	iterator1 = set1.begin();
// 	iterator2 = set2.begin();
// 	while (iterator1 != set1.end()){
// 		if (*iterator1 != *iterator2){
// 			return false;
// 		}
// 		iterator1++;
// 		iterator2++;
// 	}

// 	return true;
// }

// //-----------------------------------------------------------------------------
// // add all pairs of leaf states with same symbol into todo
// //(smaller state , bigger state)

// bool CongruenceInclusion::getLeafCouples(
// 	StateSetCoupleSet *couples, 
// 	SymbolToStateSet smallerLeaves, 
// 	SymbolToStateSet biggerLeaves)
// {
// 	SymbolType symbol;
// 	SymbolToStateSet::iterator itSmaller, itBigger;;
// 	for (itSmaller = smallerLeaves.begin(); itSmaller != smallerLeaves.end(); itSmaller++){
// 		symbol = itSmaller->first;
// 		itBigger = biggerLeaves.find(symbol);
// 		if (itBigger == biggerLeaves.end()){
// 			return false;
// 		}else{
// 			couples->insert(StateSetCouple(*(itSmaller->second), *(itBigger->second)));
// 		}
// 	}

// 	return true;
// }
