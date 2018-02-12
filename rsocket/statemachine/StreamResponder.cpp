// Copyright 2004-present Facebook. All Rights Reserved.

#include "rsocket/statemachine/StreamResponder.h"

namespace rsocket {

void StreamResponder::onSubscribe(
    std::shared_ptr<yarpl::flowable::Subscription> subscription) noexcept {
  publisherSubscribe(std::move(subscription));
}

void StreamResponder::onNext(Payload response) noexcept {
  checkPublisherOnNext();
  if (!publisherClosed()) {
    writePayload(std::move(response));
  }
}

void StreamResponder::onComplete() noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    writeComplete();
    removeFromWriter();
  }
}

void StreamResponder::onError(folly::exception_wrapper ex) noexcept {
  if (!publisherClosed()) {
    publisherComplete();
    writeApplicationError(ex.get_exception()->what());
    removeFromWriter();
  }
}

void StreamResponder::endStream(StreamCompletionSignal) {
  terminatePublisher();
}

void StreamResponder::handleCancel() {
  endStream(StreamCompletionSignal::CANCEL);
  removeFromWriter();
}

void StreamResponder::handleRequestN(uint32_t n) {
  processRequestN(n);
}

} // namespace rsocket
