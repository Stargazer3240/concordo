// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "users.h"

#include <ranges>

namespace concordo::user {

namespace views = std::views;

bool check_address(const User& u, string_view a) { return u.getEmail() == a; }

bool check_password(const User& u, string_view p) {
  return u.getPassword() == p;
}

tuple<Name, EmailAddress, Password> parse_new_credentials(string_view cred) {
  Name n;
  EmailAddress a;
  Password p;
  for (int i{0}; const auto word : views::split(cred, ' ')) {
    switch (i) {
      case 0:
        a = {word.begin(), word.end()};
        break;
      case 1:
        p = {word.begin(), word.end()};
        break;
      default:
        n += {word.begin(), word.end()};
        break;
    }
    ++i;
  }
  return {n, a, p};
}

pair<EmailAddress, Password> parse_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  for (int i{0}; const auto word : views::split(cred, ' ')) {
    if (i == 0) {
      a = {word.begin(), word.end()};
    } else {
      p = {word.begin(), word.end()};
    }
    ++i;
  }
  return {a, p};
}

}  // namespace concordo::user
