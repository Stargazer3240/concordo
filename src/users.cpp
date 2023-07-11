// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "users.h"

namespace concordo {

void User::save(fstream& f) {
  f << id_ << '\n';
  f << name_ << '\n';
  f << address_ << '\n';
  f << password_ << '\n';
}

ostream& operator<<(ostream& out, const User& u) { return out << u.address_; }

}  // namespace concordo
