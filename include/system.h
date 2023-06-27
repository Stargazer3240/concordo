// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef SYSTEM_H
#define SYSTEM_H

#include <string_view>
#include <unordered_set>
#include <vector>

#include "channels.h"
#include "servers.h"
#include "users.h"

namespace concordo {

using std::pair, std::string, std::string_view, std::vector, std::tuple,
    std::unordered_set;
using User = user::User;
using Channel = channel::Channel;
using Server = server::Server;
using ServerDetails = server::ServerDetails;

// Used to better represent the output of the credential parsing functions, as
// they return tuple/pair of strings only and what credential is each can be
// confusing.
using Name = string;
using EmailAddress = string;
using Password = string;

/*! A struct that contains a line input to the CLI.
 *  @see System; System::run()
 */
struct CommandLine {
  string command;   /*!< The command part of the line. */
  string arguments; /*!< The argument part of the line. */
};

/*! A class that represents Concordo's system.
 *
 *  The system is responsible for managing Users, Channels, Servers, and
 *  interacting with the CLI.
 *  @see user::User; channel::Channel; server::Server
 */
class System {
 public:
  /*! Represents the different states the system can be, which is used to
   *  determine what commands are allowed to be used by the user at a certain
   *  time.
   */
  enum class SystemState {
    kGuest,     /*!< No user logged-in. */
    kLogged_In, /*!< The user logged in, but isn't visualizing a server/channel.
                 */
    kJoinedServer, /*!< The user is visualizing a server. */
    kJoinedChannel /*!< The user is visualizing a channel from the current
                      server. */
  };

  /*! @see current_state_ */
  [[nodiscard]] SystemState getState() const { return current_state_; }

  /*! @see users_list_ */
  [[nodiscard]] vector<User> getUserList() const { return users_list_; }

  /*! Starts the main Concordo loop. */
  void init();

  /*! Runs the command input in the CLI.
   *
   *  This method respects the current system state to determine what the user
   *  is allowed to do.
   *  @see SystemState; current_state_
   */
  void run(const CommandLine& cl);

  /*! Runs the commands allowed to a guest.
   *  @see SystemState; current_state_; guest_commands_
   */
  void run_guest_cmd(const CommandLine& cl);

  /*! Runs the commands allowed to a logged-in user.
   *  @see SystemState; current_state_; logged_commands_
   */
  void run_logged_cmd(const CommandLine& cl);

  /*! Runs the commands allowed to an user visualizing a server.
   *  @see SystemState; current_state_; server_commands_
   */
  void run_server_cmd(const CommandLine& cl);

  /*! Checks if the input command is a valid command.
   *  @param cmd the command to be checked
   *  @return True if the command is valid.
   *  @see guest_commands_; logged_commands_; server_commands_
   */
  bool check_all_commands(string_view cmd);

  /*! Checks if the input credentials are valid.
   *
   *  To a credential to be valid, there needs to be an user registered
   *  in the system with the same email address and password as the input ones.
   *  @param cred the credentials parsed from the used command
   *  @see user::User; user::User::address_; user::User::password_
   *  @return True if the credentials are valid
   */
  [[nodiscard]] bool check_credentials(string_view cred) const;

  /*! Check if the user exists in the system.
   *
   *  The check is valid if there is an user in the system with the same address
   *  as the input address.
   *  @param address the address to be checked
   *  @see users_list_
   *  @see user::User; user::User::address_
   *  @return True if the email address was used by any registered user
   */
  [[nodiscard]] bool check_user(string_view address) const;

  /*! Find the position of an user in the system.
   *
   *  Goes through the entire user list checking if an user has the same id
   *  as the input one, giving the position of it when it is found. It expects
   *  that the user being looked for exists.
   *  @param id the id to be checked
   *  @see users_list_
   *  @see user::User; user::User::id_
   *  @return An iterator pointing to the position of the user in the user list
   */
  [[nodiscard]] auto find_user(int id) const;

  /*! Find the position of an user in the system.
   *
   *  Goes through the entire user list checking if an user has the same
   *  address as the input one, giving the position of it when it is found. It
   *  expects that the user being looked for exists.
   *  @param address the address to be checked
   *  @see users_list_;
   *  @see user::User; user::User::address_
   *  @return An iterator pointing to the position of the user in the user list
   */
  [[nodiscard]] auto find_user(string_view address) const;

  /*! Gets the name of the user with the same id and the input one.
   *
   *  This method expects that the input id is valid.
   *  @param id the id to be checked
   *  @see find_user(); users_list_
   *  @see user::User; user::User::id_; user::User::name_
   *  @return The name of said user
   */
  [[nodiscard]] string get_user_name(int id) const;

  /*! Creates an user in the system.
   *  @param args the arguments of the create-user command.
   *  @see check_user(); users_list_; last_id_
   *  @see user::User; user::User::id_; last_id_; user::Credentials
   */
  void create_user(string_view args);

  /*! Logs in an user in the system.
   *  @param cred the credentials parsed from the login command.
   *  @see check_credentials(); logged_user_; current_state_
   *  @see user::User; user::User::address_
   */
  void user_login(string_view cred);

  /*! Disconnects the current user from the system.
   *  @see current_state_; logged_user_
   *  @see user::User; user::User::address_
   */
  void disconnect();

