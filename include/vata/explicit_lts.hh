/*****************************************************************************
 *  Vojnar's Army Tree Automata Library
 *
 *  Copyright (c) 2011  Jiri Simacek <isimacek@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for LTS class definition.
 *
 *****************************************************************************/

#ifndef _VATA_EXPLICIT_LTS_HH_
#define _VATA_EXPLICIT_LTS_HH_

#include <vector>

#include <vata/util/binary_relation.hh>
#include <vata/util/smart_set.hh>

namespace VATA { class ExplicitLTS; }

class VATA::ExplicitLTS {

	size_t states_;
	size_t transitions_;
	std::vector<
		std::pair<
			std::vector<std::vector<size_t>>,
			std::vector<std::vector<size_t>>
		>
	> data_;
	std::vector<Util::SmartSet> bwLabels_;

public:

	ExplicitLTS() : states_(0), transitions_(0), data_(), bwLabels_() {}

	void addTransition(size_t q, size_t a, size_t r) {

		if (a >= this->data_.size())
			this->data_.resize(a + 1);

		if (q >= this->data_[a].first.size()) {
			if (q >= this->states_)
				this->states_ = q + 1;
			this->data_[a].first.resize(q + 1);
		}

		if (r >= this->data_[a].second.size()) {
			if (r >= this->states_)
				this->states_ = r + 1;
			this->data_[a].second.resize(r + 1);
		}
		
		this->data_[a].first[q].push_back(r);
		this->data_[a].second[r].push_back(q);
		
		++this->transitions_;

	}

	void init() {

		this->bwLabels_.resize(this->states_, Util::SmartSet(this->data_.size()));

		for (size_t a = 0; a < this->data_.size(); ++a) {

			this->data_[a].first.resize(this->states_);
			this->data_[a].second.resize(this->states_);

			for (size_t r = 0; r < this->states_; ++r)
				this->bwLabels_[r].init(a, this->data_[a].second[r].size());

		}

	}

	void clear() {

		this->data_.clear();
		this->bwLabels_.clear();
		this->states_ = 0;
		this->transitions_ = 0;

	}

	const std::vector<std::vector<size_t>>& post(size_t a) const {

		assert(a < this->data_.size());

		return this->data_[a].first;

	}

	const std::vector<std::vector<size_t>>& pre(size_t a) const {

		assert(a < this->data_.size());

		return this->data_[a].second;

	}

	const Util::SmartSet& bwLabels(size_t q) const {

		assert(q < this->bwLabels_.size());

		return this->bwLabels_[q];

	}
	
	void buildDelta(std::vector<Util::SmartSet>& delta, std::vector<Util::SmartSet>& delta1) const {

		delta.resize(this->data_.size(), Util::SmartSet(this->states_));
		delta1.resize(this->data_.size(), Util::SmartSet(this->states_));

		for (size_t a = 0; a < this->data_.size(); ++a) {

			for (size_t q = 0; q < this->data_[a].first.size(); ++q) {

				for (auto& r : this->data_[a].first[q]) {

					delta[a].add(r);
					delta1[a].add(q);

				}

			}

		}

	}

	size_t labels() const { return this->data_.size(); }

	size_t states() const { return this->states_; }

	friend std::ostream& operator<<(std::ostream& os, const ExplicitLTS& lts) {

		for (size_t a = 0; a < lts.data_.size(); ++a) {

			for (size_t q = 0; q < lts.data_[a].second.size(); ++q) {

				for (auto& r : lts.data_[a].first[q])
					os << q << " --" << a << "--> " << r << std::endl;

			}

		}

		return os;

	}

public:

	void computeSimulation(const std::vector<std::vector<size_t>>& partition,
		Util::BinaryRelation& relation, size_t outputSize);

	void computeSimulation(Util::BinaryRelation& relation, size_t outputSize) {

		relation = Util::BinaryRelation(1, true);

		this->computeSimulation(std::vector<std::vector<size_t>>(), relation, outputSize);

	}


};

#endif