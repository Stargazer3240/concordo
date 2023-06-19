// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef CHANNELS_H
#define CHANNELS_H

#include <ctime>
#include <string>
#include <string_view>
#include <vector>

using std::string, std::string_view, std::tm, std::vector;

class Message {
 public:
  Message() = default;
  Message(tm date_time, int sender_id, string_view content)
      : date_time_{date_time}, sender_id_{sender_id}, content_{content} {}

  [[nodiscard]] tm getDateTime() const { return date_time_; }
  [[nodiscard]] int getId() const { return sender_id_; }
  [[nodiscard]] string getContent() const { return content_; }

 private:
  tm date_time_{};
  int sender_id_{};
  string content_;
};

class Channel {
 public:
  explicit Channel(string_view name) : name_{name} {}

  [[nodiscard]] string getName() const { return name_; }

 private:
  string name_;
};

class TextChannel : public Channel {
 public:
  explicit TextChannel(string_view name) : Channel(name) {}

  [[nodiscard]] vector<Message> getMessages() const { return messages_; }

 private:
  vector<Message> messages_;
};

class VoiceChannel : public Channel {
 public:
  explicit VoiceChannel(string_view name) : Channel(name) {}

  [[nodiscard]] Message getMessage() const { return last_message_; }

 private:
  Message last_message_;
};

#endif  // CHANNELS_H
