#pragma once

#include <metahost.h>
#include <mscoree.h>

#include "meta_host.hpp"
#include "unique_releasable.hpp"
#include "unknown_enumerator.hpp"
#include "runtime_info.hpp"

namespace dncomlib
{
    auto clr_create_instance(const IID & clsid, const IID & riid) -> dncomlib::meta_host;
}