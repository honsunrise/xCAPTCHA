//
// Created by zhsyourai on 8/4/17.
//
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/algorithm/string.hpp>

#include "write_redis_plugin.h"

void write_redis_plugin::initialization(const captcha_api &api) {
  this->api = &api;
}

void write_redis_plugin::release() {

}

captcha_config::config_define write_redis_plugin::get_config_define() const {
  captcha_config::config_define define;
  define["sentinel"] = "";
  define["master_name"] = "";
  define["port"] = 26379;
  return define;
}

void write_redis_plugin::set_config(const captcha_config::config &node) {
  sentinel = node["sentinel"];
  master_name = node["master_name"];
  port = node["port"];
  pool = connection_pool::create(sentinel, master_name, port);
}

std::vector<uint8_t> base64_decode(const std::string &val) {
  using namespace boost::archive::iterators;
  using IT = transform_width<binary_from_base64<std::string::const_iterator>, 8, 6>;
  return std::vector<uint8_t>(IT(std::begin(val)), IT(std::end(val)));
}

std::string base64_encode(const std::vector<uint8_t> &val) {
  using namespace boost::archive::iterators;
  using IT = base64_from_binary<transform_width<std::vector<uint8_t>::const_iterator, 6, 8>>;
  auto tmp = std::string(IT(std::begin(val)), IT(std::end(val)));
  return tmp.append((3 - val.size() % 3) % 3, '=');
}

void write_redis_plugin::pipe(captcha &in) const {
  const image &img = in.get_image();
  const answer &ans = in.get_answer();
  cv::Mat mat = img;
  vector<int> compression_params;
  compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
  compression_params.push_back(9);
  std::vector<uint8_t> result;
  assert(cv::imencode(".png", mat, result, compression_params));
#if 0
  std::cout << base64_encode(result) << std::endl;
#else
  connection::ptr_t c = pool->get(connection::MASTER);
  c->run(command("HMSET") << "image" << base64_encode(result) << "answer");
  pool->put(c);
#endif
}

processor_plugin_interface *create() {
  return new write_redis_plugin();
}

void destroy(processor_plugin_interface *plugin_interface) {
  delete plugin_interface;
}
