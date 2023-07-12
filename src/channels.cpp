// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "channels.h"

namespace concordo {

void Message::save(fstream& f) {
  f << sender_id_ << '\n';
  f << time_to_string(date_time_) << '\n';
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

string time_to_string(const time_t& t) {
  const int y_epoch{1900};
  std::tm* now = std::localtime(&t);
  std::stringstream ss;
  ss << now->tm_mday << '/' << now->tm_mon + 1 << '/' << now->tm_year + y_epoch
     << " - " << now->tm_hour << ':' << now->tm_min;
  return ss.str();
}

}  // namespace concordo
