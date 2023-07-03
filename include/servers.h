// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SERVERS_H
#define SERVERS_H

#include <string>
#include <string_view>
#include <vector>

#include "channels.h"
#include "users.h"

namespace server {

using Channel = channel::Channel;
using User = user::User;
using std::shared_ptr, std::string, std::string_view, std::vector;

/*! A struct that contains server details.
 *
 *  The server details are used as input for some system commands.
 *  @see Server
 *  @see concordo::System::change_description();
 *  concordo::System::change_invite(); concordo::System::enter_server()
 */
struct ServerDetails {
  string name;        /*!< A server name to be input from the system. */
  string description; /*!< A server description to be input from the system. */
  string invite_code; /*!< A server invite code to be input from the system. */
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

  /*! @see owner_id_ */
  [[nodiscard]] int getOwner() const { return owner_id_; }

  /*! @see name_ */
  [[nodiscard]] string getName() const { return name_; }

  /*! @see invite_code_ */
  [[nodiscard]] string getInvite() const { return invite_code_; }

  /*! @see members_ids_ */
  [[nodiscard]] vector<int> getMembers() const { return members_ids_; }

  vector<shared_ptr<Channel>> getChannels() { return channels_; }
  [[nodiscard]] vector<shared_ptr<Channel>> getChannels() const {
    return channels_;
  }

  /*! @see description_ */
  void setDescription(string_view desc) { this->description_ = desc; }

  /*! @see invite_code_ */
  void setInvite(string_view code) { this->invite_code_ = code; }

  /*! A method that adds an user to the member list.
   *  @see members_ids_
   */
  void add_member(const User& u) { members_ids_.push_back(u.getId()); }

 private:
  int owner_id_{}; /*!< The id of the user who created and owns the server. */
  string name_;    /*!< The name of the server. It's unique. */
  string description_; /*!< The description of the server. Can be changed. */
  string invite_code_; /*!< The invite code of the server. Can be empty. */
  vector<shared_ptr<Channel>>
      channels_;            /*!< The list of channels from the server. */
  vector<int> members_ids_; /*!< The list of ids from the users that are member
                               of the server */
};

}  // namespace server

#endif  // SERVERS_H
