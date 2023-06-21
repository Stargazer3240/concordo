// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>
#include <utility>

namespace concordo::user {

using std::string_view, std::string;

struct EmailAddress {
  string email_address;
};

struct Password {
  string password;
};

struct Name {
  string name;
};

class User {
 public:
  User() = default;
  User(Name n, EmailAddress a, Password p)
      : name_{std::move(n)}, address_{std::move(a)}, password_{std::move(p)} {}

  [[nodiscard]] int getId() const { return id_; }
  [[nodiscard]] Name getName() const { return name_; }
  [[nodiscard]] EmailAddress getEmail() const { return address_; }
  [[nodiscard]] Password getPassword() const { return password_; }

 private:
  int id_{};
  Name name_;
  EmailAddress address_;
  Password password_;
};

}  // namespace concordo::user

#endif  // USERS_H
