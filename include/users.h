// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <fstream>
#include <string>
#include <string_view>

#include "channels.h"

namespace concordo {

using std::string, std::string_view, std::ostream, std::fstream;

/*! A struct that contains user credentials.
 *
 *  The user credentials are used as an input to the constructor of the user
 *  class.
 *  @see User; User::name_; User::address_; User::password_
 */
struct UserCredentials {
  string address;  /*!< An user email address to be input from the system. */
  string password; /*!< An user password to be input from the system. */
  string name;     /*!< An user name to be input from the system. */
};

/*! A class that represents an user in the Concordo app.
 *
 *  An user is capable of creating/accessing servers and channels, and capable
 *  of sending messages.
 *  @see channel::Channel; server::Server; channel::Message
 */
class User {
 public:
  /*! Default constructor to be used by std::vector class.
   *  @see concordo::System::users_list_
   */
  User() = default;

  /*! A constructor to be used by the system.
   *  @see concordo::System::create_user()
   *  @see Credentials
   */
  User(int id, const UserCredentials& c)
      : id_{id}, name_{c.name}, address_{c.address}, password_{c.password} {}

  /*! @see id_ */
  [[nodiscard]] int getId() const { return id_; }
  [[nodiscard]] string getName() const { return name_; }
  [[nodiscard]] string getEmail() const { return address_; }

  [[nodiscard]] bool check_id(int id) const { return id_ == id; }

  [[nodiscard]] bool check_address(string_view a) const {
    return address_ == a;
  }

  [[nodiscard]] bool check_password(string_view p) const {
    return password_ == p;
  }

  void send_message(Channel* c, string_view msg) const {
    c->send_message({id_, msg});
  }

  void save(fstream& f);

  friend ostream& operator<<(ostream& out, const User& u);

 private:
  int id_{};        /*!< The user's unique id created by the system. */
  string name_;     /*!< The user's name. */
  string address_;  /*!< The user's email address. Has to be unique. */
  string password_; /*!< The user's password. */
};

ostream& operator<<(ostream& out, const User& u);

}  // namespace concordo

#endif  // USERS_H
