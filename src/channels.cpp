// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "channels.h"

namespace concordo {

void Message::save(fstream& f) {
  f << sender_id_ << '\n';
  f << date_time_ << '\n';
  f << content_ << '\n';
}

void TextChannel::save(fstream& f) {
  f << getName() << '\n';
  f << "TEXT\n";
  f << messages_.size() << '\n';
  save_messages(f);
}

void TextChannel::save_messages(fstream& f) {
  for (auto& m : messages_) {
    m.save(f);
  }
}

void VoiceChannel::save(fstream& f) {
  f << getName() << '\n';
  f << "VOICE\n";
  f << "1\n";
  last_message_.save(f);
}

}  // namespace concordo
