#include "channels.h"

#include <fstream>

namespace concordo {

void Message::save(ofstream& f) {
  f << sender_id_ << '\n';
  f << date_time_ << '\n';
  f << content_ << '\n';
}

void TextChannel::save(ofstream& f) {
  f << getName() << '\n';
  f << "TEXT\n";
  f << messages_.size() << '\n';
  save_messages(f);
}

void TextChannel::save_messages(ofstream& f) {
  for (auto& m : messages_) {
    m.save(f);
  }
}

void VoiceChannel::save(ofstream& f) {
  f << getName() << '\n';
  f << "VOICE\n";
  f << "1\n";
  last_message_.save(f);
}

}  // namespace concordo
