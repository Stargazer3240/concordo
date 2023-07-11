#include "users.h"

#include <ostream>

namespace concordo {

void User::save_password(ofstream& f) { f << password_; }

ostream& operator<<(ostream& out, const User& u) { return out << u.address_; }

}  // namespace concordo
