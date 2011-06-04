#include "UdpSender.hpp"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <cstring>

class UdpMessage {
public:
	UdpMessage(shb::AbstractBuffer& srcBuffer) : size(srcBuffer.getLength()), buffer(new uint8_t[size]) {
		srcBuffer.read(buffer, 0, size);
	}

	~UdpMessage() {
		delete[] buffer;
	}

	boost::asio::mutable_buffers_1 getBuffer() {
		return boost::asio::buffer(buffer, size);
	}
private:
	size_t size;
	uint8_t *buffer;
};

UdpSender::UdpSender() :
		ipv4Socket(Asio::getIoService()),
		ipv6Socket(Asio::getIoService()),
		resolver(Asio::getIoService()) {
	boost::system::error_code error;
	ipv4Socket.open(udp::v4(), error);
	ipv6Socket.open(udp::v6(), error);
}

void UdpSender::send(shb::AbstractBuffer& buffer, std::string host, uint16_t port) {
	udp::resolver::query query(host, boost::lexical_cast<std::string>(port), tcp::resolver::query::numeric_service);
	boost::shared_ptr<UdpMessage> message(new UdpMessage(buffer));
	resolver.async_resolve(query, boost::bind(
			&UdpSender::handleResolve,
			shared_from_this(),
			boost::asio::placeholders::error,
			boost::asio::placeholders::iterator,
			message));
}

void UdpSender::handleResolve(const boost::system::error_code &error,
		udp::resolver::iterator endpointIterator,
		boost::shared_ptr<UdpMessage> message) {
	if(!error) {
		boost::system::error_code hostNotFound = boost::asio::error::host_not_found;
		handleSend(hostNotFound, endpointIterator, message);
	}
}

// TODO ipv4 first
void UdpSender::handleSend(const boost::system::error_code &error,
		udp::resolver::iterator endpointIterator,
		boost::shared_ptr<UdpMessage> message) {
	if(error && endpointIterator != udp::resolver::iterator()) {
		udp::endpoint endpoint = *endpointIterator;
		udp::socket *socket = getAppropriateSocket(endpoint);
		if(socket->is_open()) {
			socket->async_send_to(message->getBuffer(), endpoint, boost::bind(
					&UdpSender::handleSend,
					shared_from_this(),
					boost::asio::placeholders::error,
					++endpointIterator,
					message));
		} else {
			handleSend(error, ++endpointIterator, message);
		}
	}
}

/*
 * Access to the sockets is not thread safe, but since they are
 * only accessed from the handler thread after construction and
 * we only use one handler thread, we don't need any synchronization.
 */

udp::socket *UdpSender::getAppropriateSocket(udp::endpoint &endpoint) {
	if(endpoint.protocol() == udp::v4()) {
		return &ipv4Socket;
	} else {
		return &ipv6Socket;
	}
}
