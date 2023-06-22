// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>
#include <utility>

namespace concordo::user {

using std::ostream, std::string_view, std::string;

struct Name {
  string name;
};

ostream& operator<<(ostream& os, const Name& n);

struct EmailAddress {
  string email_address;
};

ostream& operator<<(ostream& os, const EmailAddress& a);

struct Password {
  string password;
};

ostream& operator<<(ostream& os, const Password& p);

class User {
 public:
  User() = default;
  User(int id, Name n, EmailAddress a, Password p)
      : id_{id},
        name_{std::move(n)},
        address_{std::move(a)},
        password_{std::move(p)} {}

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
