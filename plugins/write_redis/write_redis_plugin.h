//
// Created by zhsyourai on 8/4/17.
//

#ifndef XCAPTCHA_FILLING_PLUGIN_H
#define XCAPTCHA_FILLING_PLUGIN_H

#include <cstdint>
#include <cstddef>
#include <redis3m/redis3m.hpp>
#include "processor_plugin_interface.h"
using namespace redis3m;

class write_redis_plugin : public processor_plugin_interface {
 public:
  void initialization(const captcha_api &api) override;
  void release() override;
  captcha_config::config_define get_config_define() const override;
  void set_config(const captcha_config::config &node) override;
  void pipe(captcha &in) const override;
 private:
  const captcha_api *api;
  connection_pool::ptr_t pool;
  std::string sentinel;
  std::string master_name;
  uint32_t port;
};

extern "C" {
processor_plugin_interface *create();

void destroy(processor_plugin_interface *plugin_interface);
}

#endif //XCAPTCHA_FILLING_PLUGIN_H
