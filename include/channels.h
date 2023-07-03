// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef CHANNELS_H
#define CHANNELS_H

#include <chrono>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace channel {

using std::string, std::string_view, std::tm, std::vector;
using std::chrono::current_zone, std::chrono::system_clock,
    std::chrono::zoned_time;
using duration = std::chrono::duration<long, std::nano>;

struct ChannelDetails {
  string name;
  string type;
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

  /*! @see date_time_ */
  [[nodiscard]] zoned_time<duration> getDateTime() const { return date_time_; }

  /*! @see sender_id_ */
  [[nodiscard]] int getId() const { return sender_id_; }

  /*! @see content_ */
  [[nodiscard]] string getContent() const { return content_; }

 private:
  zoned_time<duration> date_time_{
      current_zone(),
      system_clock::now()}; /*!< The date and time when the message was sent. */
  int sender_id_{};         /*!< The id of the user who sent the message. */
  string content_;          /*!< The content written into the message. */
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

  /*! A constructor to be used by the system.
   */
  explicit Channel(string_view name) : name_{name} {}

  /*! @see name_ */
  [[nodiscard]] string getName() const { return name_; }

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

  /*! @see messages_ */
  [[nodiscard]] vector<Message> getMessages() const { return messages_; }

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

  /*! @see last_message_ */
  [[nodiscard]] Message getMessage() const { return last_message_; }

 private:
  Message last_message_; /*!< The last "voice" message sent in the channel. */
};

}  // namespace channel

#endif  // CHANNELS_H
