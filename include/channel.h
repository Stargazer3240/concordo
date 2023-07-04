#ifndef CHANNEL_H
#define CHANNEL_H

#include <chrono>
#include <string>
#include <vector>

namespace channel {

using std::chrono::system_clock, std::string, std::string_view, std::time_t, std::vector;

class Message {
 public:
  Message() = default;
  Message(int id, string_view content): sender_id_{id}, content_{content} {}

 private:
  time_t date_time_{system_clock::to_time_t(system_clock::now())};
  int sender_id_{};
  string content_;
};

class Channel {
 public:
  Channel() = default;

 private:
  string name_;
};

class TextChannel : public Channel {
 public:
  TextChannel() = default;

 private:
  vector<Message> messages_;
};

class VoiceChannel : public Channel {
 public:
  VoiceChannel() = default;

 private:
  Message last_message_;
};

} // namespace channel
#endif // CHANNEL_H
