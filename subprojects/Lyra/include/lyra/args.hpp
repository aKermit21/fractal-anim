// Copyright René Ferdinand Rivera Morell
// Copyright 2017 Two Blue Cubes Ltd. All rights reserved.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef LYRA_ARGS_HPP
#define LYRA_ARGS_HPP

#include <initializer_list>
#include <string>
#include <vector>

namespace lyra {

/* tag::reference[]

[#lyra_args]
= `lyra::args`

Transport for raw args (copied from main args, supplied via init list, or from
a pair of iterators).

*/ // end::reference[]
class args
{
	public:
	// Construct from usual main() function arguments.
	args(int argc, char const * const * argv)
		: m_exeName((argv && (argc >= 1)) ? argv[0] : "")
		, m_args((argv && (argc >= 1)) ? argv + 1 : nullptr, argv + argc)
	{}

	// Construct directly from an initializer '{}'.
	args(std::initializer_list<std::string> args_list)
		: m_exeName(args_list.size() >= 1 ? *args_list.begin() : "")
		, m_args(
			  args_list.size() >= 1 ? args_list.begin() + 1 : args_list.end(),
			  args_list.end())
	{}

	// Construct from iterators.
	template <typename It>
	args(const It & start, const It & end)
		: m_exeName(start != end ? *start : "")
		, m_args(start != end ? start + 1 : end, end)
	{}

	// The executable name taken from argument zero.
	std::string exe_name() const { return m_exeName; }

	// Arguments, excluding the exe name.
	std::vector<std::string>::const_iterator begin() const
	{
		return m_args.begin();
	}

	std::vector<std::string>::const_iterator end() const
	{
		return m_args.end();
	}

	private:
	std::string m_exeName;
	std::vector<std::string> m_args;
};

} // namespace lyra

#endif
