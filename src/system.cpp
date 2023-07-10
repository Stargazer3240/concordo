// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_set>
#include <utility>

#include "channels.h"

namespace concordo {

using std::array, std::cin, std::cout, std::getline, std::unique_ptr;
namespace ranges = std::ranges;
namespace views = std::views;
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
    print_unable();
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
    print_unable();
  }
}

void System::run_channel_cmd(const CommandLine& cl) {
  if (check_command(channel_commands_, cl.command)) {
    if (cl.command == "send-message") {
      send_message(cl.arguments);
    } else {
      list_messages();
    }
  } else {
    print_unable();
  }
}

bool System::check_all_commands(string_view cmd) const {
  const array<unordered_set<string>, 4> all_cmds{
      guest_commands_, logged_commands_, server_commands_, channel_commands_};
  return ranges::any_of(all_cmds, [=](const unordered_set<string>& s) {
    return s.contains(string(cmd));
  });
}

// User related commands.
bool System::check_credentials(string_view cred) const {
  const UserCredentials c = parse_credentials(cred);
  return ranges::any_of(users_list_, [&](const User& u) {
    return check_address(u, c.address) && check_password(u, c.password);
  });
}

auto System::find_user(int id) const {
  return ranges::find_if(users_list_,
                         [=](const User& u) { return check_id(u, id); });
}

auto System::find_user(string_view address) {
  return ranges::find_if(
      users_list_, [=](const User& u) { return check_address(u, address); });
}

string System::get_user_name(int id) const { return find_user(id)->getName(); }

void System::create_user(string_view args) {
  const UserCredentials c = parse_new_credentials(args);
  if (!any_of<const vector<User>&>(users_list_, c.address, check_address)) {
    ++last_id_;
    users_list_.emplace_back(last_id_, c);
    cout << "User created\n";
  } else {
    cout << "User already exist!\n";
  }
}

void System::user_login(string_view cred) {
  const string a{(parse_credentials(cred)).address};
  if (check_credentials(cred)) {
    current_user_ = &*find_user(a);
    current_state_ = kLogged_In;
    cout << "Logged-in as " << a << '\n';
  } else {
    cout << "User or password invalid!\n";
  }
}

void System::disconnect() {
  if (current_state_ > kGuest) {
    current_state_ = kGuest;
    cout << "Disconnecting user " << *current_user_ << '\n';
    current_server_ = nullptr;
    current_user_ = nullptr;
  } else {
    cout << "Not connected\n";
  }
}

// Server related commands.
auto System::find_server(string_view name) {
  return find_if(servers_list_.begin(), servers_list_.end(),
                 [=](const Server& s) { return check_name(s, name); });
}

void System::create_server(string_view name) {
  if (!any_of<const vector<Server>&>(servers_list_, name, check_name)) {
    servers_list_.emplace_back(current_user_->getId(), name);
    cout << "Server created\n";
  } else {
    cout << "There is already a server with that name\n";
  }
}

void System::change_description(const ServerDetails& sd) {
  if (any_of<const vector<Server>&>(servers_list_, sd.name, check_name)) {
    auto it{find_server(sd.name)};
    if (it->check_owner(*current_user_)) {
      it->change_description(sd.description);
      print_info_changed(make_tuple("Description", sd.name, "changed"));
    } else {
      print_no_permission("description");
    }
  } else {
    print_abscent(sd.name);
  }
}

