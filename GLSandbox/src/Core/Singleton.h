#pragma once


namespace CoreUtils {
	template <typename T>
	class Singleton {
	protected:
		constexpr Singleton() noexcept = default;

		Singleton(const Singleton&) = delete;
		Singleton& operator=(const Singleton&) = delete;

		Singleton(const Singleton&&) = delete;
		Singleton&& operator=(const Singleton&&) = delete;

	public:
		static T& GetInstance() noexcept {
			static T instance{};
			return instance;
		}

		static T* GetInstancePtr() noexcept {
			return &GetInstance();
		}
	};
}