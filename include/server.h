#ifndef SERVER_H
#define SERVER_H

#include <memory>
#include <string>
#include <vector>

#include "channel.h"

namespace server {

using std::string, std::vector, std::unique_ptr;
using Channel = channel::Channel;

class Server {
 private:
  int owner_id_{};
  string name_;
  string description_;
  string invite_code_;
  vector<unique_ptr<Channel>> channels_;
  vector<int> members_ids_;
};

} // namespace server
#endif // SERVER_H
