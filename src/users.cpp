#include "users.h"

namespace concordo {

ostream& operator<<(ostream& out, const User& u) { return out << u.address_; }

}  // namespace concordo
