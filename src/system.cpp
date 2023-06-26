// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>

#include "users.h"

namespace concordo {

using std::cin, std::cout, std::getline, std::string;
namespace ranges = std::ranges;
namespace views = std::views;
using Credentials = user::Credentials;
using enum System::SystemState;

// Main system loop.
void System::init() {
  string cmd_line;
  string cmd;
  string args;
  while (getline(cin, cmd_line)) {
    cmd = parse_cmd(cmd_line);
    args = parse_args(cmd_line);
    this->run({cmd, args});
  }
}

void System::run(const CommandLine& cl) {
  if (cl.command == "quit") {
    quit();
  } else {
    switch (current_state_) {
      case kGuest:
        run_guest(cl);
        break;
      case kLogged_In:
        run_logged(cl);
        break;
      case kJoinedServer:
        run_server(cl);
        break;
      case kJoinedChannel:
        // TODO
        break;
    }
  }
}

void System::run_guest(const CommandLine& cl) {
  if (check_command(guest_commands_, cl.command)) {
    if (cl.command == "create-user") {
      create_user(cl.arguments);
    } else {
      user_login(cl.arguments);
    }
  } else {
    cout << "You have to login to run that command\n";
  }
}

void System::run_logged(const CommandLine& cl) {
  if (check_command(logged_commands_, cl.command)) {
    if (cl.command == "disconnect") {
      disconnect();
    } else if (cl.command == "create-server") {
      create_server(cl.arguments);
    } else if (cl.command == "set-server-desc") {
      change_description(parse_details(cl.arguments, 1));
    } else if (cl.command == "set-server-invite-code") {
      change_invite(parse_details(cl.arguments, 2));
    } else if (cl.command == "list-servers") {
      list_servers();
    } else if (cl.command == "remove-server") {
      remove_server(cl.arguments);
    } else {
      enter_server(parse_details(cl.arguments));
    }
  } else {
    cout << "You are not visualizing any server\n";
  }
}

void System::run_server(const CommandLine& cl) {
  if (check_command(server_commands_, cl.command)) {
    if (cl.command == "leave-server") {
      leave_server();
    } else if (cl.command == "list-participants") {
      list_participants();
    }
  } else {
    cout << "You are not visualizing any channel\n";
  }
}

// User related commands.
void System::create_user(string_view args) {
  const auto [n, a, p] = parse_new_credentials(args);
  const Credentials c(n, a, p);
  if (!check_user(a)) {
    ++last_id_;
    users_list_.emplace_back(last_id_, c);
    cout << "User created\n";
  } else {
    cout << "User already exist!\n";
  }
}

void System::user_login(string_view cred) {
  const string address{(parse_credentials(cred)).first};
  if (check_credentials(cred)) {
    logged_user_ = get_user(address);
    current_state_ = kLogged_In;
    cout << "Logged-in as " << address << '\n';
  } else {
    cout << "User or password invalid!\n";
  }
}

void System::disconnect() {
  if (current_state_ > kGuest) {
    current_state_ = kGuest;
    cout << "Disconnecting user " << logged_user_.getEmail() << '\n';
    logged_user_ = User();
  } else {
    cout << "Not connected\n";
  }
}

bool System::check_credentials(string_view cred) const {
  const pair ap = parse_credentials(cred);
  const EmailAddress a{ap.first};
  const Password p{ap.second};
  return ranges::any_of(users_list_, [&](const User& u) {
    return check_address(u, a) && check_password(u, p);
  });
}

bool System::check_user(string_view address) const {
  return ranges::any_of(
      users_list_, [&](const User& u) { return check_address(u, address); });
}

ItVectorUser System::find_user(int id) const {
  return ranges::find_if(users_list_,
                         [&](const User& u) { return check_id(u, id); });
}

ItVectorUser System::find_user(string_view address) const {
  return ranges::find_if(
      users_list_, [&](const User& u) { return check_address(u, address); });
}

User System::get_user(int id) const { return *(find_user(id)); }

User System::get_user(string_view address) const {
  return *(find_user(address));
}

string System::get_user_name(int id) const { return get_user(id).getName(); }

// Server related commands.
void System::create_server(string_view name) {
  if (!check_server(name)) {
    servers_list_.emplace_back(logged_user_.getId(), name);
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
      print_info_changed(make_tuple("Description", sd.name, "changed"));
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

void System::list_servers() const {
  for (const auto& server : servers_list_) {
    cout << server.getName() << '\n';
  }
}

void System::remove_server(string_view name) {
  if (check_server(name)) {
    const Server s{get_server(name)};
    if (check_owner(s, logged_user_)) {
      servers_list_.erase(find_server(name));
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
  if (current_state_ >= kJoinedServer) {
    cout << "Leaving server '" << current_server_.getName() << "'\n";
    current_server_ = Server();
    current_state_ = kLogged_In;
  } else {
    cout << "You are not visualising any server\n";
  }
}

void System::list_participants() const {
  ranges::for_each(current_server_.getMembers(),
                   [this](int id) { cout << get_user_name(id) << '\n'; });
}

bool System::check_server(string_view name) const {
  return ranges::any_of(servers_list_,
                        [&](const Server& s) { return check_name(s, name); });
}

ItVectorServer System::find_server(string_view name) const {
  return ranges::find_if(servers_list_,
                         [&](const Server& s) { return check_name(s, name); });
}

Server System::get_server(string_view name) const {
  return *(find_server(name));
}

// System related helper functions.
void quit() {
  cout << "Leaving Concordo\n";
  std::exit(0);
}

bool check_command(const unordered_set<string>& s, const string& c) {
  return s.contains(c);
}

string parse_cmd(string_view cmd_line) {
  string cmd;
  for (const auto c : views::split(cmd_line, ' ')) {
    cmd = {c.begin(), c.end()};
    break;
  }
  return cmd;
}

string parse_args(string_view cmd_line) {
  string args;
  for (int i{0}; const auto a : views::split(cmd_line, ' ')) {
    if (i > 0) {
      args += {a.begin(), a.end()};
      args += " ";
    }
    ++i;
  }
  args.pop_back();
  return args;
}

ServerDetails parse_details(string_view args, int cmd) {
  string n;
  string d;
  string ic;
  for (int i{0}; const auto a : views::split(args, ' ')) {
    switch (cmd) {
      case 0:
        n = {a.begin(), a.end()};
        break;
      case 1:
        if (i == 0) {
          n = {a.begin(), a.end()};
        } else {
          d += {a.begin(), a.end()};
        }
        break;
      case 2:
        if (i == 0) {
          n = {a.begin(), a.end()};
        } else {
          ic += {a.begin(), a.end()};
        }
        break;
      default:
        break;
    }
    ++i;
  }
  return {n, d, ic};
}

// User related helping functions.
bool check_id(const User& u, int id) { return u.getId() == id; }

bool check_address(const User& u, string_view a) { return u.getEmail() == a; }

bool check_password(const User& u, string_view p) {
  return u.getPassword() == p;
}

tuple<EmailAddress, Password, Name> parse_new_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  Name n;
  for (int i{0}; const auto w : views::split(cred, ' ')) {
    switch (i) {
      case 0:
        a = {w.begin(), w.end()};
        break;
      case 1:
        p = {w.begin(), w.end()};
        break;
      default:
        n += {w.begin(), w.end()};
        n += " ";
        break;
    }
    ++i;
  }
  n.pop_back();
  return {a, p, n};
}

pair<EmailAddress, Password> parse_credentials(string_view cred) {
  EmailAddress a;
  Password p;
  for (int i{0}; const auto w : views::split(cred, ' ')) {
    if (i == 0) {
      a = {w.begin(), w.end()};
    } else {
      p = {w.begin(), w.end()};
    }
    ++i;
  }
  return {a, p};
}

// Server related helping functions.
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

// Print related helping functions.
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

}  // namespace concordo
