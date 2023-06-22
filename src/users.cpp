// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "users.h"

#include <ranges>

namespace concordo::user {

namespace views = std::views;

ostream& operator<<(ostream& os, const Name& n) { return os << n.name; }

ostream& operator<<(ostream& os, const EmailAddress& a) {
  return os << a.email_address;
}

ostream& operator<<(ostream& os, const Password& p) { return os << p.password; }

bool check_address(const User& u, const EmailAddress& a) {
  return u.getEmail().email_address == a.email_address;
}

bool check_password(const User& u, const Password& p) {
  return u.getPassword().password == p.password;
}

tuple<Name, EmailAddress, Password> parse_new_credentials(string_view cred) {
  Name n;
  EmailAddress a;
  Password p;
  int i{0};
  for (const auto word : views::split(cred, ' ')) {
    switch (i) {
      case 0:
        a.email_address = {word.begin(), word.end()};
        break;
      case 1:
        p.password = {word.begin(), word.end()};
        break;
      default:
        n.name += {word.begin(), word.end()};
        break;
    }
    ++i;
  }
  return {n, a, p};
}

pair<EmailAddress, Password> parse_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  int i{0};
  for (const auto word : views::split(cred, ' ')) {
    if (i == 0) {
      a.email_address = {word.begin(), word.end()};
    } else {
      p.password = {word.begin(), word.end()};
    }
    ++i;
  }
  return {a, p};
}

}  // namespace concordo::user
