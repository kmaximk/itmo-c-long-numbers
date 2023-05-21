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
	size_t ind = s;
	capacity_ = (s + 1) / 2 + 2;
	isNan_ = 0;
	number_ = new uint8_t[capacity_];
	if (num[0] == '-')
	{
		sign_ = -1;
		if (num[1] == '0' && ind == 2)
		{
			sign_ = 0;
			size_ = 1;
			number_[0] = 0;
			return;
		}
	}
	else
	{
		sign_ = 1;
		if (num[0] == '0' && ind == 1)
		{
			sign_ = 0;
			size_ = 1;
			number_[0] = 0;
			return;
		}
	}
	size_ = 0;
	int begin = (0 + (sign_ == -1));
	if (ind > 1 && (num[1 + (sign_ == -1)] == 'X' || num[1 + (sign_ == -1)] == 'x'))
	{
		begin += 2;
	}
	while (ind - begin >= 2)
	{
		size_t nu = makeNum(num[ind - 1]) + 16 * makeNum(num[ind - 2]);
		number_[size_] = nu;
		size_++;
		ensureCapacity();
		if (ind < 2)
		{
			break;
		}
		ind -= 2;
	}
	if (ind == begin + 1)
	{
		number_[size_] = makeNum(num[ind - 1]);
		size_++;
		ensureCapacity();
	}
}
LN::LN(long long num)
{
	capacity_ = sizeof(size_t) + 2;
	number_ = new uint8_t[capacity_];
	this->sign_ = (num >= 0) ? 1 : -1;
	size_t n = (num >= 0) ? num : -num;
	size_ = 0;
	isNan_ = 0;
	while (n > 0)
	{
		number_[size_] = n % DEL_;
		size_++;
		ensureCapacity();
		n /= DEL_;
	}
	if (num == 0)
	{
		sign_ = 0;
		number_[size_] = 0;
		size_ = 1;
	}
}
LN::LN(const LN &x)
{
	size_ = x.size_;
	sign_ = x.sign_;
	capacity_ = x.capacity_;
	isNan_ = x.isNan_;
	number_ = new uint8_t[capacity_];
	memcpy(number_, x.number_, size_);
}
// copy assignment operator
LN &LN::operator=(const LN &ln)
{
	if (this != &ln)
	{
		delete[] number_;
		size_ = ln.size_;
		capacity_ = ln.capacity_;
		sign_ = ln.sign_;
		isNan_ = ln.isNan_;
		number_ = new ::uint8_t[capacity_];
		memcpy(number_, ln.number_, size_);
		return *this;
	}
	return *this;
}
// move assignment operator
LN &LN::operator=(LN &&ln) noexcept
{
	delete[] number_;
	size_ = ln.size_;
	capacity_ = ln.capacity_;
	sign_ = ln.sign_;
	isNan_ = ln.isNan_;
	number_ = ln.number_;
	ln.number_ = nullptr;
	ln.size_ = 0;
	return *this;
}
// move constructor
LN::LN(LN &&ln) noexcept
{
	size_ = ln.size_;
	capacity_ = ln.capacity_;
	sign_ = ln.sign_;
	isNan_ = ln.isNan_;
	number_ = ln.number_;
	ln.number_ = nullptr;
	ln.size_ = 0;
}
LN LN::operator~() const
{
	LN ret = LN();
	if (sign_ < 0 || isNan_)
	{
		ret.isNan_ = 1;
		return ret;
	}
	ret.sign_ = 1;
	LN curr = LN();
	LN rett = LN();
	rett.size_ = 0;
	rett.sign_ = 1;
	curr.size_ = 0;
	LN ost = LN();
	ost.size_ = 0;
	for (size_t i = 0; i < size_; i += 2)
	{
		curr.number_[0] = number_[size_ - i - 1 - (size_ % 2 == 0)];
		if (size_ % 2 != 0 && i == 0)
		{
			curr.number_[1] = 0;
		}
		else
		{
			curr.number_[1] = number_[size_ - i - (size_ % 2 == 0)];
		}
		for (int j = 2; j < ost.size_ + 2; j++)
		{
			curr.number_[j] = ost.number_[j - 2];
			curr.size_ = j + 1;
			curr.ensureCapacity();
		}
		curr.size_ = 1 + (size_ % 2 == 0 || i != 0) + ost.size_;
		curr.sign_ = 1;
		int nextNum = 255;
		size_t zero = 0;
		while (curr.number_[curr.size_ - zero - 1] == 0 && zero < curr.size_)
		{
			zero++;
		}
		curr.size_ = curr.size_ - zero;
		if (curr.size_ == 0)
		{
			curr.size_ = 1;
			curr.sign_ = 0;
		}
		for (int j = 1; j < 256; j++)
		{
			LN subtr = LN(j) * (ret + LN(j));
			if (subtr > curr)
			{
				nextNum = j - 1;
				break;
			}
		}
		LN subtr = (LN(nextNum) * (ret + LN(nextNum)));
		ost = curr - subtr;
		rett.number_[rett.size_] = nextNum;
		rett.size_++;
		rett.ensureCapacity();
		ret = (ret + LN(2 * nextNum)) * LN(256);
	}
	lnReverse(rett);
	if (rett.number_[0] == 0 && rett.size_ == 1)
	{
		rett.sign_ = 0;
	}
	return rett;
}
LN LN::operator-()
{
	LN ret = LN(*this);
	if (isNan_)
	{
		ret.isNan_ = 1;
		return ret;
	}
	ret.sign_ = sign_ * -1;
	return ret;
}
void LN::lnReverse(LN &ln)
{
	for (size_t i = 0; i < ln.size_ / 2; i++)
	{
		int temp = ln.number_[i];
		ln.number_[i] = ln.number_[ln.size_ - i - 1];
		ln.number_[ln.size_ - i - 1] = temp;
	}
}
void LN::karatsubaMultiply(LN &ret, const LN &ln) const
{
	if (size_ <= 2 || ln.size_ <= 2)
	{
		multiplySigned(ret, ln);
		return;
	}
	size_t msize = std::min(size_, ln.size_);
	LN a;
	if (size_ > msize / 2)
	{
		a = LN(1);
		a.resize(size_ - msize / 2 + 2);
		a.size_ = size_ - msize / 2;
	}
	else
	{
		a = LN();
	}
	if (size_ > msize / 2)
	{
		memcpy(a.number_, number_ + msize / 2, size_ - msize / 2);
	}
	LN b = LN(1);
	b.resize(msize / 2 + 2);
	b.size_ = msize / 2;
	memcpy(b.number_, number_, msize / 2);
	LN c = LN(1);
	c.resize(ln.size_ - msize / 2 + 2);
	c.size_ = ln.size_ - msize / 2;
	memcpy(c.number_, ln.number_ + msize / 2, ln.size_ - msize / 2);
	LN d = LN(1);
	d.resize(msize / 2 + 2);
	d.size_ = msize / 2;
	memcpy(d.number_, ln.number_, msize / 2);
	LN ac = LN();
	a.karatsubaMultiply(ac, c);
	LN bd = LN();
	LN bracket = LN();
	(a + b).karatsubaMultiply(bracket, (c + d));
	b.karatsubaMultiply(bd, d);
	bracket -= (ac + bd);
	size_t x2 = (msize / 2) + (msize / 2);
	a.resize(x2 + ac.size_ + 5);
	a.size_ = x2 + ac.size_;
	for (size_t i = 0; i < x2; i++)
	{
		a.number_[i] = 0;
	}
	for (size_t i = x2; i < ac.size_ + x2; i++)
	{
		a.number_[i] = ac.number_[i - x2];
	}
	b.resize((msize / 2) + bracket.size_ + 5);
	b.size_ = (msize / 2) + bracket.size_;
	for (size_t i = 0; i < msize / 2; i++)
	{
		b.number_[i] = 0;
	}
	for (size_t i = msize / 2; i < bracket.size_ + (msize / 2); i++)
	{
		b.number_[i] = bracket.number_[i - (msize / 2)];
	}
	ret = a + b + bd;
	if (sign_ != ln.sign_)
	{
		ret.sign_ = -1;
	}
}
int LN::subtractModule(const LN &ln, LN *ret) const
{
	int carry = 0;
	size_t length = std::min(size_, ln.size_) - (size_ == ln.size_);
	size_t old = size_;
	int zero = 0;
	ret->size_ = 0;
	for (size_t i = 0; i < length; i++)
	{
		if (number_[i] < carry)
		{
			ret->number_[i] = DEL_ - ln.number_[i] - 1;
			carry = 1;
		}
		else if (number_[i] - carry < ln.number_[i])
		{
			ret->number_[i] = number_[i] - carry - ln.number_[i];
			carry = 1;
		}
		else
		{
			ret->number_[i] = number_[i] - ln.number_[i] - carry;
			carry = 0;
		}
		if (ret->number_[i] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	if (old == ln.size_)
	{
		if (number_[ret->size_] - carry < ln.number_[ret->size_])
		{
			ret->number_[ret->size_] = (ln.number_[ret->size_] - number_[ret->size_]) - carry;
		}
		else
		{
			ret->number_[ret->size_] = number_[ret->size_] - ln.number_[ret->size_] - carry;
			carry = 0;
		}
		if (ret->number_[ret->size_] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size_; i < old; i++)
	{
		if (number_[i] < carry)
		{
			ret->number_[i] = DEL_ - 1;
			carry = 1;
		}
		else
		{
			ret->number_[i] = number_[i] - carry;
			carry = 0;
		}
		if (ret->number_[ret->size_] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	ret->sign_ = zero == ret->size_ ? 0 : ret->sign_;
	ret->size_ = ret->size_ - zero + (zero == ret->size_);
	return 1;
}
LN LN::operator%(const LN &ln) const
{
	if (ln.sign_ == 0 || isNan_ || ln.isNan_)
	{
		LN ret = LN();
		ret.isNan_ = 1;
		return ret;
	}
	return *this - ((*this / ln) * ln);
}
LN LN::operator/(const LN &ln) const
{
	LN ret = LN(1);
	if (ln.sign_ == 0 || ln.isNan_ || isNan_)
	{
		ret.isNan_ = 1;
		return ret;
	}
	divideSigned(ret, ln);
	return ret;
}
LN LN::operator-(const LN &ln) const
{
	LN ret = LN();
	if (isNan_ || ln.isNan_)
	{
		ret.isNan_ = 1;
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
		if (sign_ == ln.sign_)
		{
			ret.number_[0] = 0;
			ret.size_ = 1;
			ret.sign_ = 0;
			return;
		}
		cmp = 1;
	}
	if (sign_ <= 0 && ln.sign_ <= 0 && cmp == -1)
	{
		ln.subtractModule(*this, &ret);
		ret.sign_ = 1;
	}
	else if (sign_ <= 0 && ln.sign_ <= 0 && cmp == 1)
	{
		subtractModule(ln, &ret);
		ret.sign_ = -1;
	}
	else if (sign_ <= 0 && ln.sign_ >= 0)
	{
		addModule(ln, &ret);
		ret.sign_ = -1;
	}
	else if (sign_ >= 0 && ln.sign_ <= 0)
	{
		addModule(ln, &ret);
		ret.sign_ = 1;
	}
	else if (sign_ >= 0 && ln.sign_ >= 0 && cmp == -1)
	{
		ln.subtractModule(*this, &ret);
		ret.sign_ = -1;
	}
	else if (sign_ >= 0 && ln.sign_ >= 0 && cmp == 1)
	{
		subtractModule(ln, &ret);
		ret.sign_ = 1;
	}
	if (ret.size_ == 1 && ret.number_[0] == 0)
	{
		ret.sign_ = 0;
	}
}
LN LN::operator*(const LN &ln) const
{
	LN ret = LN();
	if (isNan_)
	{
		ret.isNan_ = 1;
		return ret;
	}
	if (sign_ == 0 || ln.sign_ == 0)
	{
		return ret;
	}
	if (std::min(size_, ln.size_) < 7)
	{
		multiplySigned(ret, ln);
	}
	else
	{
		karatsubaMultiply(ret, ln);
	}
	return ret;
}
void LN::multiplySigned(LN &ret, const LN &ln) const
{
	uint32_t pere = 0;
	for (int i = 0; i < size_; i++)
	{
		LN t = LN();
		t.size_ = 0;
		for (int j = 0; j < i; j++)
		{
			t.number_[j] = 0;
			t.size_++;
			t.ensureCapacity();
		}
		for (int j = 0; j < ln.size_; j++)
		{
			uint32_t temp = number_[i] * ln.number_[j] + pere;
			pere = temp / DEL_;
			t.number_[t.size_] = temp % DEL_;
			t.size_++;
			t.ensureCapacity();
		}
		if (pere != 0)
		{
			t.number_[t.size_] = pere;
			pere = 0;
			t.size_++;
			t.ensureCapacity();
		}
		t.addModule(ret, &ret);
	}
	if (pere > 0)
	{
		ret.number_[ret.size_] = pere;
		ret.size_++;
		ret.ensureCapacity();
	}
	if (sign_ != ln.sign_)
	{
		ret.sign_ = -1;
	}
	else
	{
		ret.sign_ = 1;
	}
}
LN LN::operator+(const LN &ln) const
{
	LN ret = LN();
	if (isNan_ || ln.isNan_)
	{
		ret.isNan_ = 1;
		return ret;
	}
	ret.size_ = 0;
	addSigned(ret, ln);
	return ret;
}
void LN::addSigned(LN &ret, const LN &ln) const
{
	int cmp = compareModule(ln);
	if (cmp == 0)
	{
		if (sign_ != ln.sign_)
		{
			ret.number_[0] = 0;
			ret.size_ = 1;
			ret.sign_ = 0;
			return;
		}
		cmp = 1;
	}
	if (sign_ <= 0 && ln.sign_ <= 0 && cmp == -1)
	{
		ret.sign_ = -1;
		addModule(ln, &ret);
	}
	else if (sign_ <= 0 && ln.sign_ <= 0 && cmp == 1)
	{
		ret.sign_ = -1;
		addModule(ln, &ret);
	}
	else if (sign_ <= 0 && ln.sign_ >= 0 && cmp == -1)
	{
		ret.sign_ = 1;
		ln.subtractModule(*this, &ret);
	}
	else if (sign_ <= 0 && ln.sign_ >= 0 && cmp == 1)
	{
		ret.sign_ = -1;
		subtractModule(ln, &ret);
	}
	else if (sign_ >= 0 && ln.sign_ <= 0 && cmp == 1)
	{
		ret.sign_ = 1;
		subtractModule(ln, &ret);
	}
	else if (sign_ >= 0 && ln.sign_ <= 0 && cmp == -1)
	{
		ret.sign_ = -1;
		ln.subtractModule(*this, &ret);
	}
	else if (sign_ >= 0 && ln.sign_ >= 0 && cmp == -1)
	{
		ret.sign_ = 1;
		addModule(ln, &ret);
	}
	else if (sign_ >= 0 && ln.sign_ >= 0 && cmp == 1)
	{
		ret.sign_ = 1;
		addModule(ln, &ret);
	}
	if (ret.size_ == 1 && ret.number_[0] == 0)
	{
		ret.sign_ = 0;
	}
}
int LN::addModule(const LN &ln, LN *ret) const
{
	int carry = 0;
	int zero = 0;
	size_t length = std::min(size_, ln.size_);
	size_t old = size_;
	ret->size_ = 0;
	for (size_t i = 0; i < length; i++)
	{
		int temp = number_[i] + ln.number_[i] + carry;
		if (temp >= DEL_)
		{
			ret->number_[i] = number_[i] + ln.number_[i] + carry;
			carry = 1;
		}
		else
		{
			ret->number_[i] = temp;
			carry = 0;
		}
		if (ret->number_[i] != 0)
		{
			zero = 0;
		}
		else
		{
			zero++;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size_; i < old; i++)
	{
		int temp = number_[i] + carry;
		if (temp >= DEL_)
		{
			carry = 1;
			ret->number_[i] = temp % DEL_;
		}
		else
		{
			ret->number_[i] = temp;
			carry = 0;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	for (size_t i = ret->size_; i < ln.size_; i++)
	{
		int temp = ln.number_[i] + carry;
		if (temp >= DEL_)
		{
			carry = 1;
			ret->number_[i] = temp % DEL_;
		}
		else
		{
			ret->number_[i] = temp;
			carry = 0;
		}
		ret->size_++;
		ret->ensureCapacity();
	}
	if (carry == 1)
	{
		ret->number_[ret->size_] = 1;
		ret->size_++;
		ret->ensureCapacity();
	}
	return 0;
}
int LN::compareTo(const LN &ln) const
{
	if (sign_ < ln.sign_)
	{
		return -1;
	}
	else if (sign_ > ln.sign_)
	{
		return 1;
	}
	int ret = compareModule(ln);
	return ret * sign_;
}
int LN::compareModule(const LN &ln) const
{
	int ret = 0;
	if (size_ < ln.size_)
	{
		return -1;
	}
	else if (size_ > ln.size_)
	{
		return 1;
	}
	for (size_t i = 1; i < size_ + 1; i++)
	{
		if (number_[size_ - i] < ln.number_[size_ - i])
		{
			ret = -1;
			break;
		}
		else if (number_[size_ - i] > ln.number_[size_ - i])
		{
			ret = 1;
			break;
		}
	}
	return ret;
}

void LN::ensureCapacity()
{
	if (size_ >= capacity_)
	{
		capacity_ = 2 * capacity_;
		auto *newAr = new uint8_t[capacity_];
		memcpy(newAr, number_, size_ * sizeof(uint8_t));
		delete[] number_;
		number_ = newAr;
	}
}
void LN::resize(size_t newcap)
{
	if (capacity_ < newcap)
	{
		capacity_ = newcap;
		auto *newAr = new uint8_t[capacity_];
		memcpy(newAr, number_, size_ * sizeof(uint8_t));
		delete[] number_;
		number_ = newAr;
	}
}
LN &LN::operator+=(const LN &ln)
{
	if (isNan_ || ln.isNan_)
	{
		isNan_ = 1;
		return *this;
	}
	addSigned(*this, ln);
	return *this;
}
LN &LN::operator-=(const LN &ln)
{
	if (isNan_ || ln.isNan_)
	{
		isNan_ = 1;
		return *this;
	}
	subtractSigned(*this, ln);
	return *this;
}
LN &LN::operator*=(const LN &ln)
{
	if (isNan_ || ln.isNan_)
	{
		isNan_ = 1;
		return *this;
	}
	LN copy = LN();
	if (sign_ == 0 || ln.sign_ == 0)
	{
		*this = copy;
		return *this;
	}
	karatsubaMultiply(copy, ln);
	*this = copy;
	return *this;
}
LN::operator bool() const
{
	if (sign_ == 0 && size_ == 1 && number_[0] == 0)
	{
		return false;
	}
	return true;
}
LN::~LN()
{
	delete[] number_;
}
void LN::print(const LN &ln)
{
	::uint64_t acc = 0ll;
	for (::uint64_t i = 0; i < ln.size_; i++)
	{
		printf("%i ", ln.number_[i]);
		acc += ((size_t)ln.number_[i]) * (1ll << (8ll * i));
	}
	printf(" --- val %zu, %i %zu\n", ln.size_, ln.sign_, acc);
}
char *LN::toString() const
{
	if (isNan_)
	{
		return new char[4]{ 'N', 'a', 'N' };
	}
	size_t strSize = size_ * 2 + 1 - (number_[size_ - 1] < 16) + (sign_ == -1);
	char *str = new char[strSize];
	str[strSize - 1] = '\0';
	strSize--;
	for (int i = 0; i < size_; i++)
	{
		int first = number_[i] % 16;
		int second = (number_[i] / 16) % 16;
		str[strSize - i * 2 - 1] = makeChar(first);
		if (i != size_ - 1 || second != 0)
		{
			str[strSize - i * 2 - 2] = makeChar(second);
		}
	}
	if (sign_ == -1 && (size_ != 1 || number_[0] != 0))
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
	size_t ind = s;
	capacity_ = (s + 1) / 2 + 2;
	isNan_ = 0;
	number_ = new uint8_t[capacity_];
	if (num[0] == '-')
	{
		sign_ = -1;
		if (num[1] == '0' && ind == 2)
		{
			sign_ = 0;
			size_ = 1;
			number_[0] = 0;
			return;
		}
	}
	else
	{
		sign_ = 1;
		if (num[0] == '0' && ind == 1)
		{
			sign_ = 0;
			size_ = 1;
			number_[0] = 0;
			return;
		}
	}
	size_ = 0;
	int begin = (0 + (sign_ == -1));
	if (ind > 1 && (num[1 + (sign_ == -1)] == 'X' || num[1 + (sign_ == -1)] == 'x'))
	{
		begin += 2;
	}
	while (ind - begin >= 2)
	{
		size_t nu = makeNum(num[ind - 1]) + 16 * makeNum(num[ind - 2]);
		number_[size_] = nu;
		size_++;
		ensureCapacity();
		if (ind < 2)
		{
			break;
		}
		ind -= 2;
	}
	if (ind == begin + 1)
	{
		number_[size_] = makeNum(num[ind - 1]);
		size_++;
		ensureCapacity();
	}
}
LN::operator long long() const
{
	if (sign_ == 1 && *this > LN(LONG_LONG_MAX))
	{
		throw LNOverflowException("LN number to large");
	}
	if (sign_ == -1 && *this < LN(LONG_LONG_MIN))
	{
		throw LNOverflowException("LN number to large");
	}
	long long acc = 0;
	for (int i = 0; i < size_; i++)
	{
		acc += sign_ * number_[i] * (1ll << (8ll * i));
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
	return compareTo(ln) == 1 && !isNan_ && !ln.isNan_;
}
bool LN::operator<(const LN &ln) const
{
	return compareTo(ln) == -1 && !isNan_ && !ln.isNan_;
}
bool LN::operator==(const LN &ln) const
{
	return (compareTo(ln) == 0 && !isNan_ && !ln.isNan_);
}
bool LN::operator<=(const LN &ln) const
{
	return compareTo(ln) <= 0 && !isNan_ && !ln.isNan_;
}
bool LN::operator>=(const LN &ln) const
{
	return compareTo(ln) >= 0 && !isNan_ && !ln.isNan_;
}
bool LN::operator!=(const LN &ln) const
{
	return compareTo(ln) != 0 || isNan_ || ln.isNan_;
}
LN &LN::operator%=(const LN &ln)
{
	if (ln.sign_ == 0 || ln.isNan_ || isNan_)
	{
		isNan_ = 1;
		return *this;
	}
	*this -= ((*this / ln) * ln);
	return *this;
}
LN &LN::operator/=(const LN &ln)
{
	if (ln.sign_ == 0 || ln.isNan_ || isNan_)
	{
		isNan_ = 1;
		return *this;
	}
	LN copy = LN();
	divideSigned(copy, ln);
	*this = copy;
	return *this;
}
void LN::divideSigned(LN &ret, const LN &ln) const
{
	LN copy = LN();
	copy = *this;
	ret.sign_ = sign_ == ln.sign_ ? 1 : -1;
	copy.sign_ = 1;
	ret.size_ = 0;
	long long tempold = -1;
	while (copy.size_ > 0)
	{
		int greater = 0;
		LN temp = LN();
		temp.size_ = 0;
		temp.sign_ = 1;
		size_t takenSize = 0;
		for (size_t j = 0; j < copy.size_; j++)
		{
			temp.number_[j] = copy.number_[copy.size_ - j - 1];
			temp.size_++;
			temp.ensureCapacity();
			if (j < ln.size_ && temp.number_[j] < ln.number_[ln.size_ - j - 1] && greater == 0)
			{
				greater = -1;
			}
			else if (j < ln.size_ && temp.number_[j] > ln.number_[ln.size_ - j - 1] && greater == 0)
			{
				greater = 1;
			}
			if (greater == 1 && j == ln.size_ - 1)
			{
				break;
			}
			else if (greater < 1 && j == ln.size_)
			{
				break;
			}
		}
		if (greater == 0 && temp.size_ == ln.size_)
		{
			ret.number_[ret.size_] = 1;
			ret.size_++;
			ret.ensureCapacity();
			break;
		}
		if ((greater == 1 && temp.size_ != ln.size_) || (greater < 1 && temp.size_ != ln.size_ + 1))
		{
			if (temp.size_ - tempold > 0)
			{
				ret.number_[ret.size_] = 0;
				ret.size_++;
				ret.ensureCapacity();
			}
			if (ret.size_ == 0)
			{
				ret.size_ = 1;
			}
			break;
		}
		if (tempold != -1 && tempold != temp.size_)
		{
			for (long long i = 0; i < (temp.size_ - tempold - 1); i++)
			{
				ret.number_[ret.size_] = 0;
				ret.size_++;
				ret.ensureCapacity();
			}
		}
		takenSize = temp.size_;
		LN temp2 = LN();
		temp2 = ln;
		temp2.sign_ = 1;
		lnReverse(temp);
		for (int j = 1; j < 256; j++)
		{
			LN umn = LN(j);
			temp2 = temp2 * umn;
			if (temp2.compareModule(temp) == 1)
			{
				LN supe = ln * LN(j - 1);
				supe.sign_ = 1;
				ret.number_[ret.size_] = j - 1;
				ret.size_++;
				ret.ensureCapacity();
				temp -= supe;
				break;
			}
			if (j == 255)
			{
				LN supe = ln * LN(j);
				supe.sign_ = 1;
				ret.number_[ret.size_] = j;
				ret.size_++;
				ret.ensureCapacity();
				temp -= supe;
			}
			temp2 = ln;
		}
		tempold = temp.size_;
		if (temp.sign_ == 0 && temp.number_[0] == 0 && temp.size_ == 1)
		{
			tempold = 0;
		}
		size_t zero = 0;
		for (size_t j = copy.size_ - takenSize; j < copy.size_ - (takenSize - temp.size_); j++)
		{
			copy.number_[j] = temp.number_[j - (copy.size_ - takenSize)];
			if (copy.number_[j] == 0)
			{
				zero++;
			}
			else
			{
				zero = 0;
			}
		}
		copy.size_ -= (takenSize - temp.size_) + zero;
		for (long long j = copy.size_ - 1; j > -1; j--)
		{
			if (copy.number_[j] != 0)
			{
				break;
			}
			ret.number_[ret.size_] = 0;
			ret.size_++;
			ret.ensureCapacity();
			copy.size_--;
		}
	}
	lnReverse(ret);
	if (ret.size_ == 1 && ret.number_[0] == 0)
	{
		ret.sign_ = 0;
	}
}
