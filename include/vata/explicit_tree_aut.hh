/*****************************************************************************
 *  Vojnar's Army Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for a explicitly represented tree automaton.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_TREE_AUT_HH_
#define _VATA_EXPLICIT_TREE_AUT_HH_

// VATA headers
#include <vata/vata.hh>
#include <vata/aut_base.hh>
#include <vata/aut_op.hh>
#include <vata/parsing/abstr_parser.hh>
#include <vata/serialization/abstr_serializer.hh>
#include <vata/util/ord_vector.hh>

// Standard library headers
#include <cstdint>
#include <memory>
#include <unordered_set>

namespace VATA { template <class SymbolType> class ExplicitTreeAut; }

GCC_DIAG_OFF(effc++)
template <class SymbolType>
class VATA::ExplicitTreeAut : public AutBase {
GCC_DIAG_ON(effc++)

	template <class Aut>
	friend Aut Union(const Aut&, const Aut&, AutBase::StateToStateMap*);

	template <class Aut>
	friend Aut Intersection(const Aut&, const Aut&,
		AutBase::ProductTranslMap*);

	template <class Aut>
	friend Aut RemoveUnreachableStates(const Aut&,
		AutBase::StateToStateMap* pTranslMap);

	template <class Aut>
	friend bool CheckInclusion(const Aut&, const Aut&);

public:   // public data types

	typedef std::vector<StateType> StateTuple;
	typedef std::set<StateTuple> StateTupleSet;
	typedef std::unordered_set<StateType> StateSet;
	typedef VATA::Util::OrdVector<StateType> StateSetLight;

private:  // private data types

	typedef VATA::Util::AutDescription AutDescription;
	typedef std::shared_ptr<StateTupleSet> StateTupleSetPtr;
	typedef std::unordered_map<SymbolType, StateTupleSetPtr> TransitionCluster;
	typedef std::shared_ptr<TransitionCluster> TransitionClusterPtr;
	typedef std::unordered_map<StateType, TransitionClusterPtr> StateToTransitionClusterMap;
	typedef std::shared_ptr<StateToTransitionClusterMap> StateToTransitionClusterMapPtr;

private:  // data members

	StateSet finalStates_;

	StateToTransitionClusterMapPtr transitions_;

protected:

	template <class T>
	static const typename T::mapped_type::element_type* genericLookup(const T& cont, const typename T::Key& key) {

		auto iter = cont.find(key);
		if (iter == cont.end())
			return NULL;

		return iter->second.get();

	}

public:

	struct TransitionIterator {

		const ExplicitTreeAut& aut_;

		typename StateToTransitionClusterMap::const_iterator stateClusterIterator_;
		typename TransitionCluster::const_iterator symbolSetIterator_;
		StateTupleSet::const_iterator tupleIterator_;

		void _init() {

			if (this->stateClusterIterator_ == this->aut.transitions_->end())
				return;
			this->symbolSetIterator_ = this->stateClusterIterator_->second->begin();
			this->tupleIterator_ = this->symbolSetIterator_->second->begin();

		}

		void _increment() {

			++this->tupleIterator_;
			if (this->tupleIterator_ != this->symbolSetIterator_->second->end())
				return;

			++this->symbolSetIterator_;
			if (this->symbolSetIterator_ != this->stateClusterIterator_->second->end()) {
				this->tupleIterator_ = this->symbolSetIterator_->second->begin();
				return;
			}
			
			++this->stateClusterIterator_;
			this->_init();

		}

		TransitionIterator(const ExplicitTreeAut& aut,
			typename StateToTransitionClusterMap::const_iterator pos)
			: aut_(aut), stateClusterIterator_(pos) {

			this->_init();

		}

		bool operator==(const TransitionIterator& rhs) const {

			if (this->stateClusterIterator_ != rhs.stateClusterIterator_)
				return false;

			if (this->stateClusterIterator_ == this->aut.transitions_->end())
				return true;

			return (this->symbolSetIterator_ == rhs.symbolSetIterator_) &&
				(this->tupleIterator_ == rhs.tupleIterator_);
				
		}

		TransitionIterator& operator++() {

			this->_increment();

			return *this;

		}

		TransitionIterator operator++(int) {

			TransitionIterator iter = *this;

			iter._increment();

			return iter;

		}

		const StateTuple& children() const {
			return *this->tupleIterator_;
		}
		
		const SymbolType& symbol() const {
			return this->symbolSetIterator_->first;
		}
		
		const StateType& state() const {
			return this->stateClusterIterator_->first;
		}

	};

	typedef TransitionIterator iterator;
	typedef TransitionIterator const_iterator;

	TransitionIterator begin() { return TransitionIterator(*this, this->transitions_->begin()); }
	TransitionIterator end() { return TransitionIterator(*this, this->transitions_->end()); }

public:

	struct AcceptingTransitions {

		const ExplicitTreeAut& aut_;

		AcceptingTransitions(const ExplicitTreeAut& aut) : aut_(aut) {}

		struct AcceptingIterator {
	
			const ExplicitTreeAut& aut_;
	
			StateSet::const_iterator stateSetIterator_;
			typename TransitionCluster::const_iterator symbolSetIterator_;
			StateTupleSet::const_iterator tupleIterator_;
	
			void _init() {
	
				typename StateToTransitionClusterMap::const_iterator iter;
	
				while (1) {
					if (this->stateSetIterator_ == this->aut.finalStates_->end())
						return;
					iter = this->aut_.transitions_.find(*this->stateSetIterator_);
					if (iter != this->aut_.transitions_.end())
						break;
					++this->stateSetIterator_;
				}
				
				this->symbolSetIterator_ = iter->second->begin();
				this->tupleIterator_ = this->symbolSetIterator_->second->begin();
	
			}
	
			void _increment() {
	
				++this->tupleIterator_;
				if (this->tupleIterator_ != this->symbolSetIterator_->second->end())
					return;
	
				++this->symbolSetIterator_;
				if (this->symbolSetIterator_ != this->stateClusterIterator_->second->end()) {
					this->tupleIterator_ = this->symbolSetIterator_->second->begin();
					return;
				}
				
				++this->stateSetIterator_;
				this->_init();
	
			}
	
			AcceptingIterator(const ExplicitTreeAut& aut, StateSet::const_iterator pos)
				: aut_(aut), stateSetIterator_(pos) {
			
				this->_init();
	
			}
	
			bool operator==(const AcceptingIterator& rhs) const {
	
				if (this->stateSetIterator_ != rhs.stateSetIterator_)
					return false;
	
				if (this->stateSetIterator_ == this->aut.finalStates_->end())
					return true;
	
				return (this->symbolSetIterator_ == rhs.symbolSetIterator_) &&
					(this->tupleIterator_ == rhs.tupleIterator_);
					
			}
	
			AcceptingIterator& operator++() {
	
				this->_increment();
	
				return *this;
	
			}
	
			AcceptingIterator operator++(int) {
	
				TransitionIterator iter = *this;
	
				iter._increment();
	
				return iter;
	
			}
	
			const StateTuple& children() const {
				return *this->tupleIterator_;
			}
			
			const SymbolType& symbol() const {
				return this->symbolSetIterator_->first;
			}
			
			const StateType& state() const {
				return *this->stateIterator_;
			}
	
		};

		typedef AcceptingIterator iterator;
		typedef AcceptingIterator const_iterator;

		AcceptingIterator begin() {

			return AcceptingIterator(this->aut_, this->aut_.finalStates_->begin());

		}

		AcceptingIterator end() {

				return AcceptingIterator(this->aut_, this->aut_.finalStates_->end());

		}

	};

	AcceptingTransitions accepting;

public:   // public methods

	ExplicitTreeAut() : finalStates_(),
		transitions_(StateToTransitionClusterMapPtr(new StateToTransitionClusterMap())),
		accepting(*this) {}

	ExplicitTreeAut(const ExplicitTreeAut& aut)
		: finalStates_(aut.finalStates_), transitions_(aut.transitions), accepting(*this) {}

	ExplicitTreeAut& operator=(const ExplicitTreeAut& rhs) {

		this->finalStates_ = rhs.finalStates_;
		this->transitions_ = rhs.transitions_;

	}

	template <class StateTransFunc, class SymbolTransFunc>
	void LoadFromString(VATA::Parsing::AbstrParser& parser, const std::string& str,
		StateTransFunc stateTranslator, SymbolTransFunc symbolTranslator,
		const std::string& params = "") {

		AutDescription desc = parser.ParseString(str);

		for (auto s : desc.finalStates)
			this->finalStates_.insert(stateTranslator(s));

		for (auto t : desc.transitions) {

			// traverse the transitions
			const AutDescription::StateTuple& childrenStr = t.first;
			const std::string& symbolStr = t.second;
			const AutDescription::State& parentStr = t.third;
	
			// translate children
			StateTuple children;
			for (auto c : childrenStr) {
				// for all children states
				children.push_back(stateTranslator(c));
			}

			this->AddTransition(children, symbolTranslator(symbolStr), stateTranslator(parentStr));	
	
		}

	}

	template <class StatePrintFunc, class SymbolPrintFunc>
	std::string DumpToString(VATA::Serialization::AbstrSerializer& serializer,
		StatePrintFunc statePrinter, SymbolPrintFunc symbolPrinter,
		const std::string& params = "") const {

		AutDescription desc;

		for (auto s : this->finalStates_)
			desc.finalStates.insert(s);

		for (auto stateClusterPair : *this->transitions_) {

			assert(stateClusterPair.second);

			for (auto symbolTupleSetPair : *stateClusterPair.second) {

				assert(symbolTupleSetPair.second);

				for (auto tuple : *symbolTupleSetPair.second) {

					std::vector<std::string> tupleStr;

					for (auto s : tuple)
						tupleStr.push_back(statePrinter(s));

					desc.transitions.insert(
						AutDescription::Transition(
							tupleStr,
							symbolPrinter(symbolTupleSetPair.first),
							statePrinter(stateClusterPair.first)
						)
					);
					
				}

			}

		}
		
		return serializer.Serialize(desc);
		
	}

	inline const StateSet& GetFinalStates() const {
		return this->finalStates_;
	}

	inline void SetStateFinal(const StateType& state) {
		this->finalStates_.insert(state);
	}

	void AddTransition(const StateTuple& children, const SymbolType& symbol,
		const StateType& state) {

		if (this->transitions_.unique()) {

			this->transitions_ = StateToTransitionClusterMapPtr(
				new StateToTransitionClusterMap(*transitions_)
			);

		}

		auto& cluster = this->transitions_.insert(
			std::make_pair(state, TransitionClusterPtr(NULL))
		).first->second;

		if (!cluster || !cluster.unique()) {

			cluster = TransitionClusterPtr(
				cluster ? (new TransitionCluster(*cluster)) : (new TransitionCluster())
			);

		}

		auto& tupleSet = cluster.insert(
			std::make_pair(symbol, StateTupleSetPtr(NULL))
		).first->second;

		if (!tupleSet || !tupleSet.unique()) {

			tupleSet = StateTupleSetPtr(
				tupleSet ? (new StateTupleSet(*tupleSet)) : (new StateTupleSet())
			);

		}

		tupleSet.insert(children);

	}

	template <class OperationFunc>
	static void ForeachDownSymbolFromStateAndStateSetDo(const ExplicitTreeAut& lhs,
		const ExplicitTreeAut& rhs, const StateType& lhsState,
		const StateSetLight& rhsSet, OperationFunc& opFunc)
	{

		auto leftCluster = ExplicitTreeAut::genericLookup(*lhs.transitions_, lhsState);

		if (!leftCluster)
			return;

		for (auto rhsState : rhsSet) {

			auto rightCluster = ExplicitTreeAut::genericLookup(*rhs.transitions_, rhsState);

			if (!rightCluster)
				continue;

			for (auto leftSymbolTupleSetPair : *leftCluster) {

				auto rightTupleSet = ExplicitTreeAut::genericLookup(
					*rightCluster, leftSymbolTupleSetPair.first
				);

				if (!rightTupleSet)
					continue;

				assert(leftSymbolTupleSetPair.second);

				opFunc(*leftSymbolTupleSetPair.second, *rightTupleSet);

			}

		}

	}

	~ExplicitTreeAut() {}

public:

};

#endif