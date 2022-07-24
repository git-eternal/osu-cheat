#pragma once

#include "unique_releasable.hpp"
#include "app_domain.hpp"

#include <metahost.h>
#include <mscoree.h>

namespace dncomlib
{
    class runtime_host : public dncomlib::unique_releasable<ICorRuntimeHost>
    {
    public:
        runtime_host(runtime_host &) = delete;
        runtime_host(const runtime_host &) = delete;

        runtime_host() = default;
        runtime_host(ICorRuntimeHost * i_);

        auto get_default_domain_thunk() -> dncomlib::unique_releasable<IUnknown>;
    };
}