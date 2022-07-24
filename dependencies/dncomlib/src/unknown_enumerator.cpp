#include <dncomlib/unknown_enumerator.hpp>

#include <utility>

dncomlib::unknown_enumerator::unknown_enumerator(IEnumUnknown * i_)
    : instance(i_)
{
}

dncomlib::unknown_enumerator::operator bool () const noexcept
{
    return static_cast<bool>(instance);
}

auto dncomlib::unknown_enumerator::operator -> () -> IEnumUnknown *
{
    return *instance.ppinstance();
}

auto dncomlib::unknown_enumerator::operator = (unknown_enumerator && rhs) -> void
{
    instance = std::move(rhs.instance);
}

auto dncomlib::unknown_enumerator::begin() -> unknown_enumerator::enumerable
{
    return unknown_enumerator::enumerable(*instance.ppinstance());
}

auto dncomlib::unknown_enumerator::end() -> unknown_enumerator::enumerable
{
    return unknown_enumerator::enumerable(nullptr);
}

dncomlib::unknown_enumerator::enumerable::enumerable(IEnumUnknown * i_)
    : instance(i_), current(nullptr)
{
}

auto dncomlib::unknown_enumerator::enumerable::operator * () -> dncomlib::unique_releasable<IUnknown>
{
    return dncomlib::unique_releasable<IUnknown>(current);
}

auto dncomlib::unknown_enumerator::enumerable::operator ++ () -> void
{
    ULONG fetched {};
    if (instance->Next(1, &current, &fetched) == S_OK && fetched)
        return;
    
    current  = nullptr;
    instance = nullptr;
}

auto dncomlib::unknown_enumerator::enumerable::operator != (const enumerable & rhs) -> bool
{
    if (instance && !current)
        ++*this;

    return instance != rhs.instance && current != rhs.current;
}