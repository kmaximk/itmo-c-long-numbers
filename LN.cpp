//
// Created by Максим on 28.04.2023.
//
#include "LN.h"

#include <string_view>

#include <limits.h>
#include <stdio.h>
#include <string.h>
LN::LN(const char *num)
{
	size_t s = strlen(num);
	size_t ind = s - 1;
	capacity = s / 2;
	isNan = 0;
	ar = new uint8_t[capacity];
	if (!ar)
	{
		throw 2;
	}
	if (num[0] == '-')
	{
		sign = -1;
		if (num[1] == '0' && ind == 2)
		{
			sign = 0;
		}
	}
	else
	{
		sign = 1;
		if (num[0] == '0' && ind == 1)
		{
			sign = 0;
		}
	}
	size = 0;
	int begin = (1 + (sign == -1));
	if (ind > 1 && num[1] == 'X' || num[1] == 'x')
	{
		begin += 2;
	}
	while (ind > begin)
	{
		size_t nu = makeNum(num[ind]) + 16 * makeNum(num[ind - 1]);
		ar[size] = nu;
		size++;
		ensureCapacity();
		if (ind < 2)
		{
			break;
		}
		ind -= 2;
	}
	if (ind == begin)
	{
		ar[size] = makeNum(num[ind]) + 16 * makeNum(num[ind - 1]);
		size++;
		ensureCapacity();
	}
	if (ind == begin - 1)
	{
		ar[size] = makeNum(num[ind]);
		size++;
		ensureCapacity();
	}
}
LN::LN(long long num)
{
	capacity = sizeof(size_t) + 1;
	capacity = 1;
	ar = new uint8_t[capacity];
	if (!ar)
	{
		throw 2;
	}
	this->sign = (num >= 0) ? 1 : -1;
	size_t n = (num >= 0) ? num : -num;
	size = 0;
	isNan = 0;
	while (n > 0)
	{
		ar[size] = n % DEL;
		size++;
		ensureCapacity();
		n /= DEL;
	}
	if (num == 0)
	{
		sign = 0;
		ar[size] = 0;
		size = 1;
	}
}
LN::LN(const LN &x)
{
	size = x.size;
	sign = x.sign;
	capacity = x.capacity;
	isNan = x.isNan;
	ar = new uint8_t[capacity];
	if (!ar)
	{
		throw 2;
	}
	memcpy(ar, x.ar, size);
}
// copy assignment operator
LN &LN::operator=(const LN &ln)
{
	if (this != &ln)
	{
		delete[] ar;
		size = ln.size;
		capacity = ln.capacity;
		sign = ln.sign;
		isNan = ln.isNan;
		ar = new ::uint8_t[capacity];
		if (!ar)
		{
			throw 2;
		}
		memcpy(ar, ln.ar, size);
		return *this;
	}
	return *this;
}
// move assignment operator
LN &LN::operator=(LN &&ln) noexcept
{
	delete[] ar;
	size = ln.size;
	capacity = ln.capacity;
	sign = ln.sign;
	isNan = ln.isNan;
	ar = ln.ar;
	ln.ar = nullptr;
	ln.size = 0;
	return *this;
}
// move constructor
LN::LN(LN &&ln) noexcept
{
	size = ln.size;
	capacity = ln.capacity;
	sign = ln.sign;
	isNan = ln.isNan;
	ar = ln.ar;
	ln.ar = nullptr;
	ln.size = 0;
}
LN LN::operator~() const
{
	LN ret = LN();
	if (sign < 0 || isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	ret.sign = 1;
	LN curr = LN();
	LN rett = LN();
	rett.size = 0;
	rett.sign = 1;
	curr.size = 0;
	LN ost = LN();
	ost.size = 0;
	uint8_t leftover[] = { 0, 0 };
	for (size_t i = 0; i < size; i += 2)
	{
		curr.ar[0] = ar[size - i - 1 - (size % 2 == 0)];
		if (size % 2 != 0 && i == 0)
		{
			curr.ar[1] = 0;
		}
		else
		{
			curr.ar[1] = ar[size - i - (size % 2 == 0)];
		}
		for (int j = 2; j < ost.size + 2; j++)
		{
			curr.ar[j] = ost.ar[j - 2];
			curr.size = j + 1;
			curr.ensureCapacity();
		}
		curr.size = 1 + (size % 2 == 0 || i != 0) + ost.size;
		curr.sign = 1;
		int nextNum = 255;
		for (int j = 1; j < 256; j++)
		{
			LN vich = LN(j) * (ret + LN(j));
			if (vich > curr)
			{
				nextNum = j - 1;
				break;
			}
		}
		LN vich = (LN(nextNum) * (ret + LN(nextNum)));
		ost = curr - vich;
		ret.ar[0] = nextNum;
		rett.ar[rett.size] = nextNum;
		rett.size++;
		rett.ensureCapacity();
		ret = ret * LN(512);
	}
	for (size_t i = 0; i < rett.size / 2; i++)
	{
		int temp = rett.ar[i];
		rett.ar[i] = rett.ar[rett.size - i - 1];
		rett.ar[rett.size - i - 1] = temp;
	}
	if (rett.ar[0] == 0 && rett.size == 1)
	{
		rett.sign = 0;
	}
	return rett;
}
LN LN::operator-()
{
	LN ret = LN(*this);
	if (isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	ret.sign = sign * -1;
	return ret;
}

int LN::subtractModule(const LN &ln, LN *ret) const
{
	int perenos = 0;
	size_t length = std::min(size, ln.size) - (size == ln.size);
	size_t old = size;
	int zero = 0;
	ret->size = 0;
	for (size_t i = 0; i < length; i++)
	{
		if (ar[i] < perenos)
		{
			ret->ar[i] = DEL - ln.ar[i] - 1;
			perenos = 1;
		}
		else if (ar[i] - perenos < ln.ar[i])
		{
			ret->ar[i] = ar[i] - perenos - ln.ar[i];
			perenos = 1;
		}
		else
		{
			ret->ar[i] = ar[i] - ln.ar[i] - perenos;
			perenos = 0;
		}
		if (ret->ar[i] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	if (old == ln.size)
	{
		if (ar[ret->size] - perenos < ln.ar[ret->size])
		{
			ret->ar[ret->size] = (ln.ar[ret->size] - ar[ret->size]) - perenos;
		}
		else
		{
			ret->ar[ret->size] = ar[ret->size] - ln.ar[ret->size] - perenos;
			perenos = 0;
		}
		if (ret->ar[ret->size] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size; i < old; i++)
	{
		if (ar[i] < perenos)
		{
			ret->ar[i] = DEL - 1;
			perenos = 1;
		}
		else
		{
			ret->ar[i] = ar[i] - perenos;
			perenos = 0;
		}
		if (ret->ar[ret->size] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	ret->sign = zero == ret->size ? 0 : ret->sign;
	ret->size = ret->size - zero + (zero == ret->size);
	return 1;
}
LN LN::operator%(const LN &ln) const
{
	if (ln.sign == 0 || isNan || ln.isNan)
	{
		LN ret = LN();
		ret.isNan = 1;
		return ret;
	}
	return *this - ((*this / ln) * ln);
}
LN LN::operator/(const LN &ln) const
{
	LN ret = LN();
	if (ln.sign == 0 || ln.isNan || isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	divideSigned(ret, ln);
	return ret;
}
LN LN::operator-(const LN &ln) const
{
	LN ret = LN();
	if (isNan || ln.isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	subtractSigned(ret, ln);
	return ret;
}
void LN::subtractSigned(LN &ret, const LN &ln) const
{
	int cmp = compareModule(ln);
	if (cmp == 0)
	{
		if (sign == ln.sign)
		{
			ret.ar[0] = 0;
			ret.size = 1;
			ret.sign = 0;
			return;
		}
		cmp = 1;
	}
	if (sign <= 0 && ln.sign <= 0 && cmp == -1)
	{
		ln.subtractModule(*this, &ret);
		ret.sign = 1;
	}
	else if (sign <= 0 && ln.sign <= 0 && cmp == 1)
	{
		subtractModule(ln, &ret);
		ret.sign = -1;
	}
	else if (sign <= 0 && ln.sign >= 0)
	{
		addModule(ln, &ret);
		ret.sign = -1;
	}
	else if (sign >= 0 && ln.sign <= 0)
	{
		addModule(ln, &ret);
		ret.sign = 1;
	}
	else if (sign >= 0 && ln.sign >= 0 && cmp == -1)
	{
		ln.subtractModule(*this, &ret);
		ret.sign = -1;
	}
	else if (sign >= 0 && ln.sign >= 0 && cmp == 1)
	{
		subtractModule(ln, &ret);
		ret.sign = 1;
	}
	if (ret.size == 1 && ret.ar[0] == 0)
	{
		ret.sign = 0;
	}
}
LN LN::operator*(const LN &ln) const
{
	LN ret = LN();
	if (isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	if (sign == 0 || ln.sign == 0)
	{
		return ret;
	}
	multiplySigned(ret, ln);
	return ret;
}
void LN::multiplySigned(LN &ret, const LN &ln) const
{
	uint32_t pere = 0;
	for (int i = 0; i < size; i++)
	{
		LN t = LN();
		t.size = 0;
		for (int j = 0; j < i; j++)
		{
			t.ar[j] = 0;
			t.size++;
			t.ensureCapacity();
		}
		for (int j = 0; j < ln.size; j++)
		{
			uint32_t temp = ar[i] * ln.ar[j] + pere;
			pere = temp / DEL;
			t.ar[t.size] = temp % DEL;
			t.size++;
			t.ensureCapacity();
		}
		if (pere != 0)
		{
			t.ar[t.size] = pere;
			pere = 0;
			t.size++;
			t.ensureCapacity();
		}
		t.addModule(ret, &ret);
	}
	if (pere > 0)
	{
		ret.ar[ret.size] = pere;
		ret.size++;
		ret.ensureCapacity();
	}
	if (sign != ln.sign)
	{
		ret.sign = -1;
	}
	else
	{
		ret.sign = 1;
	}
}
LN LN::operator+(const LN &ln) const
{
	LN ret = LN();
	if (isNan || ln.isNan)
	{
		ret.isNan = 1;
		return ret;
	}
	ret.size = 0;
	addSigned(ret, ln);
	return ret;
}
void LN::addSigned(LN &ret, const LN &ln) const
{
	int cmp = compareModule(ln);
	if (cmp == 0)
	{
		if (sign != ln.sign)
		{
			ret.ar[0] = 0;
			ret.size = 1;
			ret.sign = 0;
			return;
		}
		cmp = 1;
	}
	if (sign <= 0 && ln.sign <= 0 && cmp == -1)
	{
		ret.sign = -1;
		addModule(ln, &ret);
	}
	else if (sign <= 0 && ln.sign <= 0 && cmp == 1)
	{
		ret.sign = -1;
		addModule(ln, &ret);
	}
	else if (sign <= 0 && ln.sign >= 0 && cmp == -1)
	{
		ret.sign = 1;
		ln.subtractModule(*this, &ret);
	}
	else if (sign <= 0 && ln.sign >= 0 && cmp == 1)
	{
		ret.sign = -1;
		subtractModule(ln, &ret);
	}
	else if (sign >= 0 && ln.sign <= 0 && cmp == 1)
	{
		ret.sign = 1;
		subtractModule(ln, &ret);
	}
	else if (sign >= 0 && ln.sign <= 0 && cmp == -1)
	{
		ret.sign = -1;
		ln.subtractModule(*this, &ret);
	}
	else if (sign >= 0 && ln.sign >= 0 && cmp == -1)
	{
		ret.sign = 1;
		addModule(ln, &ret);
	}
	else if (sign >= 0 && ln.sign >= 0 && cmp == 1)
	{
		ret.sign = 1;
		addModule(ln, &ret);
	}
	if (ret.size == 1 && ret.ar[0] == 0)
	{
		ret.sign = 0;
	}
}
int LN::addModule(const LN &ln, LN *ret) const
{
	int perenos = 0;
	int zero = 0;
	size_t length = std::min(size, ln.size);
	size_t old = size;
	ret->size = 0;
	for (size_t i = 0; i < length; i++)
	{
		int temp = ar[i] + ln.ar[i] + perenos;
		if (temp >= DEL)
		{
			ret->ar[i] = ar[i] + ln.ar[i] + perenos;
			perenos = 1;
		}
		else
		{
			ret->ar[i] = temp;
			perenos = 0;
		}
		if (ret->ar[i] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size; i < old; i++)
	{
		int temp = ar[i] + perenos;
		if (temp >= DEL)
		{
			perenos = 1;
			ret->ar[i] = temp % DEL;
		}
		else
		{
			ret->ar[i] = temp;
			perenos = 0;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size; i < ln.size; i++)
	{
		int temp = ln.ar[i] + perenos;
		if (temp >= DEL)
		{
			perenos = 1;
			ret->ar[i] = temp % DEL;
		}
		else
		{
			ret->ar[i] = temp;
			perenos = 0;
		}
		ret->size++;
		ret->ensureCapacity();
	}
	if (perenos == 1)
	{
		ret->ar[ret->size] = 1;
		ret->size++;
		ret->ensureCapacity();
	}
	return 0;
}
int LN::compareTo(const LN &ln) const
{
	if (sign < ln.sign)
	{
		return -1;
	}
	else if (sign > ln.sign)
	{
		return 1;
	}
	int ret = compareModule(ln);
	return ret * sign;
}
int LN::compareModule(const LN &ln) const
{
	int ret = 0;
	if (size < ln.size)
	{
		return -1;
	}
	else if (size > ln.size)
	{
		return 1;
	}
	for (size_t i = 1; i < size + 1; i++)
	{
		if (ar[size - i] < ln.ar[size - i])
		{
			ret = -1;
			break;
		}
		else if (ar[size - i] > ln.ar[size - i])
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

void LN::ensureCapacity()
{
	if (size >= capacity)
	{
		capacity = 2 * capacity;
		auto *newAr = new uint8_t[capacity];
		if (!newAr)
		{
			throw 2;
		}
		memcpy(newAr, ar, size * sizeof(uint8_t));
		delete[] ar;
		ar = newAr;
	}
}
void LN::resize(size_t newcap)
{
	if (capacity < newcap)
	{
		capacity = newcap;
		auto *newAr = new uint8_t[capacity];
		if (!newAr)
		{
			throw 2;
		}
		memcpy(newAr, ar, size * sizeof(uint8_t));
		delete[] ar;
		ar = newAr;
	}
}
LN &LN::operator+=(const LN &ln)
{
	if (isNan || ln.isNan)
	{
		isNan = 1;
		return *this;
	}
	addSigned(*this, ln);
	return *this;
}
LN &LN::operator-=(const LN &ln)
{
	if (isNan || ln.isNan)
	{
		isNan = 1;
		return *this;
	}
	subtractSigned(*this, ln);
	return *this;
}
LN &LN::operator*=(const LN &ln)
{
	if (isNan || ln.isNan)
	{
		isNan = 1;
		return *this;
	}
	multiplySigned(*this, ln);
	return *this;
}
LN::operator bool() const
{
	if (sign == 0 && size == 1 && ar[0] == 0)
	{
		return false;
	}
	return true;
}
LN::~LN()
{
	delete[] ar;
	~size;
	~capacity;
	~sign;
	~isNan;
}
void print(const LN &ln)
{
	::uint64_t acc = 0ll;
	for (::uint64_t i = 0; i < ln.size; i++)
	{
		acc += ((size_t)ln.ar[i]) * (1ll << (8ll * i));
	}
	printf(" --- val %i %zu\n", ln.sign, acc);
}
char *LN::toString() const
{
	if (isNan)
	{
		return new char[3]{ 'N', 'a', 'N' };
	}
	size_t strSize = size * 2 + 1 - (ar[size - 1] < 16) + (sign == -1);
	char *str = new char[strSize];
	if (!str)
	{
		throw 2;
	}
	str[strSize - 1] = '\0';
	strSize--;
	for (int i = 0; i < size; i++)
	{
		int first = ar[i] % 16;
		int second = (ar[i] / 16) % 16;
		str[strSize - i * 2 - 1] = makeChar(first);
		if (i != size - 1 || second != 0)
		{
			str[strSize - i * 2 - 2] = makeChar(second);
		}
	}
	if (sign == -1 && (size != 1 || ar[0] != 0))
	{
		str[0] = '-';
	}
	return str;
}
LN operator""_ln(const char *ln)
{
	return LN(ln);
}
LN::LN(const std::string_view num)
{
	size_t s = num.length();
	size_t ind = s - 1;
	capacity = s / 2;
	isNan = 0;
	ar = new uint8_t[capacity];
	if (!ar)
	{
		throw 2;
	}
	if (num[0] == '-')
	{
		sign = -1;
		if (num[1] == '0')
		{
			sign = 0;
		}
	}
	else
	{
		sign = 1;
		if (num[0] == '0')
		{
			sign = 0;
		}
	}
	size = 0;
	while (ind > (1 + (sign == -1)))
	{
		size_t nu = makeNum(num[ind]) + 16 * makeNum(num[ind - 1]);
		ar[size] = nu;
		size++;
		ensureCapacity();
		if (ind < 2)
		{
			break;
		}
		ind -= 2;
	}
	if (ind == 1 + (sign == -1))
	{
		ar[size] = makeNum(num[ind]) + 16 * makeNum(num[ind - 1]);
		size++;
		ensureCapacity();
	}
	if (ind == 0 + (sign == -1))
	{
		ar[size] = makeNum(num[ind]);
		size++;
		ensureCapacity();
	}
}
LN::operator long long() const
{
	if (sign == 1 && *this > LN(LONG_LONG_MAX))
	{
		throw 1;
	}
	if (sign == -1 && *this < LN(LONG_LONG_MIN))
	{
		throw 1;
	}
	long long acc = 0;
	for (int i = 0; i < size; i++)
	{
		acc += sign * ar[i] * (1ll << (8ll * i));
	}
	return acc;
}
int LN::makeNum(const char c)
{
	if ('0' <= c && c <= '9')
	{
		return c - '0';
	}
	if ('A' <= c && c <= 'F')
	{
		return c - 'A' + 10;
	}
	if ('a' <= c && c <= 'f')
	{
		return c - 'a' + 10;
	}
	return 0;
}
char LN::makeChar(int num)
{
	if (0 <= num && num <= 9)
	{
		return num + '0';
	}
	if (10 <= num && num <= 15)
	{
		return num + 'A' - 10;
	}
	return 0;
}
bool LN::operator>(const LN &ln) const
{
	return compareTo(ln) == 1 && !isNan && !ln.isNan;
}
bool LN::operator<(const LN &ln) const
{
	return compareTo(ln) == -1 && !isNan && !ln.isNan;
}
bool LN::operator==(const LN &ln) const
{
	return (compareTo(ln) == 0 && !isNan && !ln.isNan);
}
bool LN::operator<=(const LN &ln) const
{
	return compareTo(ln) <= 0 && !isNan && !ln.isNan;
}
bool LN::operator>=(const LN &ln) const
{
	return compareTo(ln) >= 0 && !isNan && !ln.isNan;
}
bool LN::operator!=(const LN &ln) const
{
	return compareTo(ln) != 0 || isNan || ln.isNan;
}
LN &LN::operator%=(const LN &ln)
{
	if (ln.sign == 0 || ln.isNan || isNan)
	{
		isNan = 1;
		return *this;
	}
	*this -= ((*this / ln) * ln);
	return *this;
}
LN &LN::operator/=(const LN &ln)
{
	if (ln.sign == 0 || ln.isNan || isNan)
	{
		isNan = 1;
		return *this;
	}
	divideSigned(*this, ln);
	return *this;
}
void LN::divideSigned(LN &ret, const LN &ln) const
{
	LN copy = LN();
	copy = *this;
	ret.sign = sign == ln.sign ? 1 : -1;
	copy.sign = 1;
	ret.size = 0;
	long long tempold = -1;
	while (copy.size > 0)
	{
		int greater = 0;
		LN temp = LN();
		temp.size = 0;
		temp.sign = 1;
		size_t takenSize = 0;
		for (size_t j = 0; j < copy.size; j++)
		{
			temp.ar[j] = copy.ar[copy.size - j - 1];
			temp.size++;
			temp.ensureCapacity();
			if (j < ln.size && temp.ar[j] < ln.ar[ln.size - j - 1] && greater == 0)
			{
				greater = -1;
			}
			else if (j < ln.size && temp.ar[j] > ln.ar[ln.size - j - 1] && greater == 0)
			{
				greater = 1;
			}
			if (greater == 1 && j == ln.size - 1)
			{
				break;
			}
			else if (greater < 1 && j == ln.size)
			{
				break;
			}
		}
		if (greater == 0 && temp.size == ln.size)
		{
			ret.ar[ret.size] = 1;
			ret.size++;
			ret.ensureCapacity();
			break;
		}
		if ((greater == 1 && temp.size != ln.size) || (greater < 1 && temp.size != ln.size + 1))
		{
			if (temp.size - tempold > 0)
			{
				ret.ar[ret.size] = 0;
				ret.size++;
				ret.ensureCapacity();
			}
			if (ret.size == 0)
			{
				ret.size = 1;
			}
			break;
		}
		if (tempold != -1)
		{
			for (int i = 0; i < (temp.size - tempold - 1); i++)
			{
				ret.ar[ret.size] = 0;
				ret.size++;
				ret.ensureCapacity();
			}
		}
		takenSize = temp.size;
		int x = greater;
		LN temp2 = LN();
		temp2 = ln;
		temp2.sign = 1;
		for (size_t j = 0; j < temp.size / 2; j++)
		{
			uint8_t tmp = temp.ar[j];
			temp.ar[j] = temp.ar[temp.size - j - 1];
			temp.ar[temp.size - j - 1] = tmp;
		}
		for (int j = 1; j < 256; j++)
		{
			LN umn = LN(j);
			temp2 = temp2 * umn;
			if (temp2.compareModule(temp) == 1)
			{
				LN supe = ln * LN(j - 1);
				supe.sign = 1;
				ret.ar[ret.size] = j - 1;
				ret.size++;
				ret.ensureCapacity();
				temp -= supe;
				break;
			}
			if (j == 255)
			{
				LN supe = ln * LN(j);
				supe.sign = 1;
				ret.ar[ret.size] = j;
				ret.size++;
				ret.ensureCapacity();
				temp -= supe;
			}
			temp2 = ln;
		}
		tempold = temp.size;
		if (temp.sign == 0 && temp.ar[0] == 0 && temp.size == 1)
		{
			tempold = 0;
		}
		int zero = 0;
		for (size_t j = copy.size - takenSize; j < copy.size - (takenSize - temp.size); j++)
		{
			copy.ar[j] = temp.ar[j - (copy.size - takenSize)];
			if (copy.ar[j] == 0)
			{
				zero++;
			}
			else
			{
				zero = 0;
			}
		}
		copy.size -= (takenSize - temp.size) + zero;
		for (long long j = copy.size - 1; j > -1; j--)
		{
			if (copy.ar[j] != 0)
			{
				break;
			}
			ret.ar[ret.size] = 0;
			ret.size++;
			ret.ensureCapacity();
			copy.size--;
		}
	}
	for (size_t j = 0; j < ret.size / 2; j++)
	{
		uint8_t tmp = ret.ar[j];
		ret.ar[j] = ret.ar[ret.size - j - 1];
		ret.ar[ret.size - j - 1] = tmp;
	}
	if (ret.size == 1 && ret.ar[0] == 0)
	{
		ret.sign = 0;
	}
}
