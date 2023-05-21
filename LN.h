//
// Created by Максим on 13.05.2023.
//

#ifndef LN_H
#define LN_H
#include <string_view>

#include <cstdio>
class LN
{
  public:
	explicit LN(long long num = 0);
	LN(LN &&ln) noexcept;
	LN(const LN &x);
	explicit LN(const char *num);
	explicit LN(std::string_view num);
	~LN();
	LN &operator=(LN &&ln) noexcept;
	LN operator~() const;
	LN operator-();
	LN &operator=(const LN &ln);
	LN operator/(const LN &ln) const;
	LN operator%(const LN &ln) const;
	LN operator-(const LN &ln) const;
	LN operator*(const LN &ln) const;
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
	LN operator+(const LN &ln) const;
	char *toString() const;

  private:
	int sign_;
	std::size_t capacity_;
	size_t size_;
	uint8_t *number_;
	const size_t DEL_ = 1l << 8l;
	int isNan_;
	int subtractModule(const LN &ln, LN *ret) const;
	void subtractSigned(LN &ret, const LN &ln) const;
	void multiplySigned(LN &ret, const LN &ln) const;
	void addSigned(LN &ret, const LN &ln) const;
	int addModule(const LN &ln, LN *ret) const;
	void divideSigned(LN &ret, const LN &ln) const;
	int compareTo(const LN &ln) const;
	int compareModule(const LN &ln) const;
	void karatsubaMultiply(LN& ret, const LN& ln) const;
	void ensureCapacity();
	void resize(size_t newcap);
	static int makeNum(char c);
	static char makeChar(int num);
	static void lnReverse(LN& ln);
	static void print(const LN &ln);
};
LN operator"" _ln(const char *ln);

struct LNOverflowException {
	const char* message;
	explicit LNOverflowException(const char* string) {
		this->message = string;
	}
};
struct LNOutOfMemoryException {
	const char* message;
	explicit LNOutOfMemoryException(const char* string) {
		this->message = string;
	}
};

#endif	  // LN_H
