// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "users.h"

namespace concordo::system {

using std::cout;

void System::create_user(string_view args) {
  auto [a, p, n] = parse_new_credentials(args);
  if (check_new_user(a)) {
  }
}

void System::user_login(string_view cred) {
  EmailAddress a{(parse_credentials(cred)).first};
  if (check_credentials(cred)) {
    logged_user_ = get_user(a);
    cout << "Logged-in as " << a.email_address << '\n';
  } else {
    cout << "User or password invalid!\n";
  }
}

bool System::check_new_user(EmailAddress a) {
  return std::ranges::none_of(user_list_,
                              [&](User& u) { return check_address(u, a); });
}

User System::get_user(EmailAddress a) {
  return *(std::ranges::find_if(user_list_,
                                [&](User& u) { return check_address(u, a); }));
}

bool System::check_credentials(string_view cred) {
  auto [a, p] = parse_credentials(cred);
  return std::ranges::any_of(user_list_, [&](const User& u) {
    return check_address(u, a) && check_password(u, p);
  });
}

bool check_address(const User& u, const EmailAddress& a) {
  return u.getEmail().email_address == a.email_address;
}

bool check_password(const User& u, const Password& p) {
  return u.getPassword().password == p.password;
}

tuple<EmailAddress, Password, Name> parse_new_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  Name n;
  int i{0};
  for (const auto word : std::views::split(cred, ' ')) {
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
  return {a, p, n};
}

pair<EmailAddress, Password> parse_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  int i{0};
  for (const auto word : std::views::split(cred, ' ')) {
    if (i == 0) {
      a.email_address = {word.begin(), word.end()};
    } else {
      p.password = {word.begin(), word.end()};
    }
    ++i;
  }
  return {a, p};
}

void quit() {
  cout << "Leaving Concordo\n";
  std::exit(0);
}

}  // namespace concordo::system
