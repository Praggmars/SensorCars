#pragma once

namespace gfx
{
	template <typename T>
	class AutoReleasePtr
	{
		T* m_ptr;

	public:
		AutoReleasePtr() :m_ptr(nullptr) {}
		AutoReleasePtr(T& ptr) :m_ptr(&ptr) {}
		AutoReleasePtr(T* ptr) :m_ptr(ptr) {}
		AutoReleasePtr(AutoReleasePtr& ptr) :m_ptr(ptr.m_ptr) { ptr.m_ptr = nullptr; }
		~AutoReleasePtr() { Release(); }
		void Release()
		{
			if (m_ptr)
			{
				m_ptr->Release();
				m_ptr = nullptr;
			}
		}
		T* get() const { return m_ptr; }
		T* getAddress() const { return &m_ptr; }
		operator T*() const { return m_ptr; }
		operator bool() const { return m_ptr != nullptr; }
		T* operator->() const { return m_ptr; }
		T** operator&() { return &m_ptr; }
		T* operator=(T* ptr) { return m_ptr = ptr; }
		T* operator=(AutoReleasePtr& ptr)
		{
			m_ptr = ptr.m_ptr;
			ptr.m_ptr = nullptr;
			return m_ptr;
		}
		bool operator==(T* ptr) const { return m_ptr == ptr; }
		bool operator!=(T* ptr) const { return m_ptr != ptr; }
	};
}