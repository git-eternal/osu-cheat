#pragma once

#include "unique_releasable.hpp"
#include "unknown_enumerator.hpp"
#include <metahost.h>
#include <mscoree.h>

namespace dncomlib
{
    class meta_host : public dncomlib::unique_releasable<ICLRMetaHost>
    {
    public:
        meta_host() = default;
        meta_host(ICLRMetaHost * i_);

        auto enumerate_loaded_runtimes(void * process_handle) -> dncomlib::unknown_enumerator;
    };
}