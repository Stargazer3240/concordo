// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "system.h"

#include <algorithm>
#include <iostream>
#include <ranges>
#include <string>
#include <type_traits>
#include <unordered_set>

#include "channels.h"

namespace concordo {

using std::array, std::cin, std::cout, std::getline, std::unique_ptr;
namespace ranges = std::ranges;
namespace views = std::views;
using TextChannel = channel::TextChannel;
using VoiceChannel = channel::VoiceChannel;
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
    if (cl.command == "send-message") {
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
  const Credentials c = parse_credentials(cred);
  return ranges::any_of(users_list_, [&](const User& u) {
    return check_address(u, c.address) && check_password(u, c.password);
  });
}

bool System::check_user(string_view address) const {
  return ranges::any_of(
      users_list_, [=](const User& u) { return check_address(u, address); });
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
  const Credentials c = parse_new_credentials(args);
  if (!check_user(c.address)) {
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
    cout << "Disconnecting user " << current_user_->getEmail() << '\n';
    current_server_ = nullptr;
    current_user_ = nullptr;
  } else {
    cout << "Not connected\n";
  }
}

// Server related commands.
bool System::check_server(string_view name) const {
  return ranges::any_of(servers_list_,
                        [=](const Server& s) { return check_name(s, name); });
}

auto System::find_server(string_view name) {
  return find_if(servers_list_.begin(), servers_list_.end(),
                 [=](const Server& s) { return check_name(s, name); });
}

void System::create_server(string_view name) {
  if (!check_server(name)) {
    servers_list_.emplace_back(current_user_->getId(), name);
    cout << "Server created\n";
  } else {
    cout << "There is already a server with that name\n";
  }
}

void System::change_description(const ServerDetails& sd) {
  if (check_server(sd.name)) {
    auto it{find_server(sd.name)};
    if (check_owner(*it, *current_user_)) {
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
    if (check_owner(*it, *current_user_)) {
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
    if (check_owner(*it, *current_user_)) {
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
    if (it->getInvite().empty() || check_owner(*it, *current_user_) ||
        sd.invite_code == it->getInvite()) {
      current_state_ = kJoinedServer;
      cout << "Joined server with success\n";
      if (!check_member(*it, *current_user_)) {
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
    cout << "Leaving server '" << current_server_->getName() << "'\n";
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
  return ranges::any_of(
      current_server_->getChannels(), [&](const unique_ptr<Channel>& c) {
        if (cd.type == "Text") {
          return c->getName() == cd.name && check_text_channel(*c);
        }
        return c->getName() == cd.name && check_voice_channel(*c);
      });
}

bool System::check_channel(string_view name) const {
  return ranges::any_of(
      current_server_->getChannels(),
      [=](const unique_ptr<Channel>& c) { return c->getName() == name; });
}

auto System::find_channel(string_view name) {
  return find_if(
      current_server_->getChannels().begin(),
      current_server_->getChannels().end(),
      [=](const unique_ptr<Channel>& c) { return c->getName() == name; });
}

auto System::find_channel(string_view name) const {
  return find_if(
      current_server_->getChannels().begin(),
      current_server_->getChannels().end(),
      [=](const unique_ptr<Channel>& c) { return c->getName() == name; });
}

void System::list_channels() const {
  cout << "#Text Channels\n";
  list_text_channels(*current_server_);
  cout << "#Voice Channels\n";
  list_voice_channels(*current_server_);
}

void System::create_channel(string_view args) {
  const ChannelDetails cd = parse_channel(args);
  if (!check_channel(cd)) {
    if (cd.type == "text") {
      auto c = make_unique<TextChannel>(cd.name);
      current_server_->create_channel(std::move(c));
    } else if (cd.type == "voice") {
      auto c = make_unique<VoiceChannel>(cd.name);
      current_server_->create_channel(std::move(c));
    }
    cout << cd.type << " Channel '" << cd.name << "' created\n";
  } else {
    cout << cd.type << " Channel '" << cd.name << "' already exists\n";
  }
}

void System::enter_channel(string_view name) {
  if (check_channel(name)) {
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
  if (check_text_channel(*current_channel_)) {
    auto tc = dynamic_cast<TextChannel&>(*current_channel_);
    tc.send_message({current_user_->getId(), msg});
    *current_channel_ = tc;
  } else if (check_voice_channel(*current_channel_)) {
    auto vc = dynamic_cast<VoiceChannel&>(*current_channel_);
    vc.send_message({current_user_->getId(), msg});
    *current_channel_ = vc;
  }
  cout << "Message sent\n";
}

void System::list_messages() {
  if (check_text_channel(*current_channel_)) {
    auto tc = dynamic_cast<TextChannel&>(*current_channel_);
    if (tc.getMessages().empty()) {
      cout << "No message to show\n";
    } else {
      ranges::for_each(tc.getMessages(),
                       [this](const Message& m) { print_message(m); });
    }
  } else if (check_voice_channel(*current_channel_)) {
    auto vc = dynamic_cast<VoiceChannel&>(*current_channel_);
    if (vc.getMessage().getContent().empty()) {
      cout << "No message to show\n";
    } else {
      print_message(vc.getMessage());
    }
  }
}

void System::print_message(const Message& m) const {
  cout << get_user_name(m.getId()) << "<" << m.getDateTime()
       << ">: " << m.getContent();
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
bool check_id(const User& u, int id) { return u.getId() == id; }

bool check_address(const User& u, string_view a) { return u.getEmail() == a; }

bool check_password(const User& u, string_view p) {
  return u.getPassword() == p;
}

Credentials parse_new_credentials(string_view cred) {
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

Credentials parse_credentials(string_view cred) {
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
  return s.getName() == name;
}

bool check_owner(const Server& s, const User& u) {
  return s.getOwner() == u.getId();
}

bool check_member(const Server& s, const User& u) {
  return ranges::any_of(s.getMembers(),
                        [&](int id) { return id == u.getId(); });
}

// Channel related helping functions.
void list_text_channels(const Server& server) {
  for (const unique_ptr<Channel>& channel : server.getChannels()) {
    if (check_text_channel(*channel)) {
      cout << channel->getName() << '\n';
    }
  }
}

void list_voice_channels(const Server& server) {
  for (const unique_ptr<Channel>& channel : server.getChannels()) {
    if (check_voice_channel(*channel)) {
      cout << channel->getName() << '\n';
    }
  }
}

bool check_text_channel(const Channel& channel) {
  return typeid(channel).name() == typeid(channel::TextChannel).name();
}

bool check_voice_channel(const Channel& channel) {
  return typeid(channel).name() == typeid(channel::VoiceChannel).name();
}

ChannelDetails parse_channel(string_view args) {
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

}  // namespace concordo
