// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>

using std::string_view, std::string;

class User {
 public:
  User(string_view name, string_view email, string_view password)  // NOLINT
      : name_{name}, email_{email}, password_{password} {}

  [[nodiscard]] int getId() const { return id_; }
  [[nodiscard]] string getName() const { return name_; }
  [[nodiscard]] string getEmail() const { return email_; }
  [[nodiscard]] string getPassword() const { return password_; }

 private:
  int id_{};
  string name_;
  string email_;
  string password_;
};

#endif  // USERS_H
