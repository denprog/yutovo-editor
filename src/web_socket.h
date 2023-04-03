#ifndef __WEB_SOCKET_H__
#define __WEB_SOCKET_H__

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include "config.h"
#include "result_codes.h"

namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>

namespace yutovo
{

class Logger;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
public:
    WebSocket(Config& _config);

    bool Connect();
    bool Send(const std::string& message, Result& result);
    bool Receive(std::string& message, Result& result);
    bool IsOpen();

private:
    void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void OnHandshake(beast::error_code ec);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);

private:
    Config& config;

    net::io_context ioc;
    websocket::stream<beast::tcp_stream> ws;

    std::string host, port;

    bool connected = false;
    bool connection = false;
    bool writing = false, reading = false;
    beast::error_code last_error;

    Logger* logger;
};

typedef std::shared_ptr<WebSocket> WebSocketPtr;

}

#endif
