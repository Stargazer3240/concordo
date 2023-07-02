// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>

#include "users.h"

namespace concordo {

using std::array, std::cin, std::cout, std::getline;
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
    if (cmd == "quit") {
      cout << "Leaving Concordo\n";
      break;
    }
    if (check_args(cmd_line)) {
      args = parse_args(cmd_line);
    }
    this->run({cmd, args});
  }
}

void System::run(const CommandLine& cl) {
  // Disconnect can be run at any state that isn't guest.
  if (cl.command == "disconnect") {
    disconnect();
  } else if (check_all_commands(cl.command)) {
    switch (current_state_) {
      case kGuest:
        run_guest_cmd(cl);
        break;
      case kLogged_In:
        run_logged_cmd(cl);
        break;
      case kJoinedServer:
        run_server_cmd(cl);
        break;
      case kJoinedChannel:
        run_channel_cmd(cl);
        break;
    }
  } else {
    cout << "Invalid command\n";
  }
}

void System::run_guest_cmd(const CommandLine& cl) {
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

void System::run_logged_cmd(const CommandLine& cl) {
  if (check_command(logged_commands_, cl.command)) {
    if (cl.command == "create-server") {
      create_server(cl.arguments);
    } else if (cl.command == "set-server-desc") {
      change_description(parse_details(cl.arguments, 0));
    } else if (cl.command == "set-server-invite-code") {
      change_invite(parse_details(cl.arguments, 1));
    } else if (cl.command == "list-servers") {
      list_servers();
    } else if (cl.command == "remove-server") {
      remove_server(cl.arguments);
    } else {
      enter_server(parse_details(cl.arguments, 2));
    }
  } else {
    cout << "You can't do that right now\n";
  }
}

void System::run_server_cmd(const CommandLine& cl) {
  if (check_command(server_commands_, cl.command)) {
    if (cl.command == "leave-server") {
      leave_server();
    } else if (cl.command == "list-participants") {
      list_participants();
    } else if (cl.command == "list-channels") {
      list_channels();
    } else if (cl.command == "create-channel") {
      create_channel(cl.arguments);
    } else if (cl.command == "enter-channel") {
      enter_channel(cl.arguments);
    } else if (cl.command == "leave-channel") {
      leave_channel();
    }
  } else {
    cout << "You can't do that right now\n";
  }
}

void System::run_channel_cmd(const CommandLine& cl) {
  if (check_command(channel_commands_, cl.command)) {
    if (cl.command == "leave-server") {
      send_message(cl.arguments);
    } else {
      list_messages();
    }
  } else {
    cout << "You can't do that right now\n";
  }
}

bool System::check_all_commands(string_view cmd) {
  const array<unordered_set<string>, 4> all_cmds{
      guest_commands_, logged_commands_, server_commands_, channel_commands_};
  return ranges::any_of(all_cmds, [=](const unordered_set<string>& s) {
    return s.contains(string(cmd));
  });
}

// User related commands.
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

auto System::find_user(int id) const {
  return find_if(users_list_.begin(), users_list_.end(),
                 [&](const User& u) { return check_id(u, id); });
}

auto System::find_user(string_view address) const {
  return find_if(users_list_.begin(), users_list_.end(),
                 [&](const User& u) { return check_address(u, address); });
}

string System::get_user_name(int id) const { return find_user(id)->getName(); }

void System::create_user(string_view args) {
  const auto [a, p, n] = parse_new_credentials(args);
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
    logged_user_ = *find_user(address);
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
    current_server_ = Server();
    logged_user_ = User();
  } else {
    cout << "Not connected\n";
  }
}

// Server related commands.
bool System::check_server(string_view name) const {
  return ranges::any_of(servers_list_,
                        [&](const Server& s) { return check_name(s, name); });
}

auto System::find_server(string_view name) {
  return find_if(servers_list_.begin(), servers_list_.end(),
                 [&](const Server& s) { return check_name(s, name); });
}

void System::create_server(string_view name) {
  if (!check_server(name)) {
    servers_list_.emplace_back(logged_user_.getId(), name);
    servers_list_.back().add_member(logged_user_);
    cout << "Server created\n";
  } else {
    cout << "There is already a server with that name\n";
  }
}

void System::change_description(const ServerDetails& sd) {
  if (check_server(sd.name)) {
    auto it{find_server(sd.name)};
    if (check_owner(*it, logged_user_)) {
      it->setDescription(sd.description);
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
    auto it{find_server(sd.name)};
    if (check_owner(*it, logged_user_)) {
      it->setInvite(sd.invite_code);
      if (!sd.invite_code.empty()) {
        print_info_changed(make_tuple("Invite code", it->getName(), "changed"));
      } else {
        print_info_changed(make_tuple("Invite code", it->getName(), "removed"));
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
    auto it{find_server(name)};
    if (check_owner(*it, logged_user_)) {
      servers_list_.erase(it);
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
    auto it{find_server(sd.name)};
    if (it->getInvite().empty() || check_owner(*it, logged_user_) ||
        sd.invite_code == it->getInvite()) {
      current_state_ = kJoinedServer;
      cout << "Joined server with success\n";
      if (!check_member(*it, logged_user_)) {
        it->add_member(logged_user_);
      }
      current_server_ = *it;
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

// System related helper functions.
bool check_command(const unordered_set<string>& s, string_view c) {
  return s.contains(string(c));
}

string parse_cmd(string_view cmd_line) {
  string cmd;
  for (const auto c : views::split(cmd_line, ' ')) {
    cmd = {c.begin(), c.end()};
    break;
  }
  return cmd;
}

bool check_args(string_view cmd_line) {
  ranges::split_view sv{cmd_line, ' '};
  return ranges::distance(sv.begin(), sv.end()) > 1;
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
  // "set-server-description", "set-server-invite-code" and "enter-server".
  enum Command { kDescription, kInvite, kEnter };
  string name;
  string desc;
  string invite;
  for (int i{0}; const auto a : views::split(args, ' ')) {
    if (cmd == kEnter || cmd == kInvite) {
      if (i == 0) {
        name = {a.begin(), a.end()};
      } else {
        invite = {a.begin(), a.end()};
      }
    } else {
      if (i == 0) {
        name = {a.begin(), a.end()};
      } else {
        desc += {a.begin(), a.end()};
      }
    }

    ++i;
  }
  return {name, desc, invite};
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
