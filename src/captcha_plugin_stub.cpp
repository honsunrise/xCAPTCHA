//
// Created by zhsyourai on 8/10/17.
//
#include <dlfcn.h>
#include "captcha_plugin_stub.h"
captcha_plugin_stub::captcha_plugin_stub() {}

captcha_plugin_stub::captcha_plugin_stub(const std::string &plugin_path) : plugin_path(plugin_path), handle(nullptr) {
  handle = dlopen(plugin_path.c_str(), RTLD_LAZY);
  if (!handle) {
    std::cerr << "Cannot load library: " << dlerror() << '\n';
  }
  create_fn = (create_t*) dlsym(handle, "create");
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "Cannot load symbol create: " << dlsym_error << '\n';
  }

  destroy_fn = (destroy_t*) dlsym(handle, "destroy");
  dlsym_error = dlerror();
  if (dlsym_error) {
    std::cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
  }

  plugin_interface = create_fn();
}

captcha_plugin_stub::captcha_plugin_stub(captcha_plugin_stub &&rhs) noexcept  {
  handle = rhs.handle;
  plugin_path = std::move(rhs.plugin_path);
  plugin_interface = rhs.plugin_interface;
  create_fn = rhs.create_fn;
  destroy_fn = rhs.destroy_fn;
}

captcha_plugin_stub &captcha_plugin_stub::operator=(captcha_plugin_stub &&rhs) noexcept {
  handle = rhs.handle;
  plugin_path = std::move(rhs.plugin_path);
  plugin_interface = rhs.plugin_interface;
  create_fn = rhs.create_fn;
  destroy_fn = rhs.destroy_fn;
  return *this;
}

captcha_plugin_stub::~captcha_plugin_stub() {
  if (!handle) {
    destroy_fn(plugin_interface);
    dlclose(handle);
  }
}

processor_plugin_interface *captcha_plugin_stub::get_interface() {
  return plugin_interface;
}
