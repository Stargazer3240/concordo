// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <unordered_set>

#include "channels.h"
#include "servers.h"
#include "users.h"

using std::unordered_set;

enum class SystemState { kGuest, kLogged_In, kJoinedServer, kJoinedChannel };

class System {
 public:
 private:
  int state{};
  unordered_set<User> user_list_;
  unordered_set<Server> server_list_;
  User logged_user_;
  Server open_server_;
  Channel open_channel_;
};

#endif  // SYSTEM_H
