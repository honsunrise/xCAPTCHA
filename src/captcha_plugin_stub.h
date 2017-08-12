//
// Created by zhsyourai on 8/10/17.
//

#ifndef XCAPTCHA_CAPTCHA_PLUGIN_STUB_H
#define XCAPTCHA_CAPTCHA_PLUGIN_STUB_H
#include <string>
#include <boost/core/noncopyable.hpp>
#include "processor_plugin_interface.h"

class captcha_plugin_stub : boost::noncopyable {
 public:
  captcha_plugin_stub();
  explicit captcha_plugin_stub(const std::string &plugin_path);
  virtual ~captcha_plugin_stub();
  processor_plugin_interface *get_interface();

  captcha_plugin_stub(captcha_plugin_stub&& rhs) noexcept ;
  captcha_plugin_stub& operator=(captcha_plugin_stub&& rhs) noexcept;
 private:
  void* handle;
  std::string plugin_path;
  processor_plugin_interface *plugin_interface;
  create_t* create_fn;
  destroy_t* destroy_fn;
};

#endif //XCAPTCHA_CAPTCHA_PLUGIN_STUB_H
