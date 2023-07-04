#ifndef USER_H
#define USER_H
#include <string>

namespace user {

using std::string;

struct Credentials {
  string email;
  string password;
  string name;
};

class User {
 public:
  User(int id, const Credentials& c): id_{id}, email_{c.email}, password_{c.password}, name_{c.name}{}

 private:
  int id_{};
  string email_;
  string password_;
  string name_;
};

} // namespace user
#endif // USER_H
