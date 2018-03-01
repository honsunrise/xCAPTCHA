//
// Created by zhsyourai on 8/15/17.
//

#ifndef XCAPTCHA_DETAIL_CONFIG_H
#define XCAPTCHA_DETAIL_CONFIG_H

#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

namespace captcha_config {
    namespace detail {

        template<typename BasicConfigType>
        class config_ref {
        public:
            using value_type = BasicConfigType;

            config_ref(value_type &&value) noexcept
                    : owned_value(std::move(value)),
                      value_ref(&owned_value),
                      is_rvalue(true) {}

            config_ref(const value_type &value)
                    : value_ref(const_cast<value_type *>(&value)),
                      is_rvalue(false) {}

            config_ref(std::initializer_list<config_ref> init)
                    : owned_value(init),
                      value_ref(&owned_value),
                      is_rvalue(true) {}

            template<class... Args>
            config_ref(Args... args)
                    : owned_value(std::forward<Args>(args)...),
                      value_ref(&owned_value),
                      is_rvalue(true) {}

            // class should be movable only
            config_ref(config_ref &&) noexcept = default;

            config_ref(const config_ref &) = delete;

            config_ref &operator=(const config_ref &) = delete;

            value_type moved_or_copied() const {
                if (is_rvalue) {
                    return std::move(*value_ref);
                } else {
                    return *value_ref;
                }
            }

            value_type const &operator*() const {
                return *static_cast<value_type const *>(value_ref);
            }

            value_type const *operator->() const {
                return static_cast<value_type const *>(value_ref);
            }

        private:
            mutable value_type owned_value = nullptr;
            value_type *value_ref = nullptr;
            const bool is_rvalue;
        };
    }
}
#endif //XCAPTCHA_DETAIL_CONFIG_H
