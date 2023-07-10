#include "servers.h"

namespace concordo {

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
