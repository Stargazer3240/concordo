// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

namespace concordo {

using std::string_view, std::string, std::begin, std::find;

class EmailAddress {
 public:
  EmailAddress() = default;
  explicit EmailAddress(string_view a)
      : username_{parse_username(a)},
        mail_server_{parse_server(a)},
        domain_{parse_domain(a)} {}

  static string parse_username(string_view a) {
    return {begin(a), begin(a) + a.find('@')};
  };
  static string parse_server(string_view a) {
    return {begin(a) + a.find('@') + 1, begin(a) + a.find('.')};
  };
  static string parse_domain(string_view a) {
    return {begin(a) + a.find('.') + 1, end(a)};
  };
  [[nodiscard]] string compose_address() const {
    return username_ + '@' + mail_server_ + '.' + domain_;
  }

 private:
  string username_;
  string mail_server_;
  string domain_;
};

class Password {
 public:
  Password() = default;
  explicit Password(string_view p) : password_{p} {}

 private:
  string password_;
};

class User {
 public:
  User() = default;
  User(string_view n, EmailAddress a, Password p)
      : name_{n}, address_{std::move(a)}, password_{std::move(p)} {}

  [[nodiscard]] int getId() const { return id_; }
  [[nodiscard]] string getName() const { return name_; }
  [[nodiscard]] string getEmail() const {
    return this->address_.compose_address();
  }
  [[nodiscard]] Password getPassword() const { return password_; }

 private:
  int id_{};
  string name_;
  EmailAddress address_;
  Password password_;
};

}  // namespace concordo

#endif  // USERS_H
