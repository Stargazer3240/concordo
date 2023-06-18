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
      : date_time{date_time}, sender_id{sender_id}, content{content} {}

  tm getDateTime() { return date_time; }
  [[nodiscard]] int getId() const { return sender_id; }
  string getContent() { return content; }

 private:
  tm date_time{};
  int sender_id{};
  string content;
};

class Channel {
 public:
  explicit Channel(string_view name) : name{name} {}

  string getName() { return name; }

 private:
  string name;
};

class TextChannel : public Channel {
 public:
  explicit TextChannel(string_view name) : Channel(name) {}

  vector<Message>& getMessages() { return messages; }

 private:
  vector<Message> messages;
};

class VoiceChannel : public Channel {
 public:
  explicit VoiceChannel(string_view name) : Channel(name) {}

  Message& getMessage() { return last_message; }

 private:
  Message last_message;
};

#endif  // CHANNELS_H
