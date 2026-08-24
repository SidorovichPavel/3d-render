export module math.exception;

import exception;

using exception::LogicError;
using exception::RuntimeError;

export namespace math::exception {

class UnavailableBatchData : public LogicError {
  using LogicError::LogicError;
};

class UnavailableArch : public LogicError {
  using LogicError::LogicError;
};

class SimdNotImplemented : public RuntimeError {
  using RuntimeError::RuntimeError;
};

}  // namespace math::exception
