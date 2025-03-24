#pragma once

#include <bitset>
#include <cstddef>
#include <optional>
#include <regex>
#include <string_view>

namespace utils::finantial {
template <std::size_t I = 24, std::size_t F = 8> class Decimal final {
public:
  Decimal(const std::string_view& value = {});
  Decimal(const Decimal&) noexcept = default;
  Decimal(Decimal&&) noexcept = default;
  ~Decimal() = default;

  auto operator=(const std::string_view& value) -> Decimal&;
  auto operator=(const Decimal&) noexcept -> Decimal& = default;
  auto operator=(Decimal&&) noexcept -> Decimal& = default;

  [[nodiscard]] auto operator<(const Decimal& other) const noexcept -> bool;
  [[nodiscard]] auto operator>(const Decimal& other) const noexcept -> bool;
  [[nodiscard]] auto operator>=(const Decimal& other) const noexcept -> bool;
  [[nodiscard]] auto operator<=(const Decimal& other) const noexcept -> bool;
  [[nodiscard]] auto operator==(const Decimal& other) const noexcept -> bool;
  [[nodiscard]] auto operator!=(const Decimal& other) const noexcept -> bool;

  [[nodiscard]] auto operator-() const noexcept -> Decimal;
  [[nodiscard]] auto operator+(const Decimal& other) const noexcept -> Decimal;
  [[nodiscard]] auto operator-(const Decimal& other) const noexcept -> Decimal;
  [[nodiscard]] auto operator*(const Decimal& other) const noexcept -> Decimal;
  [[nodiscard]] auto operator/(const Decimal& other) const noexcept -> Decimal;
  [[nodiscard]] auto operator%(const Decimal& other) const noexcept -> Decimal;

  auto operator+=(const Decimal& other) noexcept -> Decimal&;
  auto operator-=(const Decimal& other) noexcept -> Decimal&;
  auto operator*=(const Decimal& other) noexcept -> Decimal&;
  auto operator/=(const Decimal& other) noexcept -> Decimal&;
  auto operator%=(const Decimal& other) noexcept -> Decimal&;

  template <std::size_t _I, std::size_t _F>
  friend auto operator<<(std::ostream& os, Decimal<_I, _F> decimal) -> std::ostream&;

  explicit operator std::string() const noexcept;

private:
  static inline const std::regex m_mask{"^([+-]?)(?:0*)(\\d{0," + std::to_string(I) + "})(?:\\.(\\d{0," + std::to_string(F) + "})(?:\\d*))?$"};

  static constexpr std::size_t m_bits = (I + F) * 3.3219280948873623478703194294894 + 3;
  static constexpr std::bitset<m_bits> m_bit_ten{0b1010};

  std::bitset<m_bits> m_mantissa{};

  [[nodiscard]] auto parse(const std::string_view& value) const -> std::optional<std::cmatch>;
  [[nodiscard]] auto sign() const noexcept -> bool;
  auto conversion(const std::cmatch& match) -> void;
  auto abs() noexcept -> Decimal&;
};
} // namespace utils::finantial

#include "decimal.tpp"