  /*! Check if the server exists in the system.
   *
   *  The check is valid if there is a server in the system with the same name
   *  as the input name.
   *  @param name the name to be checked
   *  @see servers_list_
   *  @see server::Server; server::Server::name_
   *  @return True if the name was used by any registered server
   */
  [[nodiscard]] bool check_server(string_view name) const;

  /*! Find the position of an server in the system.
   *
   *  Goes through the entire server list checking if a server has the same
   *  name as the input one, giving the position of it when it is found. It
   *  expects that the server being looked for exists.
   *  @param name the name to be checked
   *  @see servers_list_
   *  @see server::Server; server::Server::name_
   *  @return An iterator pointing to the position of the server in the server
   * list
   */
  [[nodiscard]] auto find_server(string_view name);

  /*! Creates a server in the system.
   *  @param name the name of the server to be created.
   *  @see check_server(); find_server(); servers_list_
   *  @see server::Server::add_member()
   */
  void create_server(string_view name);

  /*! Changes the description of a server.
   *
   *  To change the description of a server, you have to be its owner.
   *  @param sd the details of the server to be changed, including its name and
   *  the new description.
   *  @see check_server(); find_server();
   *  @see server::Server::setDescription(); server::ServerDetails
   */
  void change_description(const ServerDetails& sd);

  /*! Changes the invite code of a server.
   *
   *  To change the invite code of a server, you have to be its owner. If the
   *  input invite code is empty, the server's invite code requirement will be
   *  removed.
   *  @param sd the details of the server to be changed, including its name and
   *  the new invite code.
   *  @see check_server(); find_server();
   *  @see server::Server::setInvite(); server::ServerDetails
   */
  void change_invite(const ServerDetails& sd);

  /*! Lists all the existing serversin the system.
   *  @see server::Server;
   *  @see servers_list_
   */
  void list_servers() const;

  /*! Removes a server from the system.
   *
   *  To remove a server, you have to be its owner.
   *  @see check_server(); find_server(); servers_list_
   *  @see server::Server
   */
  void remove_server(string_view name);

  /*! Makes the logged-in user join a server.
   *
   *  If the server has an invite code, and you are not its owner, you have
   *  to input the valid code to be able to enter.
   *  @see current_server_; current_state_
   *  @see server::Server; server::ServerDetails
   */
  void enter_server(const ServerDetails& sd);

  /*! Leaves the current server.
   *
   *  As you can only visualize a server at a time, you need to leave the
   * current server if you wish to visualize another one. This does not remove
   * an user from the server's members list.
   *  @see server::Server; server::Server::members_ids_
   *  @see current_server_; current_state_
   */
  void leave_server();

  /*! List all the members of the current server.
   *  @see current_server_; get_user_name()
   *  @see server::Server::members_ids_
   */
  void list_participants() const;

 private:
  using enum SystemState;
  SystemState current_state_{kGuest}; /*!< The current state of the system */
  vector<User> users_list_;     /*!< The list of all users in the system */
  vector<Server> servers_list_; /*!< The list of all servers in the system */
  User logged_user_;            /*!< The current logged-in user */
  Server current_server_;       /*!< The current server being visualized */
  Channel current_channel_;     /*!< The current channel being visualized */
  int last_id_{};               /*!< The last user id generated by the system */
  unordered_set<string> guest_commands_{
      "create-user", "login"}; /*!< The commands allowed to a kGuest state. */
  unordered_set<string> logged_commands_{
      "create-server",
      "set-server-desc",
      "set-server-invite-code",
      "list-servers",
      "remove-server",
      "enter-server"}; /*!< The commands allowed to a kLogged_In state. */
  unordered_set<string> server_commands_{
      "leave-server", "list-participants"}; /*! The commands allowed to a
                                               kJoinedServer state. */
  // unordered_set<string> channel_commands_ {};
};

// Check if the command input in the CLI is valid.
bool check_command(const unordered_set<string>& s, string_view c);

// Parse the command part input in the CLI.
string parse_cmd(string_view cmd_line);

// Check if the command input in the CLI has any argument.
bool check_args(string_view cmd_line);

// Parse the argument part input in the CLI.
string parse_args(string_view cmd_line);

// Parse the details in the argument of a server command that needs it.
ServerDetails parse_details(string_view args, int cmd);

// Check if the user's id is equal to the parameter id.
bool check_id(const User& u, int id);

// Check if the user's address is equal to the parameter address.
bool check_address(const User& u, string_view a);

// Check if the user's password is equal to the parameter password.
bool check_password(const User& u, string_view p);

// Parse the credentials of a new user, splitting the arguments of create-user
// command.
tuple<EmailAddress, Password, Name> parse_new_credentials(string_view cred);

// Parse the credentials of an existing user, splitting the arguments of login
// command.
pair<EmailAddress, Password> parse_credentials(string_view cred);

// Check if a server's name is equal to the parameter name.
bool check_name(const Server& s, string_view name);

// Check if a server's owner id is equal to the id of an user.
bool check_owner(const Server& s, const User& u);

// Chech if an user is a member of said server.
bool check_member(const Server& s, const User& u);

// Some functions that print to the cout.
void print_abscent(string_view name);
void print_no_permission(string_view sv);
void print_info_changed(tuple<string_view, string_view, string_view> info);

}  // namespace concordo

#endif  // SYSTEM_H
