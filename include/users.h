// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#ifndef USERS_H
#define USERS_H

#include <string>
#include <string_view>

class User {
 public:
  User(std::string_view name, std::string_view email,  // NOLINT
       std::string_view password)                      // NOLINT
      : name{name}, email{email}, password{password} {}

  [[nodiscard]] int getId() const { return id; }
  std::string& getName() { return name; }
  std::string& getEmail() { return email; }
  std::string& getPassword() { return password; }

 private:
  int id{};
  std::string name;
  std::string email;
  std::string password;
};

#endif  // USERS_H
