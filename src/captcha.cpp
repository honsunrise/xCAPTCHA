//
// Created by zhsyourai on 8/4/17.
//

#include <opencv2/core/mat.hpp>
#include "captcha.h"
captcha::captcha() : i(nullptr), a(nullptr) {
}

captcha::captcha(const captcha &other) {
  if (other.i != nullptr)
    i = new image(*other.i);
  if (other.a != nullptr)
    a = new answer(*other.a);
}

captcha::captcha(captcha &&other) noexcept {
  i = other.i;
  a = other.a;
  other.i = nullptr;
  other.a = nullptr;
}

captcha &captcha::operator=(const captcha &other) {
  delete i;
  delete a;
  i = nullptr;
  a = nullptr;
  if (other.i != nullptr)
    i = new image(*other.i);
  if (other.a != nullptr)
    a = new answer(*other.a);
  return *this;
}

captcha &captcha::operator=(captcha &&other) noexcept {
  delete i;
  delete a;
  i = other.i;
  a = other.a;
  other.i = nullptr;
  other.a = nullptr;
  return *this;
}

captcha::~captcha() {
  delete i;
  delete a;
  i = nullptr;
  a = nullptr;
}

captcha::captcha(image &&i) {
  this->i = new image(std::forward<image>(i));
  this->a = nullptr;
}

captcha::captcha(image &&i, answer &&a) {
  this->i = new image(std::forward<image>(i));
  this->a = new answer(std::forward<answer>(a));
}
image &captcha::get_image() {
  return *i;
}
answer &captcha::get_answer() {
  return *a;
}
const image &captcha::get_image() const {
  return *i;
}
const answer &captcha::get_answer() const {
  return *a;
}