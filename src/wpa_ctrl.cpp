#include "wpa_ctrl.hpp"

#include "wpa_ctrl.h"

#include <array>
#include <stdexcept>
#include <utility>

namespace wpa {
namespace {

void check_result(int result, const char* operation)
{
    if (result == -2) {
        throw std::runtime_error(std::string(operation) + " timed out");
    }
    if (result < 0) {
        throw std::runtime_error(std::string(operation) + " failed");
    }
}

} // namespace

ControlConnection::ControlConnection(::wpa_ctrl* handle) noexcept
    : handle_(handle)
{
}

ControlConnection ControlConnection::open(std::string_view path)
{
    const std::string path_string(path);
    auto* handle = wpa_ctrl_open(path_string.c_str());
    if (handle == nullptr) {
        throw std::runtime_error("failed to open WPA control connection");
    }
    return ControlConnection(handle);
}

ControlConnection::~ControlConnection() noexcept
{
    if (handle_ != nullptr) {
        wpa_ctrl_close(handle_);
    }
}

ControlConnection::ControlConnection(ControlConnection&& other) noexcept
    : handle_(std::exchange(other.handle_, nullptr))
{
}

ControlConnection& ControlConnection::operator=(ControlConnection&& other) noexcept
{
    if (this != &other) {
        if (handle_ != nullptr) {
            wpa_ctrl_close(handle_);
        }
        handle_ = std::exchange(other.handle_, nullptr);
    }
    return *this;
}

void ControlConnection::request(std::string_view command)
{
    (void)request_with_reply(command);
}

std::string ControlConnection::request_with_reply(std::string_view command)
{
    std::array<char, 4096> reply{};
    std::size_t reply_length = reply.size();
    const int result = wpa_ctrl_request(
        require_open(), command.data(), command.size(), reply.data(),
        &reply_length, nullptr);
    check_result(result, "WPA control request");
    return std::string(reply.data(), reply_length);
}

void ControlConnection::attach()
{
    check_result(wpa_ctrl_attach(require_open()), "WPA event attach");
}

void ControlConnection::detach()
{
    check_result(wpa_ctrl_detach(require_open()), "WPA event detach");
}

std::string ControlConnection::receive()
{
    std::array<char, 4096> message{};
    std::size_t message_length = message.size();
    check_result(wpa_ctrl_recv(require_open(), message.data(), &message_length),
                 "WPA event receive");
    return std::string(message.data(), message_length);
}

bool ControlConnection::pending() const
{
    const int result = wpa_ctrl_pending(require_open());
    check_result(result < 0 ? result : 0, "WPA event check");
    return result > 0;
}

int ControlConnection::native_handle() const noexcept
{
    return handle_ == nullptr ? -1 : wpa_ctrl_get_fd(handle_);
}

::wpa_ctrl* ControlConnection::require_open() const
{
    if (handle_ == nullptr) {
        throw std::logic_error("WPA control connection is not open");
    }
    return handle_;
}

} // namespace wpa