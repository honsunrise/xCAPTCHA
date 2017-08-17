//
// Created by zhsyourai on 8/17/17.
//

#ifndef XCAPTCHA_EXCEPTION_H
#define XCAPTCHA_EXCEPTION_H

#include <string>
#include <stdexcept>

class exception : public std::exception {
 public:
  const char *what() const noexcept override {
    return m.what();
  }

  const int id;

 protected:
  exception(int id_, const char *what_arg) : id(id_), m(what_arg) {}

  static std::string name(const std::string &e_name, int id) {
    return "["e_name + ":" + std::to_string(id) + "] ";
  }

 private:
  std::runtime_error m;
};

#endif //XCAPTCHA_EXCEPTION_H
