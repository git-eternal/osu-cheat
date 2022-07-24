#pragma once

#include "unique_releasable.hpp"
#include "runtime_host.hpp"
#include <Windows.h>
#include <metahost.h>
#include <mscoree.h>

namespace dncomlib
{
    class runtime_info : public dncomlib::unique_releasable<ICLRRuntimeInfo>
    {
    public:
        runtime_info(runtime_info &) = delete;
        runtime_info(const runtime_info &) = delete;

        runtime_info() = default;
        runtime_info(ICLRRuntimeInfo *i_);

        auto is_started(DWORD * out_startup_flags = nullptr) -> bool;
        auto get_host() -> runtime_host;

        static auto from_unknown(const dncomlib::unique_releasable<IUnknown> & unk) -> runtime_info;
    };
}