#include <algorithm>

namespace utils::finantial {
template <std::size_t _I, std::size_t _F> auto operator<<(std::ostream& os, Decimal<_I, _F> decimal) -> std::ostream& {
  return os << static_cast<std::string>(decimal);
}

template <std::size_t bits> auto operator+=(std::bitset<bits>& a, std::bitset<bits> b) -> std::bitset<bits>& {
  std::bitset<bits> carry;
  while (b.any()) {
    carry = a & b;
    a ^= b;
    b = carry << 1;
  }
  return a;
}

template <std::size_t bits> auto operator+(std::bitset<bits> a, std::bitset<bits> b) -> std::bitset<bits> {
  return a += b;
}

template <std::size_t bits> auto operator-(std::bitset<bits> a) -> std::bitset<bits> {
  return ~a + std::bitset<bits>(0b1);
}

template <std::size_t bits> auto operator-=(std::bitset<bits>& a, std::bitset<bits> b) -> std::bitset<bits>& {
  return a += -b;
}

template <std::size_t bits> auto operator*(std::bitset<bits> a, std::bitset<bits> b) -> std::bitset<bits> {
  std::bitset<bits> result, one(0b1);
  while (b.any()) {
    if ((b & one)[0]) {
      result += a;
    }
    a <<= 1;
    b >>= 1;
  }
  return result;
}

template <std::size_t bits> auto operator<(std::bitset<bits> a, std::bitset<bits> b) -> bool {
  return (a -= b)[bits - 1];
}

template <std::size_t bits> auto operator<=(std::bitset<bits> a, std::bitset<bits> b) -> bool {
  return !(b < a);
}

template <std::size_t bits> auto div(std::bitset<bits> a, std::bitset<bits> b) -> std::pair<std::bitset<bits>, std::bitset<bits>> {
  bool div_sign = a[bits - 1] ^ b[bits - 1];
  bool mod_sign = a[bits - 1];
  if (a[bits - 1]) a = -a;
  if (b[bits - 1]) b = -b;
  if (a.none() || b.none()) {
    return {};
  }

  std::size_t bit{bits - 1};
  std::pair<std::bitset<bits>, std::bitset<bits>> result;
  do {
    result.second <<= 1;
    result.second.set(0, a[bit]);
    if (b <= result.second) {
      result.first.set(bit);
      result.second -= b;
    }
  } while (bit--);

  if (div_sign) {
    result.first = -result.first;
  }

  if (mod_sign) {
    result.second = -result.second;
  }

  return result;
}

template <std::size_t bits> auto operator/(std::bitset<bits> a, std::bitset<bits> b) -> std::bitset<bits> {
  return div(a, b).first;
}

template <std::size_t bits> auto operator%(std::bitset<bits> a, std::bitset<bits> b) -> std::bitset<bits> {
  return div(a, b).second;
}

template <std::size_t I, std::size_t F> Decimal<I, F>::Decimal(const std::string_view& value) {
  *this = value;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator=(const std::string_view& value) -> Decimal<I, F>& {
  if (const auto& match = parse(value); match) {
    conversion(*match);
  }
  return *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator<(const Decimal& other) const noexcept -> bool {
  return this->m_mantissa < other.m_mantissa;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator>(const Decimal& other) const noexcept -> bool {
  return other < *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator>=(const Decimal& other) const noexcept -> bool {
  return !(*this < other);
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator<=(const Decimal& other) const noexcept -> bool {
  return !(other < *this);
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator==(const Decimal& other) const noexcept -> bool {
  return this->m_mantissa == other.m_mantissa;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator!=(const Decimal& other) const noexcept -> bool {
  return !(*this == other);
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator-() const noexcept -> Decimal {
  Decimal result(*this);
  result.m_mantissa = -result.m_mantissa;
  return result;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator+(const Decimal& other) const noexcept -> Decimal {
  return Decimal(*this) += other;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator-(const Decimal& other) const noexcept -> Decimal {
  return Decimal(*this) -= other;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator*(const Decimal& other) const noexcept -> Decimal {
  return Decimal(*this) *= other;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator/(const Decimal& other) const noexcept -> Decimal {
  return Decimal(*this) /= other;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator%(const Decimal& other) const noexcept -> Decimal {
  return Decimal(*this) %= other;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator+=(const Decimal& other) noexcept -> Decimal& {
  m_mantissa += other.m_mantissa;
  return *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator-=(const Decimal& other) noexcept -> Decimal& {
  m_mantissa -= other.m_mantissa;
  return *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator*=(const Decimal& other) noexcept -> Decimal& {
  bool negative = sign() ^ other.sign();

  std::bitset<2 * m_bits> a((this->sign() ? -this->m_mantissa : this->m_mantissa).to_string());
  std::bitset<2 * m_bits> b((other.sign() ? -other.m_mantissa : other.m_mantissa).to_string());
  std::bitset<2 * m_bits> c = a * b;

  for (std::size_t exponent{}; exponent < F; ++exponent) {
    c = div(c, std::bitset<2 * m_bits>(0b1010)).first;
  }

  m_mantissa = std::bitset<m_bits>(c.to_string().substr(m_bits));

  if (negative) {
    m_mantissa = -m_mantissa;
  }

  return *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator/=(const Decimal& other) noexcept -> Decimal& {
  Decimal temp(other);

  bool negative = sign() ^ temp.sign();
  abs();
  temp.abs();

  auto divmod = div(m_mantissa, temp.m_mantissa);
  m_mantissa = divmod.first;
  auto div = divmod.second;

  for (std::size_t exponent = {}; exponent < F; ++exponent) {
    m_mantissa = m_mantissa * m_bit_ten;

    if (div.any()) {
      div = div * m_bit_ten;
      m_mantissa += div / temp.m_mantissa;
      div = div % temp.m_mantissa;
    }
  }

  if (negative) m_mantissa = -m_mantissa;
  return *this;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::operator%=(const Decimal& other) noexcept -> Decimal& {
  m_mantissa = m_mantissa % other.m_mantissa;
  return *this;
}

template <std::size_t I, std::size_t F> Decimal<I, F>::operator std::string() const noexcept {
  std::bitset<m_bits> mantissa(sign() ? -this->m_mantissa : this->m_mantissa);

  std::string result;
  while (mantissa.any()) {
    const auto& divmod = div(mantissa, m_bit_ten);
    result += divmod.second.to_ulong() + '0';
    mantissa = divmod.first;
  }

  while (result.size() <= F) {
    result += '0';
  }

  if (F) {
    result.insert(F, ".");
  }

  result += (sign() ? '-' : '+');

  std::reverse(result.begin(), result.end());
  return result;
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::parse(const std::string_view& value) const -> std::optional<std::cmatch> {
  std::cmatch match;
  if (std::regex_match(value.begin(), value.end(), match, m_mask)) {
    return match;
  }
  return {};
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::sign() const noexcept -> bool {
  return m_mantissa[m_bits - 1];
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::conversion(const std::cmatch& match) -> void {
  std::string number = match[2].str() + match[3].str() + std::string(F - match[3].length(), '0');

  std::vector<int> digits(number.size());
  for (const char& digit : number) {
    digits.push_back(digit - '0');
  }
  
  std::size_t bit{}, value{}, carry{};

  m_mantissa.reset();
  while(!digits.empty()) {
    m_mantissa.set(bit++, digits.back() & 1);

    carry = {};
    for (int & digit : digits) {
      value = carry * 10 + digit;
      digit = value / 2;
      carry = value % 2;
    };

    digits.erase(digits.begin(), std::find_if(digits.begin(), digits.end(), [](const int& digit) { return digit != 0; }));
  }

  if (match[1] == "-") {
    *this = -(*this);
  }
}

template <std::size_t I, std::size_t F> auto Decimal<I, F>::abs() noexcept -> Decimal<I, F>& {
  if (sign()) {
    m_mantissa = -m_mantissa;
  }
  return *this;
}
} // namespace utils::finantial
