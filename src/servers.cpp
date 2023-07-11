// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "servers.h"

namespace concordo {

using std::fstream;

void Server::save(fstream& f) {
  save_owner(f);
  f << name_ << '\n';
  save_description(f);
  save_invite(f);
  save_members_amount(f);
  save_ids(f);
  save_channels_amount(f);
  save_channels(f);
}

void Server::save_ids(fstream& f) {
  for (const auto& id : members_ids_) {
    f << id << '\n';
  }
}

void Server::save_channels(fstream& f) {
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
