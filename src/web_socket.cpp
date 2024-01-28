#include "web_socket.h"
#include "window.h"
#include <yutovo_logger/logger.h>
#include <chrono>
#include <boost/asio/strand.hpp>

namespace yutovo
{

using namespace std::chrono_literals;
using namespace std::chrono;
namespace net = boost::asio;

//WebSocket

WebSocket::WebSocket(Config& _config, Window* _window) :
    config(_config),
    window(_window),
#ifndef EMSCRIPTEN
    ws(net::make_strand(ioc), ssl_context.ssl_context),
#endif
    logger(Logger::GetInstance(config.logs_path, "yutovo", true, true))
{
}

WebSocket::~WebSocket()
{
#ifdef EMSCRIPTEN
    if (socket_id > 0)
        window->Close(socket_id);
#endif
    logger->Info("WebSocket closed");
}

bool WebSocket::Connect()
{
    host = config.service_ip;
    port = std::to_string(config.service_port);

#ifdef EMSCRIPTEN
    socket_id = window->Connect(host + ":" + port);
    return socket_id > 0;
#else
    connection = true;
    connected = false;

    asio::ip::tcp::resolver resolver(ioc);
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * std::chrono::seconds(1));
    ioc.restart();
    
    beast::get_lowest_layer(ws).async_connect(resolver.resolve(host, port), beast::bind_front_handler(&WebSocket::OnConnect, shared_from_this()));

    while (connection)
    {
        ioc.run_one();
    }
    return connected;
#endif
}

bool WebSocket::Send(const std::string& message, Result& result)
{
#ifdef EMSCRIPTEN
    if (!window->Send(socket_id, message))
    {
        result.error.error_code = yutovo_service::ErrorCode::OPERATION_ERROR;
        return false;
    }
    return true;
#else
    writing = true;
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * std::chrono::seconds(1));
    ioc.restart();
    ws.async_write(asio::buffer(message), beast::bind_front_handler(&WebSocket::OnWrite, shared_from_this()));

    while (writing)
    {
        ioc.run_one();
    }
    if (last_error != boost::system::errc::success)
    {
        result.error.error_code = yutovo_service::ErrorCode::OPERATION_ERROR;
        return false;
    }
    return true;
#endif
}

bool WebSocket::Receive(std::string& message, Result& result)
{
#ifdef EMSCRIPTEN
    auto now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    auto next = now;
    while (next - now < config.service_timeout * 1s) //wait for message
    {
        if (!window->Receive(socket_id, message))
            return false;
        if (!message.empty())
            break;
        std::this_thread::sleep_for(1ms);
        next = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    }
    return true;
#else
    beast::flat_buffer buffer;
    reading = true;
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * seconds(1));
    ioc.restart();
    ws.async_read(buffer, beast::bind_front_handler(&WebSocket::OnRead, shared_from_this()));

    while (reading)
    {
        ioc.run_one();
    }
    if (last_error == boost::system::errc::success)
    {
        message = std::string(boost::asio::buffers_begin(buffer.data()), boost::asio::buffers_end(buffer.data()));
        return true;
    }
    result.error.error_code = yutovo_service::ErrorCode::OPERATION_ERROR;
    return false;
#endif
}

bool WebSocket::IsOpen()
{
#ifdef EMSCRIPTEN
    return window->IsOpen(socket_id);
#else
    return ws.is_open();
#endif
}

#ifndef EMSCRIPTEN
void WebSocket::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        connection = false;
        return;
    }

    beast::get_lowest_layer(ws).expires_after(std::chrono::seconds(30));
    ws.next_layer().async_handshake(ssl::stream_base::client, beast::bind_front_handler(&WebSocket::OnSslHandshake, shared_from_this()));
}

void WebSocket::OnSslHandshake(beast::error_code ec)
{
    last_error = ec;
    if (ec)
    {
        logger->Error("OnSslHandshake to {} failed: {}", host + ":" + port, ec.message());
        return;
    }

    beast::get_lowest_layer(ws).expires_never();
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async-ssl");
        }));

    ws.async_handshake(host, "/", beast::bind_front_handler(&WebSocket::OnHandshake, shared_from_this()));
}

void WebSocket::OnHandshake(beast::error_code ec)
{
    last_error = ec;
    if (ec)
    {
        logger->Error("OnHandshake to {} failed: {}", host + ":" + port, ec.message());
        return;
    }
    connected = true;
    connection = false;
}

void WebSocket::OnWrite(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    last_error = ec;
    if (ec)
    {
        logger->Error("Write failed: {}", ec.message());
    }
    writing = false;
}

void WebSocket::OnRead(beast::error_code ec, std::size_t bytes_transferred)
{
    boost::ignore_unused(bytes_transferred);
    last_error = ec;
    if (ec)
    {
        logger->Error("Read failed: {}", ec.message());
    }
    reading = false;
}
#endif

}
