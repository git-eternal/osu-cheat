#pragma once

#include <metahost.h>
#include <mscoree.h>
#include "unique_releasable.hpp"

namespace dncomlib
{
    class unknown_enumerator
    {
        class enumerable
        {
        public:
            enumerable(IEnumUnknown * i_);

            auto operator * () -> dncomlib::unique_releasable<IUnknown>;
            auto operator ++ () -> void;
            auto operator != (const enumerable & rhs) -> bool;
        private:
            IUnknown * current;
            IEnumUnknown * instance;
        };

    public:
        unknown_enumerator(unknown_enumerator &) = delete;
        unknown_enumerator(const unknown_enumerator &) = delete;

        unknown_enumerator() = default;
        unknown_enumerator(IEnumUnknown * i_);

        operator bool () const noexcept;
        auto operator -> () -> IEnumUnknown *;
        auto operator = (unknown_enumerator && rhs) -> void;

        auto begin() -> unknown_enumerator::enumerable;
        auto end() -> unknown_enumerator::enumerable;

    private:
        dncomlib::unique_releasable<IEnumUnknown> instance;
    };
}