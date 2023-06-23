// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>
#include <ranges>

#include "users.h"

namespace concordo::system {

namespace ranges = std::ranges;
namespace views = std::views;
using std::cout;

void System::create_user(string_view args) {
  auto [n, a, p] = parse_new_credentials(args);
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
  return ranges::none_of(user_list_,
                         [&](User& u) { return check_address(u, address); });
}

void System::user_login(string_view cred) {
  string address{(parse_credentials(cred)).first};
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
    return check_address(u, a) && check_password(u, p);
  });
}

User System::get_user(string_view address) {
  return *(ranges::find_if(user_list_,
                           [&](User& u) { return check_address(u, address); }));
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
      server_list_, [&](Server& s) { return check_name(s, name); });
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
      server_list_, [&](Server& s) { return check_name(s, name); }));
}

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

bool check_name(const Server& s, string_view name) {
  return s.getName() == name;
}

void print_abscent(string_view name) {
  cout << "Server '" << name << "' doesn't exist\n";
}

void print_no_permission(string_view sv) {
  cout << "You can't change the " << sv << " of a server that isn't yours\n";
}

void print_info_changed(tuple<string_view, string_view, string_view> info) {
  cout << get<0>(info) << " of server '" << get<1>(info) << "' was "
       << get<2>(info) << "!\n";
}

void quit() {
  cout << "Leaving Concordo\n";
  std::exit(0);
}

}  // namespace concordo::system
