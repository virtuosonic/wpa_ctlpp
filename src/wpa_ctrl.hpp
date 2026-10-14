#ifndef WPA_CTRL_HPP
#define WPA_CTRL_HPP

#include <string>
#include <string_view>

struct wpa_ctrl;

namespace wpa {

class ControlConnection final {
public:
    [[nodiscard]] static ControlConnection open(std::string_view path);

    ~ControlConnection() noexcept;

    ControlConnection(const ControlConnection&) = delete;
    ControlConnection& operator=(const ControlConnection&) = delete;

    ControlConnection(ControlConnection&& other) noexcept;
    ControlConnection& operator=(ControlConnection&& other) noexcept;

    void request(std::string_view command);
    [[nodiscard]] std::string request_with_reply(std::string_view command);
    void attach();
    void detach();
    [[nodiscard]] std::string receive();
    [[nodiscard]] bool pending() const;
    [[nodiscard]] int native_handle() const noexcept;

private:
    explicit ControlConnection(::wpa_ctrl* handle) noexcept;
    [[nodiscard]] ::wpa_ctrl* require_open() const;

    ::wpa_ctrl* handle_;
};

} // namespace wpa

#endif // WPA_CTRL_HPP