void System::change_invite(const ServerDetails& sd) {
  if (any_of<const vector<Server>&>(servers_list_, sd.name, check_name)) {
    auto it{find_server(sd.name)};
    if (it->check_owner(*current_user_)) {
      it->change_invite(sd.invite_code);
      if (!sd.invite_code.empty()) {
        print_info_changed("Invite code", *it, "changed");
      } else {
        print_info_changed("Invite code", *it, "removed");
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
    server.print();
  }
}

void System::remove_server(string_view name) {
  if (any_of<const vector<Server>&>(servers_list_, name, check_name)) {
    auto it{find_server(name)};
    if (it->check_owner(*current_user_)) {
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
  if (any_of<const vector<Server>&>(servers_list_, sd.name, check_name)) {
    auto it{find_server(sd.name)};
    if (!it->has_invite() || it->check_owner(*current_user_) ||
        it->check_invite(sd.invite_code)) {
      current_state_ = kJoinedServer;
      cout << "Joined server with success\n";
      if (!it->check_member(*current_user_)) {
        it->add_member(*current_user_);
      }
      current_server_ = &*it;
    } else {
      cout << "Server requires invite code\n";
    }
  } else {
    print_abscent(sd.name);
  }
}

void System::leave_server() {
  if (current_state_ >= kJoinedServer) {
    cout << "Leaving server '" << *current_server_ << "'\n";
    current_server_ = nullptr;
    current_state_ = kLogged_In;
  } else {
    cout << "You are not visualising any server\n";
  }
}

void System::list_participants() const {
  ranges::for_each(current_server_->getMembers(),
                   [this](int id) { cout << get_user_name(id) << '\n'; });
}

// Channel related commands.
bool System::check_channel(const ChannelDetails& cd) const {
  return current_server_->check_channel(cd);
}

constexpr auto System::find_channel(string_view name) {
  return find_if(
      current_server_->getChannels().begin(),
      current_server_->getChannels().end(),
      [=](const unique_ptr<Channel>& c) { return c->check_name(name); });
}

void System::list_channels() const {
  cout << "#Text Channels\n";
  current_server_->list_text_channels();
  cout << "#Voice Channels\n";
  current_server_->list_voice_channels();
}

void System::create_channel(string_view args) {
  const ChannelDetails cd = parse_details(args);
  if (!check_channel(cd)) {
    if (cd.type == "text") {
      auto c = make_unique<TextChannel>(cd.name);
      current_server_->create_channel(std::move(c));
    } else if (cd.type == "voice") {
      auto c = make_unique<VoiceChannel>(cd.name);
      current_server_->create_channel(std::move(c));
    }
    print_channel_created(cd);
  } else {
    print_channel_exists(cd);
  }
}

void System::enter_channel(string_view name) {
  if (current_server_->any_of(name)) {
    auto it{find_channel(name)};
    current_state_ = kJoinedChannel;
    current_channel_ = &(**it);
    cout << "Joined '" << name << "' channel\n";
  } else {
    cout << "Channel '" << name << "' doesn't exist\n";
  }
}

void System::leave_channel() {
  if (current_state_ == kJoinedChannel) {
    cout << "Leaving channel\n";
    current_channel_ = nullptr;
    current_state_ = kJoinedServer;
  } else {
    cout << "You are not visualizing any channel\n";
  }
}

void System::send_message(string_view msg) {
  if (check_channel_type<TextChannel>(*current_channel_)) {
    auto* tc = dynamic_cast<TextChannel*>(current_channel_);
    current_user_->send_message(tc, msg);
  } else if (check_channel_type<VoiceChannel>(*current_channel_)) {
    auto* vc = dynamic_cast<VoiceChannel*>(current_channel_);
    current_user_->send_message(vc, msg);
  }
  cout << "Message sent\n";
}

void System::list_messages() {
  if (check_channel_type<TextChannel>(*current_channel_)) {
    auto tc = dynamic_cast<TextChannel&>(*current_channel_);
    if (tc.empty()) {
      cout << "No message to show\n";
    } else {
      ranges::for_each(tc.getMessages(),
                       [this](const Message& m) { print_message(m); });
    }
  } else if (check_channel_type<VoiceChannel>(*current_channel_)) {
    auto vc = dynamic_cast<VoiceChannel&>(*current_channel_);
    if (vc.empty()) {
      cout << "No message to show\n";
    } else {
      print_message(vc.getMessage());
    }
  }
}

void System::print_message(const Message& m) const {
  cout << get_user_name(m.getId()) << "<" << m.getDateTime()
       << ">: " << m.getContent() << '\n';
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
  enum Command {
    kDescription,  // "set-server-description"
    kInvite,       // "set-server-invite-code"
    kEnter         // "enter-server"
  };
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
bool check_id(const User& u, int id) { return u.check_id(id); }

bool check_address(const User& u, string_view a) { return u.check_address(a); }

bool check_password(const User& u, string_view p) {
  return u.check_password(p);
}

UserCredentials parse_new_credentials(string_view cred) {
  string addr;
  string pass;
  string name;
  for (int i{0}; const auto w : views::split(cred, ' ')) {
    switch (i) {
      case 0:
        addr = {w.begin(), w.end()};
        break;
      case 1:
        pass = {w.begin(), w.end()};
        break;
      default:
        name += {w.begin(), w.end()};
        name += " ";
        break;
    }
    ++i;
  }
  name.pop_back();
  return {addr, pass, name};
}

UserCredentials parse_credentials(string_view cred) {
  string addr;
  string pass;
  for (int i{0}; const auto w : views::split(cred, ' ')) {
    if (i == 0) {
      addr = {w.begin(), w.end()};
    } else {
      pass = {w.begin(), w.end()};
    }
    ++i;
  }
  return {addr, pass, ""};
}

// Server related helping functions.
bool check_name(const Server& s, string_view name) {
  return s.check_name(name);
}

// Channel related helping functions.
bool check_channel_name(const unique_ptr<Channel>& c, string_view name) {
  return c->check_name(name);
}

ChannelDetails parse_details(string_view args) {
  string name;
  string type;
  for (int i{0}; const auto w : views::split(args, ' ')) {
    if (i == 0) {
      name = {w.begin(), w.end()};
    } else {
      type = {w.begin(), w.end()};
    }
    ++i;
  }
  return {name, type};
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

void print_info_changed(string_view wc1, const Server& s, string_view wc2) {
  cout << wc1 << " of server '" << s << "' was " << wc2 << "!\n";
}

void print_unable() { cout << "You can't do that right now\n"; }
void print_channel_created(const ChannelDetails& cd) {
  if (cd.type == "text") {
    print_channel_created("Text", cd.name);
  } else if (cd.type == "voice") {
    print_channel_created("Voice", cd.name);
  }
}

void print_channel_created(string_view type, string_view name) {
  cout << type << " Channel '" << name << "' created\n";
}

void print_channel_exists(const ChannelDetails& cd) {
  if (cd.type == "text") {
    print_channel_exists("Text", cd.name);
  } else if (cd.type == "voice") {
    print_channel_exists("Voice", cd.name);
  }
}

void print_channel_exists(string_view type, string_view name) {
  cout << type << " Channel '" << name << "' already exists\n";
}

}  // namespace concordo
