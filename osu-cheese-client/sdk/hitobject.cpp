#include "hitobject.hpp"

auto sdk::ho_vector::get_coming_hitobject(std::uint32_t time) -> std::pair<hitobject *, int>
{
	for (int i = 0; i < count; ++i)
	{
		if (time <= container->hitobjects[i]->time.start && (i == 0 || time > container->hitobjects[i - 1]->time.start))
			return std::make_pair(container->hitobjects[i], i);
	}

	return std::make_pair(nullptr, -1);
}

auto sdk::ho_vector::begin() -> hitobject *
{
	return this->container->hitobjects[0];
}

auto sdk::ho_vector::end() -> hitobject *
{
	return this->container->hitobjects[this->count];
}

auto sdk::pp_phitobject_t::begin() -> hitobject *
{
	return (*this->ptr)->ho2->ho1->ho_vec->begin();
}

auto sdk::pp_phitobject_t::end() -> hitobject *
{
	return (*this->ptr)->ho2->ho1->ho_vec->end();
}

auto sdk::pp_phitobject_t::get_coming_hitobject(std::uint32_t time) -> std::pair<hitobject *, int>
{
	return (*this->ptr)->ho2->ho1->ho_vec->get_coming_hitobject(time);
}

auto sdk::pp_phitobject_t::count() -> std::uint32_t
{
	return (*this->ptr)->ho2->ho1->ho_vec->count;
}

auto sdk::pp_phitobject_t::operator[](int index) -> hitobject *
{
	return (*this->ptr)->ho2->ho1->ho_vec->container->hitobjects[index];
}

sdk::pp_phitobject_t::operator bool() const noexcept
{
	// not gonna macro this one
	// this is cancer
	if (!this->ptr)
		return false;

	auto ho_ptr = *this->ptr;

	if (!ho_ptr)
		return false;

	auto ho2_ptr = ho_ptr->ho2;

	if (!ho2_ptr)
		return false;

	auto ho1_ptr = ho2_ptr->ho1;

	if (!ho1_ptr)
		return false;

	auto ho_vec_ptr = ho1_ptr->ho_vec;

	if (!ho_vec_ptr)
		return false;

	auto container_ptr = ho_vec_ptr->container;

	if (!container_ptr)
		return false;

	return true;
}
