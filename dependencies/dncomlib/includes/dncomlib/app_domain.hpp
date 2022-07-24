#pragma once

#include "unique_releasable.hpp"
#include <mscoree.h>
#include <string>

namespace dncomlib
{
    class app_domain : public dncomlib::unique_releasable<IUnknown>
    {
    public:
        app_domain(app_domain &) = delete;
        app_domain(const app_domain &) = delete;

        app_domain() = default;
        app_domain(IUnknown * i_);

        auto get_friendly_name() -> std::wstring;

        static auto from_unknown(const dncomlib::unique_releasable<IUnknown> & unk) -> app_domain;
    };
}