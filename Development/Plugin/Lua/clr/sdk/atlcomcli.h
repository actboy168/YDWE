#pragma once

template <class T>
struct CComPtr {
	CComPtr()
	: v(nullptr)
	{ }
	CComPtr(T* ov)
		: v(ov)
	{ }
	~CComPtr() {
		Release();
	}
	T** operator&() {
		return &v;
	}
	T* operator->() {
		return v;
	}
	operator bool() {
		return !!v;
	}
	T* Get() const {
		return v;
	}
	void Attach(T* ov) {
		Release();
		v = ov;
	}
	T* Detach() {
		T* ret = v;
		v = nullptr;
		return ret;
	}
	void Release() {
		if (v) {
			v->Release();
			v = nullptr;
		}
	}
	template <class Query>
	HRESULT QueryInterface(Query** query) {
		return v->QueryInterface(query);
	}
	T* v;
};
