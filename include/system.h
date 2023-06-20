// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <unordered_set>

#include "channels.h"
#include "servers.h"
#include "users.h"

enum class SystemState { kGuest, kLogged_In, kJoinedServer, kJoinedChannel };

namespace concordo::system {

using namespace concordo::user;
using namespace concordo::channel;
using namespace concordo::server;
using std::unordered_set;

class System {
 public:
  SystemState getState() const { return current_state_; }

 private:
  using enum SystemState;
  SystemState current_state_{kGuest};
  unordered_set<User> user_list_;
  unordered_set<Server> server_list_;
  User logged_user_;
  Server current_server_;
  Channel current_channel_;
};

}  // namespace concordo::system

#endif  // SYSTEM_H
