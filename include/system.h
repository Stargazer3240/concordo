// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string_view>
#include <utility>
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

class System {
 public:
  [[nodiscard]] SystemState getState() const { return current_state_; }

  void create_user(string_view args);
  void user_login(string_view cred);
  bool check_new_user(EmailAddress a);
  User get_user(EmailAddress a);
  bool check_credentials(string_view cred);

 private:
  using enum SystemState;
  SystemState current_state_{kGuest};
  vector<User> user_list_;
  vector<Server> server_list_;
  User logged_user_;
  Server current_server_;
  Channel current_channel_;
};

void quit();

bool check_address(const User& u, const EmailAddress& a);
bool check_password(const User& u, const Password& p);

tuple<EmailAddress, Password, Name> parse_new_credentials(string_view cred);

pair<EmailAddress, Password> parse_credentials(string_view cred);

}  // namespace concordo::system

#endif  // SYSTEM_H
