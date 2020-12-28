#pragma once

#include <forward_list>
#include <memory>

#include <grpcpp/grpcpp.h>

class HandlerInterface
{
public:
	virtual ~HandlerInterface() = default;

	virtual bool onNext(bool ok) = 0;

	virtual void cancel() = 0;
};

using HandlerPtr = std::unique_ptr<HandlerInterface>;
using HandlerTag = HandlerPtr*;