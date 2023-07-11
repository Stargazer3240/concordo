// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SERVERS_H
#define SERVERS_H

#include <algorithm>
#include <concepts>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <typeinfo>
#include <utility>
#include <vector>

#include "channels.h"
#include "users.h"

namespace concordo {

using std::unique_ptr, std::string, std::string_view, std::vector, std::cout,
    std::fstream;
namespace ranges = std::ranges;

/*! A struct that contains server details.
 *
 *  The server details are used as input for some system commands.
 *  @see Server
 *  @see concordo::System::change_description();
 *  concordo::System::change_invite(); concordo::System::enter_server()
 */
struct ServerDetails {
  int owner_id;
  string name;        /*!< A server name to be input from the system. */
  string description; /*!< A server description to be input from the system. */
  string invite_code; /*!< A server invite code to be input from the system. */
  vector<int> members_ids;
};

/*! A class that represents a server in the Concordo system.
 *
 *  A server is where users gather for a common reason. It's owned by a single
 *  user and can have some channels in it.
 *  @see user::User; channel::Channel
 */
class Server {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see concordo::System::servers_list_
   */
  Server() = default;

  /*! A constructor to be used by the system.
   *  @see concordo::System::create_server()
   */
  explicit Server(int id, string_view n) : owner_id_{id}, name_{n} {}
  explicit Server(const ServerDetails& d)
      : owner_id_{d.owner_id},
        name_{d.name},
        description_{d.description},
        invite_code_{d.invite_code},
        members_ids_{d.members_ids} {}

  [[nodiscard]] string getName() const { return name_; }

  [[nodiscard]] vector<int> getMembers() const { return members_ids_; }

  constexpr vector<unique_ptr<Channel>>& getChannels() { return channels_; }

  void change_description(string_view desc) { this->description_ = desc; }
  void change_invite(string_view code) { this->invite_code_ = code; }

  /*! A method that adds an user to the member list.
   *  @see members_ids_
   */
  void add_member(const User& u) { members_ids_.push_back(u.getId()); }
  void create_channel(unique_ptr<Channel> c) {
    channels_.push_back(std::move(c));
  }

  void save(fstream& f);
  void save_owner(fstream& f) const { f << owner_id_ << '\n'; }
  void save_description(fstream& f) { f << description_ << '\n'; }
  void save_invite(fstream& f) { f << invite_code_ << '\n'; }
  void save_members_amount(fstream& f) { f << members_ids_.size() << '\n'; }
  void save_ids(fstream& f);
  void save_channels_amount(fstream& f) { f << channels_.size() << '\n'; }
  void save_channels(fstream& f);

  [[nodiscard]] bool check_name(string_view name) const {
    return this->name_ == name;
  }

  [[nodiscard]] bool check_owner(const User& u) const {
    return u.check_id(owner_id_);
  }

  [[nodiscard]] bool check_invite(const string& ic) const {
    return invite_code_ == ic;
  }

  [[nodiscard]] bool check_member(const User& u) const {
    return ranges::any_of(members_ids_, [&](int id) { return u.check_id(id); });
  }

  [[nodiscard]] bool check_channel(const ChannelDetails& cd) const;
  constexpr auto find_channel(string_view name);

  void print() const { cout << name_ << '\n'; }

  [[nodiscard]] bool has_invite() const { return !invite_code_.empty(); }

  void list_text_channels() const;

  void list_voice_channels() const;

  constexpr bool any_of(string_view name) {
    namespace ranges = std::ranges;
    return ranges::any_of(channels_,
                          [=](const auto& c) { return c->check_name(name); });
  }

  friend ostream& operator<<(ostream& out, const Server& s);

 private:
  int owner_id_{}; /*!< The id of the user who created and owns the server. */
  string name_;    /*!< The name of the server. It's unique. */
  string description_; /*!< The description of the server. Can be changed. */
  string invite_code_; /*!< The invite code of the server. Can be empty. */
  vector<unique_ptr<Channel>>
      channels_;            /*!< The list of channels from the server. */
  vector<int> members_ids_; /*!< The list of ids from the users that are member
                               of the server */
};

template <typename ChildType>
bool check_channel_type(const Channel& channel) {
  return typeid(channel).name() == typeid(ChildType).name();
}

}  // namespace concordo

#endif  // SERVERS_H
