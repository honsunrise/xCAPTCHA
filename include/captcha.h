//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_CAPTCHA_H
#define XCAPTCHA_CAPTCHA_H
#include <image.h>
#include <answer.h>

class captcha {
 public:
  captcha();
  captcha(const captcha &other);
  captcha(captcha &&other) noexcept;

  captcha(image &&i);
  captcha(image &&i, answer &&a);

  captcha &operator=(const captcha &other);
  captcha &operator=(captcha &&other) noexcept;
  image &get_image();
  answer &get_answer();

  const image &get_image() const;
  const answer &get_answer() const;

  virtual ~captcha();
 private:
  image *i;
  answer *a;
};

#endif //XCAPTCHA_CAPTCHA_H
