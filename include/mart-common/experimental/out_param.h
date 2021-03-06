﻿#ifndef LIB_MART_COMMON_GUARD_EXPERIMENTAL_OUT_PARAM_H
#define LIB_MART_COMMON_GUARD_EXPERIMENTAL_OUT_PARAM_H
/**
 * out_param.h (mart-common/experimental)
 *
 * Copyright (C) 2015-2017: Michael Balszun <michael.balszun@mytum.de>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See either the LICENSE file in the library's root
 * directory or http://opensource.org/licenses/MIT for details.
 *
 * @author: Michael Balszun <michael.balszun@mytum.de>
 * @brief:	provides facilities to make output parameteres explicit
 *
 */

#include <utility>

namespace mart {
namespace outp {

/*
 * template wrapper  to allow c# style out parameter where necessary:
 *
 * void read(out_parm<std::string> buffer) {
 *
 *	buffer = "Hello World!";
 *
 * }
 *
 * void bar() {
 *     std::string tmp;
 *     read(out(tmp));
 * }
 *
 * Use this when it is not bovius, that a parameter is an out parameter
 *
 */
template<class T>
class out_param {
	T& _data;

	explicit out_param( T& data )
		: _data( data )
	{
	}

	template<class U>
	friend out_param<U> out( U& );

public:
			   operator T&() { return _data; }
	T&         get() { return _data; }
	out_param& operator=( const T& other )
	{
		_data = other;
		return *this;
	}
	out_param& operator=( T&& other )
	{
		_data = std::move( other );
		return *this;
	}
};

template<class U>
out_param<U> out( const U& var )
{
	return out_param<U>( var );
}

} // namespace outp

using outp::out;
using outp::out_param;

} // namespace mart

#endif
