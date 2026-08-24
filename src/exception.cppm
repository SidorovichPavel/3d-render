module;

#include <exception>
#include <string>

#include <boost/stacktrace.hpp>

export module exception;

export namespace exception {

class StackfullException : public std::exception {
 public:
  explicit StackfullException() = default;
  ~StackfullException() override = default;

  explicit StackfullException(std::string message)
      : message_(std::move(message)) {}

  auto What() const noexcept -> std::string { return message_; }

  auto Where() const -> std::string {
    return boost::stacktrace::to_string(trace_);
  }

  auto what() const noexcept -> const char* override {
    return message_.c_str();
  }

 private:
  boost::stacktrace::stacktrace trace_;
  std::string message_{"StackfullException"};
};

class RuntimeError : public StackfullException {
  using StackfullException::StackfullException;
};

class LogicError : public StackfullException {
  using StackfullException::StackfullException;
};

}  // namespace exception
