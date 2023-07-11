// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef CHANNELS_H
#define CHANNELS_H

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace concordo {

using std::string, std::string_view, std::vector, std::cout, std::fstream;
using std::chrono::system_clock;

struct MessageDetails {
  time_t date_time;
  int sender_id;
  string content;
};

/*! A class that represents a message sent in a channel.
 *  @see Channel
 */
class Message {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see TextChannel::messages_
   */
  Message() = default;

  /*! A constructor to be used by the system.
   */
  Message(int sender_id, string_view content)
      : sender_id_{sender_id}, content_{content} {}
  explicit Message(const MessageDetails &d)
      : date_time_{d.date_time}, sender_id_{d.sender_id}, content_{d.content} {}

  /*! @see date_time_ */
  [[nodiscard]] time_t getDateTime() const { return date_time_; }

  /*! @see sender_id_ */
  [[nodiscard]] int getId() const { return sender_id_; }

  /*! @see content_ */
  [[nodiscard]] string getContent() const { return content_; }

  [[nodiscard]] bool empty() const { return content_.empty(); }

  void save(fstream &f);

 private:
  time_t date_time_{
      system_clock::to_time_t(system_clock::now())}; /*!< The date and time when
                                                        the message was sent. */
  int sender_id_{}; /*!< The id of the user who sent the message. */
  string content_;  /*!< The content written into the message. */
};

struct ChannelDetails {
  string name;
  string type;
  vector<Message> messages;
};

/*! A base class that represents a channel from a Concordo's server.
 *
 *  A channel has a name and is inherited from. It's where users send their
 *  messages.
 *  @see user::User; server::Server
 *  @see TextChannel; VoiceChannel; Message
 */
class Channel {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see server::Server::channels_
   */
  Channel() = default;
  Channel(const Channel &) = default;
  Channel(Channel &&) = delete;
  Channel &operator=(const Channel &) = default;
  Channel &operator=(Channel &&) = delete;
  virtual ~Channel() = default;
  /*! A constructor to be used by the system.
   */
  explicit Channel(string_view name) : name_{name} {}

  [[nodiscard]] string getName() const { return name_; }

  [[nodiscard]] bool check_name(string_view name) const {
    return this->name_ == name;
  }

  virtual void send_message(const Message &m) = 0;

  void print() { cout << name_ << '\n'; }

  virtual void save(fstream &f) = 0;

 private:
  string name_; /*!< The name of the channel. */
};

/*! A derived class that represents a text channel from a server.
 *
 *  A text channel has a list of messages and inherits from base channel.
 *  @see Channel; Message
 */
class TextChannel : public Channel {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see server::Server::channels_
   */
  TextChannel() = default;

  /*! A constructor to be used by the system.
   *  @see Channel::Channel(string_view)
   */
  explicit TextChannel(string_view name) : Channel(name) {}

  explicit TextChannel(const ChannelDetails &d)
      : Channel(d.name), messages_{d.messages} {}

  /*! @see messages_ */
  vector<Message> getMessages() { return messages_; }
  void send_message(const Message &m) override { messages_.push_back(m); }
  [[nodiscard]] bool empty() const { return messages_.empty(); }

  void save(fstream &f) override;
  void save_messages(fstream &f);

 private:
  vector<Message> messages_; /*!< The list of all messages sent to a channel. */
};

/*! A derived class that represents a voice channel from a server.
 *
 *  A voice channel stores the last message sent and inherits from base channel.
 *  @see Channel; Message
 */
class VoiceChannel : public Channel {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see server::Server::channels_
   */
  VoiceChannel() = default;

  /*! A constructor to be used by the system.
   *  @see Channel::Channel(string_view)
   */
  explicit VoiceChannel(string_view name) : Channel(name) {}
  explicit VoiceChannel(const ChannelDetails &d)
      : Channel(d.name), last_message_{d.messages[0]} {}

  /*! @see last_message_ */
  Message getMessage() { return last_message_; }
  void send_message(const Message &m) override { last_message_ = m; }
  [[nodiscard]] bool empty() const { return last_message_.empty(); }

  void save(fstream &f) override;

 private:
  Message last_message_; /*!< The last "voice" message sent in the channel. */
};

}  // namespace concordo

#endif  // CHANNELS_H
