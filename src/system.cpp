// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>

#include "users.h"

namespace concordo::system {

namespace ranges = std::ranges;
using std::cout;

void System::create_user(string_view args) {
  auto [n, a, p] = parse_new_credentials(args);
  if (check_user(a)) {
    ++last_id_;
    user_list_.emplace_back(last_id_, n, a, p);
    cout << "User created\n";
  } else {
    cout << "User already exist!\n";
  }
}

void System::user_login(string_view cred) {
  EmailAddress a{(parse_credentials(cred)).first};
  if (check_credentials(cred)) {
    logged_user_ = get_user(a);
    current_state_ = kLogged_In;
    cout << "Logged-in as " << a.email_address << '\n';
  } else {
    cout << "User or password invalid!\n";
  }
}

bool System::check_user(EmailAddress a) {
  return ranges::none_of(user_list_,
                         [&](User& u) { return check_address(u, a); });
}

User System::get_user(EmailAddress a) {
  return *(ranges::find_if(user_list_,
                           [&](User& u) { return check_address(u, a); }));
}

bool System::check_credentials(string_view cred) {
  auto [a, p] = parse_credentials(cred);
  return ranges::any_of(user_list_, [&](const User& u) {
    return user::check_address(u, a) && check_password(u, p);
  });
}

void System::disconnect() {
  if (current_state_ == kLogged_In) {
    current_state_ = kGuest;
    cout << "Disconnecting user " << logged_user_.getEmail() << '\n';
    logged_user_ = User();
  } else {
    cout << "Not connected\n";
  }
}

void System::create_server(string_view name) {
  if (check_server(name)) {
  }
}

bool System::check_server(string_view n) {
  return ranges::none_of(server_list_,
                         [&](Server& s) { return check_name(s, n); });
}

void quit() {
  cout << "Leaving Concordo\n";
  std::exit(0);
}

}  // namespace concordo::system
