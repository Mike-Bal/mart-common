#ifndef LIB_MART_COMMON_GUARD_NW_UNIX_H
#define LIB_MART_COMMON_GUARD_NW_UNIX_H

/**
 * unix.h (mart-netlib)
 *
 * Copyright (C) 2019: Michael Balszun <michael.balszun@mytum.de>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See either the LICENSE file in the library's root
 * directory or http://opensource.org/licenses/MIT for details.
 *
 * @author: Michael Balszun <michael.balszun@mytum.de>
 * @brief:	This file provides a simple unix doamin socket implementation
 *
 */

#include "port_layer.hpp"

#include <im_str/im_str.hpp>

#include <mart-common/utils.h>

#if __has_include(<filesystem>)
#include <filesystem>
#endif

#include <string_view>

#include "detail/socket_base.hpp"

namespace mart::nw {
// classes related to the unix sockets protocol in general
namespace un {

class endpoint {
public:
	using abi_endpoint_type = mart::nw::socks::port_layer::SockaddrUn;
	static constexpr socks::Domain domain = socks::Domain::Local;

	constexpr endpoint() noexcept = default;
	endpoint( mba::im_zstr path ) noexcept
		: _addr( std::move( path ) )
	{
	}
	explicit endpoint( std::string_view path ) noexcept
		: _addr( mba::im_zstr(path) )
	{
	}

#ifdef __cpp_lib_filesystem
	explicit endpoint( const std::filesystem::path& path ) noexcept
		// TODO use "native()" on platforms that use u8 encoding natively
		: _addr( std::string_view( path.string() ) )
	{
	}
#endif

	explicit endpoint( const abi_endpoint_type& path ) noexcept
		: endpoint( std::string_view( path.path() ) )
	{
	}

	mba::im_str asString() const noexcept { return _addr; }
	mba::im_str toString() const noexcept { return _addr; }
	mba::im_str toStringEx() const noexcept { return _addr; }

	bool valid() const noexcept { return _addr.data() != nullptr; }

	abi_endpoint_type toSockAddrUn() const noexcept { return abi_endpoint_type( _addr.data(), _addr.size() ); }

	// for use in generic contexts
	abi_endpoint_type toSockAddr() const noexcept { return toSockAddrUn(); }

	friend bool operator==( const endpoint& l, const endpoint& r ) noexcept { return l._addr == r._addr; }
	friend bool operator!=( const endpoint& l, const endpoint& r ) noexcept { return l._addr != r._addr; }
	friend bool operator<( const endpoint& l, const endpoint& r ) noexcept { return l._addr < r._addr; }

private:
	mba::im_str _addr{};
};
} // namespace un
} // namespace mart::nw

namespace mart::nw::socks::detail {
extern template class DgramSocket<mart::nw::un::endpoint>;
}

namespace mart::nw {
// classes related to the unix sockets protocol in general
namespace un {
using Socket = mart::nw::socks::detail::DgramSocket<endpoint>;
}
} // namespace mart::nw

#endif