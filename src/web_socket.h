#ifndef __WEB_SOCKET_H__
#define __WEB_SOCKET_H__

#ifndef EMSCRIPTEN
#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/buffers_iterator.hpp>
#include <fstream>
#endif
#include "config.h"
#include "result_codes.h"

#ifndef EMSCRIPTEN
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace asio = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
#endif

namespace yutovo
{

class Window;
class Logger;

class WebSocket : public std::enable_shared_from_this<WebSocket>
{
public:
    WebSocket(Config& _config, Window* _window);
    ~WebSocket();

    bool Connect();
    bool Send(const std::string& message, Result& result);
    bool Receive(std::string& message, Result& result);
    bool Reset();
    bool IsOpen();
    void Close();

private:
#ifndef EMSCRIPTEN
    void OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep);
    void OnSslHandshake(beast::error_code ec);
    void OnHandshake(beast::error_code ec);
    void OnWrite(beast::error_code ec, std::size_t bytes_transferred);
    void OnRead(beast::error_code ec, std::size_t bytes_transferred);
#endif

private:
    Config& config;
    Window* window;

    Logger* logger;

#ifdef EMSCRIPTEN
    int socket_id = 0;
#else
    struct SslContext
    {
        SslContext(Logger* logger);

        ssl::context ssl_context{ssl::context::tlsv12_client};
    };

    asio::io_context ioc;
    SslContext ssl_context;
    websocket::stream<beast::ssl_stream<beast::tcp_stream>> ws;

    bool connected = false;
    bool connection = false;
    bool writing = false, reading = false;
    beast::error_code last_error;
#endif

    std::string host, port;

    bool exit = false;
};

typedef std::shared_ptr<WebSocket> WebSocketPtr;

}

#endif
