// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>

namespace concordo::user {

using std::string;

struct Credentials {
  string name;
  string address;
  string password;
};

class User {
 public:
  User() = default;
  User(int id, const Credentials& c)
      : id_{id}, name_{c.name}, address_{c.address}, password_{c.password} {}

  [[nodiscard]] int getId() const { return id_; }
  [[nodiscard]] string getName() const { return name_; }
  [[nodiscard]] string getEmail() const { return address_; }
  [[nodiscard]] string getPassword() const { return password_; }

 private:
  int id_{};
  string name_;
  string address_;
  string password_;
};

}  // namespace concordo::user

#endif  // USERS_H
