// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>

namespace concordo::user {

using std::ostream, std::string_view, std::string, std::pair, std::tuple;

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

bool check_address(const User& u, const EmailAddress& a);
bool check_password(const User& u, const Password& p);

tuple<Name, EmailAddress, Password> parse_new_credentials(string_view cred);
pair<EmailAddress, Password> parse_credentials(string_view cred);

}  // namespace concordo::user

#endif  // USERS_H
