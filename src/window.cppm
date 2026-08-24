module;

#include <MiniFB.h>
#include <MiniFB_enums.h>

#include <spdlog/spdlog.h>

#include <bitset>
#include <cstddef>
#include <functional>
#include <meta>

export module window;

import universal;
import reflection;
import exception;

export namespace mfb {

enum class Keyboard : std::int16_t;
enum class KeyMod : std::uint8_t;
enum class MouseButton : std::uint8_t;
enum class UpdateState : std::int8_t;
enum class WindowFlag : std::uint8_t;

using WindowFlags = universal::Flags<WindowFlag>;

class MfbRuntimeError : public exception::RuntimeError {
 public:
  using RuntimeError::RuntimeError;
};

using WindowId = std::uint32_t;

class Window;
class IOContext;

auto MakeWindow(IOContext& context, const std::string_view title,
                const unsigned width, const unsigned height,
                const WindowFlags flags) -> Window;

class Window {
  friend auto MakeWindow(IOContext& context, const std::string_view title,
                         const unsigned width, const unsigned height,
                         const WindowFlags flags) -> Window;

 public:
  auto ShouldClose() const noexcept -> bool { return !mfb_wait_sync(handle_); }

  auto Update(std::uint32_t* const data, const std::uint32_t width,
              const std::uint32_t height) noexcept -> UpdateState {
    return static_cast<UpdateState>(
        mfb_update_ex(handle_, data, width, height));
  }

  auto GetId() const noexcept -> WindowId { return id_; }

 private:
  Window(mfb_window* handle, WindowId id) : handle_(handle), id_(id) {
    if (handle_ == nullptr) throw MfbRuntimeError("Failed to open window");
  }

  mfb_window* handle_{nullptr};

  WindowId id_{0};
};

auto MakeColor2(const std::uint8_t r, const std::uint8_t g,
                const std::uint8_t b, const std::uint8_t a) noexcept
    -> std::uint32_t {
  return MFB_ARGB(a, r, g, b);
}

auto MakeColor(const std::uint8_t r, const std::uint8_t g,
               const std::uint8_t b) noexcept -> std::uint32_t {
  return MFB_RGB(r, g, b);
}

enum class Keyboard : std::int16_t {
  kNone = 0,
#define KEYDECL(NAME, VALUE, _) kCode_##NAME = VALUE,
  KEY_LIST(KEYDECL)
#undef KEYDECL
};

enum class KeyMod : std::uint8_t {
  kNone = 0u,
  kShift = MFB_KB_MOD_SHIFT,
  kControl = MFB_KB_MOD_CONTROL,
  kAlt = MFB_KB_MOD_ALT,
  kSuper = MFB_KB_MOD_SUPER,
  kCaps = MFB_KB_MOD_CAPS_LOCK,
  kNumLock = MFB_KB_MOD_NUM_LOCK,
};

enum class MouseButton : std::uint8_t {
  kkNone = MFB_MOUSE_BTN_0,
  kButton_1 = MFB_MOUSE_BTN_1,
  kButton_2 = MFB_MOUSE_BTN_2,
  kButton_3 = MFB_MOUSE_BTN_3,
  kButton_4 = MFB_MOUSE_BTN_4,
  kButton_5 = MFB_MOUSE_BTN_5,
  kButton_6 = MFB_MOUSE_BTN_6,
  kButton_7 = MFB_MOUSE_BTN_7,
};

enum class UpdateState : std::int8_t {
  kOk = MFB_STATE_OK,
  kNone = kOk,
  kExit = MFB_STATE_EXIT,
  kInvalidWindow = MFB_STATE_INVALID_WINDOW,
  kInvalidBuffer = MFB_STATE_INVALID_BUFFER,
  kInternalError = MFB_STATE_INTERNAL_ERROR
};

enum class WindowFlag : std::uint8_t {
  kNone = 0u,
  kResizable = MFB_WF_RESIZABLE,
  kFullscreen = MFB_WF_FULLSCREEN,
  kFullscreenDesktop = MFB_WF_FULLSCREEN_DESKTOP,
  kBorderless = MFB_WF_BORDERLESS,
  kAlwaysOnTop = MFB_WF_ALWAYS_ON_TOP,
};

auto SerializeKeyCode(Keyboard key) -> std::string_view {
#define KEYDECL(NAME, VALUE, STRING) \
  case Keyboard::kCode_##NAME:       \
    return STRING;

  switch (key) {
    case Keyboard::kNone:
      return "None";
      KEY_LIST(KEYDECL)
  }

#undef KEYDECL

  std::unreachable();
}

class IOContext {
 public:
  IOContext() = default;

  auto OnKeyPressed(Keyboard key, [[maybe_unused]] KeyMod mod,
                    bool is_pressed) noexcept {
    if (key == Keyboard::kCode_KB_KEY_UNKNOWN) {
      spdlog::debug("Unknown key detected");
      return;
    }

    spdlog::debug("Key {} is {}", SerializeKeyCode(key),
                  is_pressed ? "pressed" : "released");

    const auto key_index = static_cast<std::size_t>(key);
    keyboard_state[key_index] = is_pressed;
  }

 private:
  static constexpr auto kMaxKeyCode = reflection::MaxEnumValue<Keyboard>();
  static_assert(kMaxKeyCode > 0, "kMaxKeyCode must be greater than zero");
  std::bitset<kMaxKeyCode + 1> keyboard_state;
};

}  // namespace mfb

namespace {

class Manager {
  using KeyboardCallbackType =
      std::move_only_function<void(mfb::Keyboard, mfb::KeyMod, bool) noexcept>;

  struct Callbacks {
    std::vector<KeyboardCallbackType> keyboard;
  };

 public:
  static auto GetInstance() noexcept -> Manager& {
    static Manager instance;
    return instance;
  }

  auto RegisterWindow(mfb_window* window) noexcept -> mfb::WindowId {
    mfb_set_keyboard_callback(window, &Manager::KeyboardCallback);

    const auto id = ++id_;
    id2window_.emplace(id, window);
    return id;
  }

  static auto KeyboardCallback(mfb_window* window, mfb_key key,
                               mfb_key_mod key_mod, bool is_pressed) noexcept
      -> void {
    auto& window_callbacks = GetInstance().window_callbacks_;
    for (auto& cb : window_callbacks[window].keyboard) {
      cb(static_cast<mfb::Keyboard>(key), static_cast<mfb::KeyMod>(key_mod),
         is_pressed);
    }
  }

  auto RegisterKeyboardCallback(mfb::WindowId id, KeyboardCallbackType callback)
      -> void {
    if (!id2window_.contains(id)) {
      spdlog::error("Try register callback for unknown window");
    }

    const auto window = id2window_.at(id);
    window_callbacks_[window].keyboard.push_back(std::move(callback));
  }

 private:
  Manager() = default;

  std::atomic<mfb::WindowId> id_{0};

  std::unordered_map<mfb::WindowId, mfb_window*> id2window_;
  std::unordered_map<mfb_window*, Callbacks> window_callbacks_;
};

}  // namespace

export namespace mfb {

auto MakeWindow(IOContext& context, const std::string_view title,
                const unsigned width, const unsigned height,
                const WindowFlags flags) -> Window {
  auto handle = mfb_open_ex(title.data(), width, height, flags.GetValue());

  const auto id = Manager::GetInstance().RegisterWindow(handle);
  Manager::GetInstance().RegisterKeyboardCallback(
      id, std::bind_front(&IOContext::OnKeyPressed, context));

  return Window(handle, id);
}

}  // namespace mfb
