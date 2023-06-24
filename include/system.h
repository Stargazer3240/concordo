// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string_view>
#include <vector>

#include "channels.h"
#include "servers.h"
#include "users.h"

namespace concordo::system {

enum class SystemState { kGuest, kLogged_In, kJoinedServer, kJoinedChannel };

using namespace concordo::user;
using namespace concordo::channel;
using namespace concordo::server;
using std::pair, std::string_view, std::vector, std::tuple;
using ItVectorServer = std::ranges::borrowed_iterator_t<
    const vector<Server, std::allocator<Server>>&>;
using ItVectorUser =
    std::ranges::borrowed_iterator_t<const vector<User, std::allocator<User>>&>;
using Name = string;
using EmailAddress = string;
using Password = string;

class System {
 public:
  [[nodiscard]] SystemState getState() const { return current_state_; }
  [[nodiscard]] vector<User> getUserList() const { return users_list_; }

  void create_user(string_view args);
  void user_login(string_view cred);
  void disconnect();
  [[nodiscard]] bool check_credentials(string_view cred) const;
  [[nodiscard]] bool check_user(string_view address) const;
  [[nodiscard]] ItVectorUser find_user(int id) const;
  [[nodiscard]] ItVectorUser find_user(string_view address) const;
  [[nodiscard]] User get_user(string_view address) const;
  [[nodiscard]] User get_user(int id) const;
  [[nodiscard]] string get_user_name(int id) const;

  void create_server(string_view name);
  void change_description(const ServerDetails& sd);
  void change_invite(const ServerDetails& sd);
  void list_servers() const;
  void remove_server(string_view name);
  void enter_server(const ServerDetails& sd);
  void leave_server();
  void list_participants() const;
  [[nodiscard]] bool check_server(string_view name) const;
  [[nodiscard]] ItVectorServer find_server(string_view name) const;
  [[nodiscard]] Server get_server(string_view name) const;

 private:
  using enum SystemState;
  SystemState current_state_{kGuest};
  vector<User> users_list_;
  vector<Server> servers_list_;
  User logged_user_;
  Server current_server_;
  Channel current_channel_;
  int last_id_{};
};

bool check_id(const User& u, int id);
bool check_address(const User& u, string_view a);
bool check_password(const User& u, string_view p);
tuple<Name, EmailAddress, Password> parse_new_credentials(string_view cred);
pair<EmailAddress, Password> parse_credentials(string_view cred);

bool check_name(const Server& s, string_view name);
bool check_owner(const Server& s, const User& u);
bool check_member(const Server& s, const User& u);

void print_abscent(string_view name);
void print_no_permission(string_view sv);
void print_info_changed(tuple<string_view, string_view, string_view> info);
void quit();

}  // namespace concordo::system

#endif  // SYSTEM_H
