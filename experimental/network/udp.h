/*
* udp.h
*
*  Created on: 2016-09-29
*      Author: Michael Balszun <michael.balszun@tum.de>
*
*      This file provides a simple udp socket implementation
*/

#ifndef LIBS_MART_COMMON_EXPERIMENTAL_NW_UDP_H_
#define LIBS_MART_COMMON_EXPERIMENTAL_NW_UDP_H_
#pragma once

/* ######## INCLUDES ######### */
/* Standard Library Includes */
#include <chrono>
#include <iostream> //TODO: remove
#include <cerrno>
#include <cstring>

/* Proprietary Library Includes */
#include "../../utils.h"
#include "../../MartTime.h"
#include "../../ArrayView.h"

/* Project Includes */
#include "ip.h"
#include "Socket.h"

namespace mart {
namespace experimental {
namespace nw {
namespace ip {
namespace udp {

using endpoint = ip::_impl_details_ip::basic_endpoint_v4<TransportProtocol::UDP>;

class Socket {
public:
	Socket() :
		_socket_handle(socks::Domain::inet, socks::TransportType::datagram, 0)
	{}
	Socket(endpoint local, endpoint remote)
		: Socket()
	{
		if (!isValid()) {
			throw std::runtime_error(mart::concat("Could not open create udp socket | Errnor:", std::to_string(errno), " msg: ", mart::StringView::fromZString(std::strerror(errno))).to_string());
		}
		if (!bind(local)) {
			throw std::runtime_error(mart::concat("Could not bind udp socket to address ", local.toStringEx(), "| Errnor:", std::to_string(errno), " msg: ", mart::StringView::fromZString(std::strerror(errno))).to_string());
		}
		if (!connect(remote)) {
			throw std::runtime_error(mart::concat("Could not connect socket to address ", local.toStringEx(), "| Errnor:", std::to_string(errno), " msg: ", mart::StringView::fromZString(std::strerror(errno))).to_string());
		}
	}


	Socket(Socket&&) = default;
	Socket& operator=(Socket&&) = default;

	const nw::socks::Socket& getSocket() const
	{
		return _socket_handle;
	}

	nw::socks::Socket& getSocket()
	{
		return _socket_handle;
	}

	void setDefaultRemoteEndpoint(endpoint ep)
	{
		_ep_remote = ep;
		_sa_remote = ep.toSockAddr_in();
	}
	bool connect(endpoint ep)
	{
		setDefaultRemoteEndpoint(ep);
		return _socket_handle.connect(_sa_remote) == 0;
	}
	bool bind(endpoint ep)
	{
		_ep_local = ep;
		auto result = _socket_handle.bind(ep.toSockAddr_in());
		return result == 0;
	}
	bool send(mart::ConstMemoryView data)
	{
		return _txWasSuccess(data, _socket_handle.send(data, 0));
	}
	bool sendto(mart::ConstMemoryView data, endpoint ep)
	{
		return _txWasSuccess(data, _socket_handle.sendto(data, 0, ep.toSockAddr_in()));
	}
	bool sendto(mart::ConstMemoryView data)
	{
		return _txWasSuccess(data, _socket_handle.sendto(data, 0, _sa_remote));
	}
	mart::MemoryView rec(mart::MemoryView buffer)
	{
		return _socket_handle.recv(buffer, 0).first;
	}
	mart::MemoryView recvfrom(mart::MemoryView buffer, endpoint& src_addr)
	{
		sockaddr_in src{};
		auto tmp = _socket_handle.recvfrom(buffer, 0, src);
		if (tmp.first.isValid() && src.sin_family == mart::toUType(nw::socks::Domain::inet)) {
			src_addr = endpoint(src);
			return tmp.first;
		} else {
			return mart::MemoryView{};
		}
	}
	void clearRxBuff()
	{
		//XXX: Make a RAII class for preserving the blocking state of the socket
		auto t = _socket_handle.isBlocking();
		_socket_handle.setBlocking(false);
		uint64_t _tmp{};
		auto tmp = mart::viewMemory(_tmp);
		try {
			while (_socket_handle.recv(tmp, 0).first.isValid()) { ; }
		} catch (...) {
			try {
				_socket_handle.setBlocking(t);
			} catch(...) {}
			throw;
		}
		_socket_handle.setBlocking(t);

	}
	void setTxTimeout(std::chrono::microseconds timeout)
	{
		_socket_handle.setTxTimeout(timeout);
	}
	void setRxTimeout(std::chrono::microseconds timeout)
	{
		_socket_handle.setRxTimeout(timeout);
	}
	std::chrono::microseconds getTxTimeout()
	{
		return _socket_handle.getTxTimeout();
	}
	std::chrono::microseconds getRxTimeout()
	{
		return _socket_handle.getRxTimeout();
	}
	bool setBlocking(bool should_block)
	{
		return _socket_handle.setBlocking(should_block);
	}
	bool isValid() const
	{
		return _socket_handle.isValid();
	}
	const endpoint& getLocalEndpoint() const { return _ep_local; }
	const endpoint& getRemoteEndpoint() const { return _ep_remote; }
private:
	bool _txWasSuccess(mart::ConstMemoryView data, nw::socks::port_layer::txrx_size_t ret)
	{
		return mart::narrow<nw::socks::port_layer::txrx_size_t>(data.size()) == ret;
	}
	endpoint _ep_local{};
	endpoint _ep_remote{};
	sockaddr_in _sa_remote{}; //this is only for caching, so we don't have to convert _ep_remote to sockaddr_in  every time.
	nw::socks::Socket _socket_handle;
};

}//ns udp

}//ns ip
}//ns nw
}//ns exp
}//ns mart


#endif