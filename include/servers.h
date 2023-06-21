// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SERVERS_H
#define SERVERS_H

#include <string>
#include <string_view>
#include <vector>

#include "channels.h"

namespace concordo::server {

using namespace concordo::channel;
using std::string, std::string_view, std::vector;

class Server {
 public:
  Server() = default;
  explicit Server(int id, string_view n) : owner_id_{id}, name_{n} {}

 private:
  int owner_id_{};
  string name_;
  string description_;
  string invite_code_;
  vector<Channel*> channels_;
  vector<int> members_id_;
};

}  // namespace concordo::server

#endif  // SERVERS_H
