// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>

namespace concordo {

using std::string;

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

  /*! @see name_ */
  [[nodiscard]] string getName() const { return name_; }

  /*! @see address_ */
  [[nodiscard]] string getEmail() const { return address_; }

  /*! @see password_ */
  [[nodiscard]] string getPassword() const { return password_; }

 private:
  int id_{};        /*!< The user's unique id created by the system. */
  string name_;     /*!< The user's name. */
  string address_;  /*!< The user's email address. Has to be unique. */
  string password_; /*!< The user's password. */
};

}  // namespace concordo

#endif  // USERS_H
