// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef CHANNELS_H
#define CHANNELS_H

#include <ctime>
#include <string>
#include <string_view>
#include <vector>

class Message {
 public:
  Message() = default;
  Message(std::tm date_time, int sender_id, std::string_view content)
      : date_time{date_time}, sender_id{sender_id}, content{content} {}

  std::tm getDateTime() { return date_time; }
  [[nodiscard]] int getId() const { return sender_id; }
  std::string getContent() { return content; }

 private:
  std::tm date_time{};
  int sender_id{};
  std::string content;
};

class Channel {
 public:
  explicit Channel(std::string_view name) : name{name} {}

  std::string getName() { return name; }

 private:
  std::string name;
};

class TextChannel : public Channel {
 public:
  explicit TextChannel(std::string_view name) : Channel(name) {}

  std::vector<Message>& getMessages() { return messages; }

 private:
  std::vector<Message> messages;
};

class VoiceChannel : public Channel {
 public:
  explicit VoiceChannel(std::string_view name) : Channel(name) {}

  Message& getMessage() { return last_message; }

 private:
  Message last_message;
};

#endif  // CHANNELS_H
