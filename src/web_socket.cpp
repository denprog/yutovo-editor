#include "web_socket.h"
#include <mutex>
#include <condition_variable>
#include <boost/asio/buffers_iterator.hpp>
#include "logger.h"

namespace yutovo
{

//WebSocket

WebSocket::WebSocket(Config& _config) :
    config(_config),
    ws(ioc),
    logger(Logger::GetInstance("programs/Math/bin/", "yutovo", true, true))
{
}

bool WebSocket::Connect()
{
    host = config.service_ip;
    port = std::to_string(config.service_port);

    connection = true;
    connected = false;

    net::ip::tcp::resolver resolver(ioc);
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * std::chrono::seconds(1));
    ioc.restart();
    
    beast::get_lowest_layer(ws).async_connect(resolver.resolve(host, port), beast::bind_front_handler(&WebSocket::OnConnect, shared_from_this()));

    while (connection)
    {
        ioc.run_one();
    }
    return connected;
}

bool WebSocket::Send(const std::string& message, Result& result)
{
    writing = true;
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * std::chrono::seconds(1));
    ioc.restart();
    ws.async_write(net::buffer(message), beast::bind_front_handler(&WebSocket::OnWrite, shared_from_this()));

    while (writing)
    {
        ioc.run_one();
    }
    if (last_error != boost::system::errc::success)
    {
        result.error.error_code == yutovo_service::ErrorCode::OPERATION_ERROR;
        return false;
    }
    return true;
}

bool WebSocket::Receive(std::string& message, Result& result)
{
    beast::flat_buffer buffer;
    reading = true;
    beast::get_lowest_layer(ws).expires_after(config.service_timeout * std::chrono::seconds(1));
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
    result.error.error_code == yutovo_service::ErrorCode::OPERATION_ERROR;
    return false;
}

bool WebSocket::IsOpen()
{
    return ws.is_open();
}

void WebSocket::OnConnect(beast::error_code ec, tcp::resolver::results_type::endpoint_type ep)
{
    if (ec)
    {
        logger->Error("Connection to {} failed", host + ":" + port);
        connection = false;
        return;
    }

    beast::get_lowest_layer(ws).expires_never();
    ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::client));
    ws.set_option(websocket::stream_base::decorator(
        [](websocket::request_type& req)
        {
            req.set(http::field::user_agent, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-client-async");
        }));

    host += ':' + std::to_string(ep.port());

    ws.async_handshake(host, "/", beast::bind_front_handler(&WebSocket::OnHandshake, shared_from_this()));
}

void WebSocket::OnHandshake(beast::error_code ec)
{
    last_error = ec;
    if (ec)
    {
        logger->Error("Handshake to {} failed: {}", host + ":" + port, ec.message());
    }
    else
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

}
