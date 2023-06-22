// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "users.h"

namespace concordo::user {

ostream& operator<<(ostream& os, const Name& n) { return os << n.name; }

ostream& operator<<(ostream& os, const EmailAddress& a) {
  return os << a.email_address;
}

ostream& operator<<(ostream& os, const Password& p) { return os << p.password; }

}  // namespace concordo::user
