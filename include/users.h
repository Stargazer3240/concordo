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
      : name{name}, email{email}, password{password} {}

  [[nodiscard]] int getId() const { return id; }
  string& getName() { return name; }
  string& getEmail() { return email; }
  string& getPassword() { return password; }

 private:
  int id{};
  string name;
  string email;
  string password;
};

#endif  // USERS_H
