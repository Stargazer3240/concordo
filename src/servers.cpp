// SPDX-FileCopyrightText: 2023 Fabrício Moura Jácome
//
// SPDX-License-Identifier: MIT

#include "servers.h"

namespace concordo::server {

bool check_name(const Server& s, string_view name) {
  return s.getName() == name;
}

}  // namespace concordo::server
