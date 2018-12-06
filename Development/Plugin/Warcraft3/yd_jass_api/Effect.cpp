#include <warcraft3/jass.h>
#include <warcraft3/jass/hook.h>
#include <warcraft3/war3_searcher.h>
#include <base/hook/fp_call.h>

#define M_PI       3.14159265358979323846   // pi


template <typename T>
class qmatrix
{
public:
	typedef T value_type[3][3];

public:
	qmatrix(T* data)
		: data_(data)
	{ }

	qmatrix<T>& operator =(const value_type& r)
	{
		value_type& m = *(value_type*)data_;
		for (size_t i = 0; i < 3; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				m[i][j] = r[i][j];
			}
		}
		return *this;
	}

	qmatrix<T>& operator *=(const value_type& r)
	{
		value_type& m = *(value_type*)data_;
		value_type l;
		for (size_t i = 0; i < 3; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				l[i][j] = m[i][j];
			}
		}
		for (size_t i = 0; i < 3; ++i)
		{
			for (size_t j = 0; j < 3; ++j)
			{
				T n(0);
				for (size_t k = 0; k < 3; ++k)
				{
					n += l[i][k] * r[k][j];
				}
				m[i][j] = n;
			}
		}
		return *this;
	}

private:
	T* data_;
};

namespace base { namespace warcraft3 { namespace japi {

	jass::jnothing_t __cdecl EXSetEffectXY(jass::jhandle_t effect, jass::jreal_t* px, jass::jreal_t* py)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC0) = jass::from_real(*px);
		*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC4) = jass::from_real(*py);
	}
	
	jass::jnothing_t __cdecl EXSetEffectZ(jass::jhandle_t effect, jass::jreal_t* pz)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC8) = jass::from_real(*pz);
	
	}

	jass::jreal_t __cdecl EXGetEffectX(jass::jhandle_t effect)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return 0;
		}
		return jass::to_real(*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC0));
	}

	jass::jreal_t __cdecl EXGetEffectY(jass::jhandle_t effect)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return 0;
		}
		return jass::to_real(*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC4));
	}

	jass::jreal_t __cdecl EXGetEffectZ(jass::jhandle_t effect)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return 0;
		}
		return jass::to_real(*(float*)(*(uintptr_t*)(obj + 0x28) + 0xC8));
	}

	jass::jnothing_t __cdecl EXSetEffectSize(jass::jhandle_t effect, jass::jreal_t* psize)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		*(float*)(*(uintptr_t*)(obj + 0x28) + 0xE8) = jass::from_real(*psize);
	}

	jass::jreal_t __cdecl EXGetEffectSize(jass::jhandle_t effect)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return 0;
		}
		return jass::to_real(*(float*)(*(uintptr_t*)(obj + 0x28) + 0xE8));
	}

	jass::jnothing_t __cdecl EXEffectMatRotateX(jass::jhandle_t effect, jass::jreal_t* pangle)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		float angle = jass::from_real(*pangle) * float(M_PI / 180.);
		qmatrix<float> mat((float*)(*(uintptr_t*)(obj + 0x28) + 0x108));
		qmatrix<float>::value_type m = {
			{ 1.f, 0.f, 0.f },
			{ 0.f, cosf(angle), sinf(angle) },
			{ 0.f, -sinf(angle), cosf(angle) },
		};
		mat *= m;
	}

	jass::jnothing_t __cdecl EXEffectMatRotateY(jass::jhandle_t effect, jass::jreal_t* pangle)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		float angle = jass::from_real(*pangle) * float(M_PI / 180.);
		qmatrix<float> mat((float*)(*(uintptr_t*)(obj + 0x28) + 0x108));
		qmatrix<float>::value_type m = {
			{ cosf(angle), 0.f, -sinf(angle) },
			{ 0.f, 1.f, 0.f },
			{ sinf(angle), 0.f, cosf(angle) },
		};
		mat *= m;
	}

	jass::jnothing_t __cdecl EXEffectMatRotateZ(jass::jhandle_t effect, jass::jreal_t* pangle)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		float angle = jass::from_real(*pangle) * float(M_PI / 180.);
		qmatrix<float> mat((float*)(*(uintptr_t*)(obj + 0x28) + 0x108));
		qmatrix<float>::value_type m = {
			{ cosf(angle), sinf(angle), 0.f },
			{ -sinf(angle), cosf(angle), 0.f },
			{ 0.f, 0.f, 1.f },
		};
		mat *= m;
	}

	jass::jnothing_t __cdecl EXEffectMatScale(jass::jhandle_t effect, jass::jreal_t* px, jass::jreal_t* py, jass::jreal_t* pz)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		float x = jass::from_real(*px);
		float y = jass::from_real(*py);
		float z = jass::from_real(*pz);
		qmatrix<float> mat((float*)(*(uintptr_t*)(obj + 0x28) + 0x108));
		qmatrix<float>::value_type m = {
			{ x, 0.f, 0.f },
			{ 0.f, y, 0.f },
			{ 0.f, 0.f, z },
		};
		mat *= m;
	}

	jass::jnothing_t __cdecl EXEffectMatReset(jass::jhandle_t effect)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		qmatrix<float> mat((float*)(*(uintptr_t*)(obj + 0x28) + 0x108));
		qmatrix<float>::value_type m = {
			{ 1.f, 0.f, 0.f },
			{ 0.f, 1.f, 0.f },
			{ 0.f, 0.f, 1.f },
		};
		mat = m;
	}

	jass::jnothing_t __cdecl EXSetEffectSpeed(jass::jhandle_t effect, jass::jreal_t* pspeed)
	{
		uintptr_t obj = handle_to_object(effect);
		if (!obj) {
			return;
		}
		uintptr_t eff = *(uintptr_t*)(obj + 0x28);
		this_call<void>(*(uintptr_t*)(*(uintptr_t*)eff+0x28), eff, jass::from_real(*pspeed));
	}

	void InitializeEffect()
	{
		jass::japi_add((uintptr_t)EXSetEffectXY,      "EXSetEffectXY",      "(Heffect;RR)V");
		jass::japi_add((uintptr_t)EXSetEffectZ,       "EXSetEffectZ",       "(Heffect;R)V");
		jass::japi_add((uintptr_t)EXGetEffectX,       "EXGetEffectX",       "(Heffect;)R");
		jass::japi_add((uintptr_t)EXGetEffectY,       "EXGetEffectY",       "(Heffect;)R");
		jass::japi_add((uintptr_t)EXGetEffectZ,       "EXGetEffectZ",       "(Heffect;)R");
		jass::japi_add((uintptr_t)EXSetEffectSize,    "EXSetEffectSize",    "(Heffect;R)V");
		jass::japi_add((uintptr_t)EXGetEffectSize,    "EXGetEffectSize",    "(Heffect;)R");
		jass::japi_add((uintptr_t)EXEffectMatRotateX, "EXEffectMatRotateX", "(Heffect;R)V");
		jass::japi_add((uintptr_t)EXEffectMatRotateY, "EXEffectMatRotateY", "(Heffect;R)V");
		jass::japi_add((uintptr_t)EXEffectMatRotateZ, "EXEffectMatRotateZ", "(Heffect;R)V");
		jass::japi_add((uintptr_t)EXEffectMatScale,   "EXEffectMatScale",   "(Heffect;RRR)V");
		jass::japi_add((uintptr_t)EXEffectMatReset,   "EXEffectMatReset",   "(Heffect;)V");
		jass::japi_add((uintptr_t)EXSetEffectSpeed,   "EXSetEffectSpeed",   "(Heffect;R)V");
	}
}}}
