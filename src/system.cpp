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
  auto [n, a, p] = user::parse_new_credentials(args);
  Credentials c(n, a, p);
  if (check_user(a)) {
    ++last_id_;
    user_list_.emplace_back(last_id_, c);
    cout << "User created\n";
  } else {
    cout << "User already exist!\n";
  }
}

bool System::check_user(string_view address) {
  return ranges::none_of(
      user_list_, [&](User& u) { return user::check_address(u, address); });
}

void System::user_login(string_view cred) {
  string address{(user::parse_credentials(cred)).first};
  if (check_credentials(cred)) {
    logged_user_ = get_user(address);
    current_state_ = kLogged_In;
    cout << "Logged-in as " << address << '\n';
  } else {
    cout << "User or password invalid!\n";
  }
}

bool System::check_credentials(string_view cred) {
  auto [a, p] = parse_credentials(cred);
  return ranges::any_of(user_list_, [&](const User& u) {
    return user::check_address(u, a) && user::check_password(u, p);
  });
}

User System::get_user(string_view address) {
  return *(ranges::find_if(
      user_list_, [&](User& u) { return user::check_address(u, address); }));
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
    server_list_.emplace_back(logged_user_.getId(), name);
    cout << "Server created\n";
  } else {
    cout << "There is already a server with that name\n";
  }
}

bool System::check_server(string_view name) {
  return ranges::none_of(
      server_list_, [&](Server& s) { return server::check_name(s, name); });
}

void System::change_description(const ServerDetails& sd) {
  if (check_server(sd.name)) {
    Server s{get_server(sd.name)};
    if (check_owner(s, logged_user_)) {
      s.setDescription(sd.description);
      print_info_changed(make_tuple("Description", s.getName(), "changed"));
    } else {
      print_no_permission("description");
    }
  } else {
    print_abscent(sd.name);
  }
}

void System::change_invite(const ServerDetails& sd) {
  if (check_server(sd.name)) {
    Server s{get_server(sd.name)};
    if (check_owner(s, logged_user_)) {
      s.setInvite(sd.invite_code);
      if (!sd.invite_code.empty()) {
        print_info_changed(make_tuple("Invite code", s.getName(), "changed"));
      } else {
        print_info_changed(make_tuple("Invite code", s.getName(), "removed"));
      }
    } else {
      print_no_permission("invite code");
    }
  } else {
    print_abscent(sd.name);
  }
}

Server System::get_server(string_view name) {
  return *(ranges::find_if(
      server_list_, [&](Server& s) { return server::check_name(s, name); }));
}

void quit() {
  cout << "Leaving Concordo\n";
  std::exit(0);
}

}  // namespace concordo::system
