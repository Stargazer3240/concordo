#include "servers.h"

#include <fstream>

namespace concordo {

using std::ofstream;

void Server::save_ids(ofstream& f) {
  for (const auto& id : members_ids_) {
    f << id << '\n';
  }
}

void Server::save_channels(ofstream& f) {
  for (const auto& channel : channels_) {
    channel->save(f);
  }
}

bool Server::check_channel(const ChannelDetails& cd) const {
  return ranges::any_of(channels_, [&](const unique_ptr<Channel>& c) {
    if (cd.type == "text") {
      return c->check_name(cd.name) && check_channel_type<TextChannel>(*c);
    }
    return c->check_name(cd.name) && check_channel_type<VoiceChannel>(*c);
  });
}

void Server::list_text_channels() const {
  for (const unique_ptr<Channel>& channel : channels_) {
    if (check_channel_type<TextChannel>(*channel)) {
      channel->print();
    }
  }
}

void Server::list_voice_channels() const {
  for (const unique_ptr<Channel>& channel : channels_) {
    if (check_channel_type<VoiceChannel>(*channel)) {
      channel->print();
    }
  }
}

ostream& operator<<(ostream& out, const Server& s) { return out << s.name_; }

}  // namespace concordo
