#pragma once

template<typename T, int size = sizeof(T)> struct se_t;
template<typename T> struct se_t<T, 1> { static __forceinline void func(T& dst, const T src) { (u8&)dst = (u8&)src; } };
template<typename T> struct se_t<T, 2> { static __forceinline void func(T& dst, const T src) { (u16&)dst = _byteswap_ushort((u16&)src); } };
template<typename T> struct se_t<T, 4> { static __forceinline void func(T& dst, const T src) { (u32&)dst = _byteswap_ulong((u32&)src); } };
template<typename T> struct se_t<T, 8> { static __forceinline void func(T& dst, const T src) { (u64&)dst = _byteswap_uint64((u64&)src); } };

template<typename T, __int64 _value, int size = sizeof(T)> struct const_se_t;;
template<typename T, __int64 _value> struct const_se_t<T, _value, 1>
{
	static const T value = (T)_value;
};

template<typename T, __int64 _value> struct const_se_t<T, _value, 2>
{
	static const T value = ((_value >> 8) & 0xff) | ((_value << 8) & 0xff00);
};

template<typename T, __int64 _value> struct const_se_t<T, _value, 4>
{
	static const T value = 
		((_value >> 24) & 0x000000ff) |
		((_value >>  8) & 0x0000ff00) |
		((_value <<  8) & 0x00ff0000) |
		((_value << 24) & 0xff000000);
};

template<typename T, __int64 _value> struct const_se_t<T, _value, 8>
{
	static const T value = 
		((_value >> 56) & 0x00000000000000ff) |
		((_value >> 40) & 0x000000000000ff00) |
		((_value >> 24) & 0x0000000000ff0000) |
		((_value >>  8) & 0x00000000ff000000) |
		((_value <<  8) & 0x000000ff00000000) |
		((_value << 24) & 0x0000ff0000000000) |
		((_value << 40) & 0x00ff000000000000) |
		((_value << 56) & 0xff00000000000000);
};

template<typename T, int size=sizeof(T)>
class be_t
{
	static_assert(size == 1 || size == 2 || size == 4 || size == 8, "Bad be_t type");
	T m_data;

public:
	be_t()
	{
	}

	be_t(const T& value)
	{
		FromLE(value);
	}

	template<typename T1>
	be_t(const be_t<T1>& value)
	{
		FromBE(value.ToBE());
	}

	T ToBE() const
	{
		return m_data;
	}

	T ToLE() const
	{
		T res;

		se_t<T>::func(res, m_data);

		return res;
	}

	void FromBE(const T& value)
	{
		m_data = value;
	}

	void FromLE(const T& value)
	{
		se_t<T>::func(m_data, value);
	}

	//template<typename T1>
	operator const T() const
	{
		return ToLE();
	}

	template<typename T1>
	operator const be_t<T1>() const
	{
		be_t<T1> res;
		res.FromBE(ToBE());
		return res;
	}

	be_t& operator = (const T& right)
	{
		FromLE(right);
		return *this;
	}

	be_t& operator = (const be_t& right)
	{
		m_data = right.m_data;
		return *this;
	}

	template<typename T1> be_t& operator += (T1 right) { return *this = T(*this) + right; }
	template<typename T1> be_t& operator -= (T1 right) { return *this = T(*this) - right; }
	template<typename T1> be_t& operator *= (T1 right) { return *this = T(*this) * right; }
	template<typename T1> be_t& operator /= (T1 right) { return *this = T(*this) / right; }
	template<typename T1> be_t& operator %= (T1 right) { return *this = T(*this) % right; }
	template<typename T1> be_t& operator &= (T1 right) { return *this = T(*this) & right; }
	template<typename T1> be_t& operator |= (T1 right) { return *this = T(*this) | right; }
	template<typename T1> be_t& operator ^= (T1 right) { return *this = T(*this) ^ right; }
	template<typename T1> be_t& operator <<= (T1 right) { return *this = T(*this) << right; }
	template<typename T1> be_t& operator >>= (T1 right) { return *this = T(*this) >> right; }

	template<typename T1> be_t& operator += (const be_t<T1>& right) { return *this = ToLE() + right.ToLE(); }
	template<typename T1> be_t& operator -= (const be_t<T1>& right) { return *this = ToLE() - right.ToLE(); }
	template<typename T1> be_t& operator *= (const be_t<T1>& right) { return *this = ToLE() * right.ToLE(); }
	template<typename T1> be_t& operator /= (const be_t<T1>& right) { return *this = ToLE() / right.ToLE(); }
	template<typename T1> be_t& operator %= (const be_t<T1>& right) { return *this = ToLE() % right.ToLE(); }
	template<typename T1> be_t& operator &= (const be_t<T1>& right) { return *this = ToBE() & right.ToBE(); }
	template<typename T1> be_t& operator |= (const be_t<T1>& right) { return *this = ToBE() | right.ToBE(); }
	template<typename T1> be_t& operator ^= (const be_t<T1>& right) { return *this = ToBE() ^ right.ToBE(); }

	template<typename T1> be_t operator & (const be_t<T1>& right) const { be_t<T> res; res.FromBE(ToBE() & right.ToBE()); return res; }
	template<typename T1> be_t operator | (const be_t<T1>& right) const { be_t<T> res; res.FromBE(ToBE() | right.ToBE()); return res; }
	template<typename T1> be_t operator ^ (const be_t<T1>& right) const { be_t<T> res; res.FromBE(ToBE() ^ right.ToBE()); return res; }

	template<typename T1> bool operator == (T1 right) const { return (T1)ToLE() == right; }
	template<typename T1> bool operator != (T1 right) const { return !(*this == right); }
	template<typename T1> bool operator >  (T1 right) const { return (T1)ToLE() >  right; }
	template<typename T1> bool operator <  (T1 right) const { return (T1)ToLE() <  right; }
	template<typename T1> bool operator >= (T1 right) const { return (T1)ToLE() >= right; }
	template<typename T1> bool operator <= (T1 right) const { return (T1)ToLE() <= right; }

	template<typename T1> bool operator == (const be_t<T1>& right) const { return ToBE() == right.ToBE(); }
	template<typename T1> bool operator != (const be_t<T1>& right) const { return !(*this == right); }
	template<typename T1> bool operator >  (const be_t<T1>& right) const { return (T1)ToLE() >  right.ToLE(); }
	template<typename T1> bool operator <  (const be_t<T1>& right) const { return (T1)ToLE() <  right.ToLE(); }
	template<typename T1> bool operator >= (const be_t<T1>& right) const { return (T1)ToLE() >= right.ToLE(); }
	template<typename T1> bool operator <= (const be_t<T1>& right) const { return (T1)ToLE() <= right.ToLE(); }
};
