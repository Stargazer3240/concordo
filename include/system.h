// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string_view>
#include <vector>

#include "channels.h"
#include "servers.h"
#include "users.h"

namespace concordo::system {

using std::pair, std::string, std::string_view, std::vector, std::tuple;
using User = user::User;
using Channel = channel::Channel;
using Server = server::Server;
using ServerDetails = server::ServerDetails;

// Iterator returned by range::find
using ItVectorServer = std::ranges::borrowed_iterator_t<
    const vector<Server, std::allocator<Server>>&>;
using ItVectorUser =
    std::ranges::borrowed_iterator_t<const vector<User, std::allocator<User>>&>;

// Used to better represent the output of the credential parsing functions, as
// they return tuple/pair of strings only.
using Name = string;
using EmailAddress = string;
using Password = string;

/*!
 * @brief A class that represents Concordo's system.
 *
 * The system is responsible for managing Users, Channels, Servers, and
 * interacting with the CLI.
 * @see User; Channel; Server
 */
class System {
 public:
  /*!
   * Represents the different states the system can be, which is used to
   * determine what commands are allowed to be used by the user at a certain
   * time.
   */
  enum class SystemState {
    kGuest,     /*!< No user logged-in. */
    kLogged_In, /*!< The user logged in, but isn't visualizing a server/channel.
                 */
    kJoinedServer, /*!< The user is visualizing a server. */
    kJoinedChannel /*!< The user is visualizing a channel from the current
                      server. */
  };

  [[nodiscard]] SystemState getState() const { return current_state_; }
  [[nodiscard]] vector<User> getUserList() const { return users_list_; }

  /*! Create an user in the system.
   *  @param args the arguments of the create-user command.
   *  @see parse_new_credentials(); check_user(); user_list_; last_id_
   *  @see User; User.id_; last_id_; User::Credentials
   */
  void create_user(string_view args);

  /*! Log the user in the system.
   *  @param cred the credentials parsed from the login command.
   *  @see parse_credentials(); check_credentials(); logged_user_;
   * current_state_
   *  @see User; User.address_
   */
  void user_login(string_view cred);

  /*! Disconnect the current user from the system.
   *  @see current_state_; logged_user_
   *  @see User; User.address_
   */
  void disconnect();

  /*! Check if the input credentials are valid.
   *
   *  To a credential to be valid, there needs to be an user registered
   *  in the system with the same email address and password as the input ones.
   *  @param cred the credentials parsed from the used command
   *  @see parse_credentials(); check_address(); check_password()
   *  @see User; User.address_; User.password_
   *  @return True if the credentials are valid
   */
  [[nodiscard]] bool check_credentials(string_view cred) const;

  /*! Check if the user exists in the system.
   *
   *  The check is valid if there is an user in the system with the same address
   *  as the input addres.
   *  @param address the address to be checked
   *  @see user_list_; check_address()
   *  @see User; User.address_
   *  @return True if the email address was used by any user registered
   */
  [[nodiscard]] bool check_user(string_view address) const;

  /*! Find the position of an user in the system.
   *
   *  Checks through the entire user list checking if an user has the same id
   *  as the input one, giving the position of it when it is found. It expects
   *  that the user being looked for exists.
   *  @param id the id to be checked
   *  @see user_list_; check_id()
   *  @see User; User.id_
   *  @return An iterator pointing to the position of the user in the user list
   */
  [[nodiscard]] ItVectorUser find_user(int id) const;

  /*! Find the position of an user in the system.
   *
   *  Checks through the entire user list checking if an user has the same
   * address as the input one, giving the position of it when it is found. It
   * expects that the user being looked for exists.
   *  @param address the address to be checked
   *  @see user_list_; check_address(); get_user()
   *  @see User; User.address_
   *  @return An iterator pointing to the position of the user in the user list
   */
  [[nodiscard]] ItVectorUser find_user(string_view address) const;

  /*! Deferences the user found by the find_user method.
   *
   *  It's expected that the user exists in the system, so a check should
   *  be done before calling this method.
   *  @param address the address to be checked
   *  @see user_list_; check_address(); find_user(); check_credentials()
   *  @see User; User.address_
   *  @return The user pointed by the iterator returned by find_user
   */
  [[nodiscard]] User get_user(string_view address) const;

  /*! Deferences the user found by the find_user method.
   *
   *  It's expected that the user exists in the system, so a check should
   *  be done before calling this method.
   *  @param id the id to be checked
   *  @see user_list_; check_id(); find_user(); check_credentials()
   *  @see User; User.id_
   *  @return The user pointed by the iterator returned by find_user
   */
  [[nodiscard]] User get_user(int id) const;

  /*! Get the name of the user with the same id and the input one.
   *
   *  This method expects that the input id is valid.
   *  @param id the id to be checked
   *  @see get_user(); user_list_
   *  @see User; User.id_; User.name_
   *  @return The name of said user
   */
  [[nodiscard]] string get_user_name(int id) const;

  void create_server(string_view name);
  void change_description(const ServerDetails& sd);
  void change_invite(const ServerDetails& sd);
  void list_servers() const;
  void remove_server(string_view name);
  void enter_server(const ServerDetails& sd);
  void leave_server();
  void list_participants() const;
  [[nodiscard]] bool check_server(string_view name) const;
  [[nodiscard]] ItVectorServer find_server(string_view name) const;
  [[nodiscard]] Server get_server(string_view name) const;

 private:
  using enum SystemState;

  SystemState current_state_{kGuest}; /*!< The current state of the system */
  vector<User> users_list_;     /*!< The list of all users in the system */
  vector<Server> servers_list_; /*!< The list of all servers in the system */
  User logged_user_;            /*!< The current logged-in user */
  Server current_server_;       /*!< The current server being visualized */
  Channel current_channel_;     /*!< The current channel being visualized */
  int last_id_{};               /*!< The last id generated by the system */
};

/*! Check if the user's id is equal to the parameter id. */
bool check_id(const User& u, int id);

/*! Check if the user's address is equal to the parameter address. */
bool check_address(const User& u, string_view a);

/*! Check if the user's password is equal to the parameter password. */
bool check_password(const User& u, string_view p);

/*! Parse the credentials of a new user, splitting the arguments of create-user
    command. */
tuple<Name, EmailAddress, Password> parse_new_credentials(string_view cred);

/*! Parse the credentials of an existing user, splitting the arguments of login
    command. */
pair<EmailAddress, Password> parse_credentials(string_view cred);

/*! Check if a server's name is equal to the parameter name. */
bool check_name(const Server& s, string_view name);

/*! Check if a server's owner id is equal to the id of an user. */
bool check_owner(const Server& s, const User& u);

/*! Chech if an user is a member of said server. */
bool check_member(const Server& s, const User& u);

void print_abscent(string_view name);
void print_no_permission(string_view sv);
void print_info_changed(tuple<string_view, string_view, string_view> info);
void quit();

}  // namespace concordo::system

#endif  // SYSTEM_H
