// Copyright 2004-present Facebook. All Rights Reserved.

#pragma once

#include <memory>
#include <stdexcept>

namespace yarpl {

namespace observable {
template <typename T>
class Observable;
} // namespace observable

namespace flowable {
template <typename T>
class Subscriber;

// Exception thrown in case the downstream can't keep up.
class MissingBackpressureException : public std::runtime_error {
 public:
  MissingBackpressureException()
      : std::runtime_error("BACK_PRESSURE: DROP (missing credits onNext)") {}
};

} // namespace flowable

/**
 *Strategy for backpressure when converting from Observable to Flowable.
 */
enum class BackpressureStrategy {
  BUFFER, // Buffers all onNext values until the downstream consumes them.
  DROP, // Drops the most recent onNext value if the downstream can't keep up.
  ERROR, // Signals a MissingBackpressureException in case the downstream can't
         // keep up.
  LATEST, // Keeps only the latest onNext value, overwriting any previous value
          // if the downstream can't keep up.
  MISSING // OnNext events are written without any buffering or dropping.
};

template <typename T>
class IBackpressureStrategy {
 public:
  virtual ~IBackpressureStrategy() = default;

  virtual void init(
      std::shared_ptr<observable::Observable<T>> upstream,
      std::shared_ptr<flowable::Subscriber<T>> downstream) = 0;

  static std::shared_ptr<IBackpressureStrategy<T>> buffer();
  static std::shared_ptr<IBackpressureStrategy<T>> drop();
  static std::shared_ptr<IBackpressureStrategy<T>> error();
  static std::shared_ptr<IBackpressureStrategy<T>> latest();
  static std::shared_ptr<IBackpressureStrategy<T>> missing();
};

} // namespace yarpl
