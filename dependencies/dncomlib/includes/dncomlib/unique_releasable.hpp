#pragma once

namespace dncomlib
{
	// TODO: implement sfinae to check if class has a Release method
	template <typename T>
	class unique_releasable
	{
	public:
		unique_releasable(unique_releasable<T> &) = delete;
		unique_releasable(const unique_releasable<T> &) = delete;

		unique_releasable() = default;

		~unique_releasable()
		{
			remove();
		}

		unique_releasable(T * instance_)
			: instance(instance_)
		{
		}

		unique_releasable(unique_releasable<T> && other)
		{
			remove();
			instance = other.instance;
			other.instance = nullptr;
		}

		auto remove() -> void
		{
			if (instance)
				instance->Release();

			instance = nullptr;
		}

		auto operator ->() const -> T *
		{
			return instance;
		}

		auto operator =(T * rhs) -> void
		{
			remove();
			instance = rhs;
		}

		auto operator =(unique_releasable<T> && rhs) -> void
		{
			remove();
			instance = rhs.instance;
			rhs.instance = nullptr;
		}

		operator bool() const noexcept
		{
			return instance != nullptr;
		}

		auto ppinstance() -> T **
		{
			return &instance;
		}

		template <int index, typename R, typename... vargs_t>
		auto vcall(vargs_t... args) -> R
		{
			return reinterpret_cast<R(__stdcall***)(T *, vargs_t...)>(instance)[0][index](instance, args...);
		}

	protected:
		T * instance { nullptr };
	};
}