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
  if (!check_user(a)) {
    ++last_id_;
    user_list_.emplace_back(last_id_, c);
    cout << "User created\n";
  } else {
    cout << "User already exist!\n";
  }
}

bool System::check_user(string_view address) {
  return ranges::any_of(
      user_list_, [&](const User& u) { return check_address(u, address); });
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

void System::disconnect() {
  current_state_ = kGuest;
  cout << "Disconnecting user " << logged_user_.getEmail() << '\n';
  logged_user_ = User();
}

bool System::check_credentials(string_view cred) {
  auto [a, p] = parse_credentials(cred);
  return ranges::any_of(user_list_, [&](const User& u) {
    return check_address(u, a) && check_password(u, p);
  });
}

ItVectorUser System::find_user(int id) {
  return ranges::find_if(user_list_,
                         [&](const User& u) { return check_id(u, id); });
}

ItVectorUser System::find_user(string_view address) {
  return ranges::find_if(
      user_list_, [&](const User& u) { return check_address(u, address); });
}

User System::get_user(int id) { return *(find_user(id)); }

User System::get_user(string_view address) { return *(find_user(address)); }

string System::get_user_name(int id) { return get_user(id).getName(); }

void System::create_server(string_view name) {
  if (!check_server(name)) {
    server_list_.emplace_back(logged_user_.getId(), name);
    Server s{get_server(name)};
    s.add_member(logged_user_);
    cout << "Server created\n";
  } else {
    cout << "There is already a server with that name\n";
  }
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

void System::list_servers() {
  for (const auto& server : server_list_) {
    cout << server.getName() << '\n';
  }
}

void System::remove_server(string_view name) {
  if (check_server(name)) {
    Server s{get_server(name)};
    if (check_owner(s, logged_user_)) {
      server_list_.erase(find_server(name));
      cout << "Server '" << name << "' was removed\n";
    } else {
      cout << "You can't remove a server that isn't yours\n";
    }
  } else {
    print_abscent(name);
  }
}

void System::enter_server(const ServerDetails& sd) {
  if (check_server(sd.name)) {
    Server s{get_server(sd.name)};
    if (sd.invite_code == s.getInvite() || check_owner(s, logged_user_)) {
      current_server_ = s;
      current_state_ = kJoinedServer;
      cout << "Joined server with success\n";
      if (!check_member(s, logged_user_)) {
        s.add_member(logged_user_);
      }
    } else {
      cout << "Server requires invite code\n";
    }
  } else {
    print_abscent(sd.name);
  }
}

void System::leave_server() {
  if (current_server_.getName() != Server().getName()) {
    cout << "Leaving server '" << current_server_.getName() << "'\n";
    current_server_ = Server();
  } else {
    cout << "You are not visualising any server\n";
  }
}

void System::list_participants() {
  ranges::for_each(current_server_.getMembers(),
                   [this](int id) { cout << get_user_name(id) << '\n'; });
}

bool System::check_server(string_view name) {
  return ranges::any_of(server_list_,
                        [&](const Server& s) { return check_name(s, name); });
}

ItVectorServer System::find_server(string_view name) {
  return ranges::find_if(server_list_,
                         [&](const Server& s) { return check_name(s, name); });
}

Server System::get_server(string_view name) { return *(find_server(name)); }

bool check_id(const User& u, int id) { return u.getId() == id; }

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

bool check_owner(const Server& s, const User& u) {
  return s.getOwner() == u.getId();
}

bool check_member(const Server& s, const User& u) {
  return ranges::any_of(s.getMembers(),
                        [&](int id) { return id == u.getId(); });
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
