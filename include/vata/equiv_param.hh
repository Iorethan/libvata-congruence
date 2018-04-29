/*****************************************************************************
 *  VATA Tree Automata Library
 *
 *  Copyright (c) 2018  Ondrej Vales <xvales03@fit.vutbr.cz>
 *
 *  Description:
 *    Header file for the EqParam structure
 *
 *****************************************************************************/

#ifndef _VATA_EQ_PARAM_
#define _VATA_EQ_PARAM_

// VATA headers
#include <vata/aut_base.hh>

namespace VATA
{
	/**
	 * @brief  Structure containing parameters for equivalence checking functions
	 *
	 * @note  Uses bit fields so take care!
	 */
	class EqParam
	{
	public:  // data types

		enum class e_algorithm
		{
			antichains,
			bisimulation
		};

		enum class e_direction
		{
			downward,
			upward
		};

		typedef unsigned TOptions;

	private: // constants

		/// 0 ... antichains (default),         1 ... bisimulation
		static const unsigned FLAG_MASK_ALGORITHM              = 1 << 0;
		/// 0 ... basic bisimulation(default),  1 ... up to congruence
		static const unsigned FLAG_MASK_CONGRUENCE             = 1 << 1;
		/// 0 ... upward algorithm (default),   1 ... downward algorithm
		static const unsigned FLAG_MASK_DIRECTION              = 1 << 2;
		/// 0 ... no caching (default),		1 ... use caching
		static const unsigned FLAG_MASK_CACHE                  = 1 << 3;

	public:  // constants
		static const unsigned ANTICHAINS_UP = 0
			;

		static const unsigned ANTICHAINS_DOWN = 0
			| FLAG_MASK_DIRECTION
			;

		static const unsigned BISIMULATION_UP = 0
			| FLAG_MASK_ALGORITHM
			;

		static const unsigned BISIMULATION_DOWN = 0
			| FLAG_MASK_ALGORITHM
			| FLAG_MASK_DIRECTION
			;

		static const unsigned CONGRUENCE_UP = 0
			| FLAG_MASK_ALGORITHM
            | FLAG_MASK_CONGRUENCE
			;

		static const unsigned CONGRUENCE_DOWN = 0
			| FLAG_MASK_ALGORITHM
            | FLAG_MASK_CONGRUENCE
			| FLAG_MASK_DIRECTION
			;


		static const unsigned BISIMULATION_UP_CACHED = 0
			| FLAG_MASK_ALGORITHM
			| FLAG_MASK_CACHE
			;
			
		static const unsigned CONGRUENCE_UP_CACHED = 0
			| FLAG_MASK_ALGORITHM
            | FLAG_MASK_CONGRUENCE
			| FLAG_MASK_CACHE
			;

	private: // data members

		/**
		 * @brief  Bit field for the options
		 */
		TOptions flags_;

	public:   // methods

		EqParam() :
			flags_(0)
		{ }

		void SetAlgorithm(e_algorithm alg)
		{
			switch (alg)
			{
				case e_algorithm::antichains:   flags_ &= ~FLAG_MASK_ALGORITHM; break;
				case e_algorithm::bisimulation:  flags_ |=  FLAG_MASK_ALGORITHM; break;
				default: assert(false);
			}
		}

		e_algorithm GetAlgorithm() const
		{
			if (flags_ & FLAG_MASK_ALGORITHM)
			{
				return e_algorithm::bisimulation;
			}
			else
			{
				return e_algorithm::antichains;
			}
		}

		void SetDirection(e_direction dir)
		{
			switch (dir)
			{
				case e_direction::upward:    flags_ &= ~FLAG_MASK_DIRECTION; break;
				case e_direction::downward:  flags_ |=  FLAG_MASK_DIRECTION; break;
				default: assert(false);
			}
		}

		e_direction GetDirection() const
		{
			if (flags_ & FLAG_MASK_DIRECTION)
			{
				return e_direction::downward;
			}
			else
			{
				return e_direction::upward;
			}
		}

		void SetUseCongr(bool useCongr)
		{
			if (useCongr)
			{
				flags_ |=  FLAG_MASK_CONGRUENCE;
			}
			else
			{
				flags_ &= ~FLAG_MASK_CONGRUENCE;
			}
		}

		bool GetUseCongr() const
		{
			return flags_ & FLAG_MASK_CONGRUENCE;
		}

		void SetUseCache(bool useCache)
		{
			if (useCache)
			{
				flags_ |=  FLAG_MASK_CACHE;
			}
			else
			{
				flags_ &= ~FLAG_MASK_CACHE;
			}
		}

		bool GetUseCache() const
		{
			return flags_ & FLAG_MASK_CACHE;
		}

		TOptions GetOptions() const
		{
			return flags_;
		}

		std::string toString() const
		{
			// TODO add toString similar to one in InclParam
			return "Use -h to see valid options";
		}
	};
}

#endif