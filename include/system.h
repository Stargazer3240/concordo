#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>

#include "user.h"
#include "server.h"
#include "channel.h"

namespace concordo {

using std::vector;
using User = user::User;
using Server = server::Server;
using Channel = channel::Channel;

class System {
  vector<User> users_;
  vector<Server> servers_;
  User* current_user_{nullptr};
  Server* current_server_{nullptr};
  Channel* current_channel_{nullptr};
};

} // namespace concordo
#endif // SYSTEM_H
