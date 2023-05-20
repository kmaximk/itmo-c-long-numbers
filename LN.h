//
// Created by Максим on 13.05.2023.
//

#ifndef LN_H
#define LN_H
#include <string_view>
struct LN
{
	int sign;
	std::size_t capacity;
	size_t size;
	uint8_t *ar;
	const size_t DEL = 1l << 8l;
	int isNan;
	explicit LN(const char *num);
	explicit LN(long long num = 0);
	LN(const LN &x);
	explicit LN(std::string_view num);
	LN &operator=(LN &&ln) noexcept;
	LN operator~() const;
	LN operator-();
	LN(LN &&ln) noexcept;
	LN &operator=(const LN &ln);
	int subtractModule(const LN &ln, LN *ret) const;
	LN operator/(const LN &ln) const;
	LN operator%(const LN &ln) const;
	LN operator-(const LN &ln) const;
	void subtractSigned(LN &ret, const LN &ln) const;
	LN operator*(const LN &ln) const;
	void multiplySigned(LN &ret, const LN &ln) const;
	LN operator+(const LN &ln) const;
	void addSigned(LN &ret, const LN &ln) const;
	int addModule(const LN &ln, LN *ret) const;
	void divideSigned(LN& ret, const LN &ln) const;
	int compareTo(const LN &ln) const;
	int compareModule(const LN &ln) const;
	void ensureCapacity();
	LN &operator+=(const LN &ln);
	LN &operator-=(const LN &ln);
	LN &operator*=(const LN &ln);
	LN &operator%=(const LN &ln);
	LN &operator/=(const LN &ln);
	bool operator>(const LN &ln) const;
	bool operator<(const LN &ln) const;
	bool operator==(const LN &ln) const;
	bool operator<=(const LN &ln) const;
	bool operator>=(const LN &ln) const;
	bool operator!=(const LN &ln) const;
	explicit operator bool() const;
	explicit operator long long() const;
	char *toString() const;
	void resize(size_t newcap);
	static int makeNum(char c);
	static char makeChar(int num);
	~LN();
};
LN operator"" _ln(const char *ln);

#endif	  // LN_H
