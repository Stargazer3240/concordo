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
using ItVectorServer =
    std::ranges::borrowed_iterator_t<vector<Server, std::allocator<Server>>&>;
using ItVectorUser =
    std::ranges::borrowed_iterator_t<vector<User, std::allocator<User>>&>;

class System {
 public:
  [[nodiscard]] SystemState getState() const { return current_state_; }
  [[nodiscard]] vector<User> getUserList() const { return user_list_; }

  void create_user(string_view args);
  bool check_user(string_view address);
  void user_login(string_view cred);
  void disconnect();
  bool check_credentials(string_view cred);
  ItVectorUser find_user(int id);
  ItVectorUser find_user(string_view address);
  User get_user(string_view address);
  User get_user(int id);
  string get_user_name(int id);

  void create_server(string_view name);
  void change_description(const ServerDetails& sd);
  void change_invite(const ServerDetails& sd);
  void list_servers();
  void remove_server(string_view name);
  void enter_server(const ServerDetails& sd);
  void leave_server();
  void list_participants();
  bool check_server(string_view name);
  ItVectorServer find_server(string_view name);
  Server get_server(string_view name);

 private:
  using enum SystemState;
  SystemState current_state_{kGuest};
  vector<User> user_list_;
  vector<Server> server_list_;
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
