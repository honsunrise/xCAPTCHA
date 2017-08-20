//
// Created by zhsyourai on 8/7/17.
//

#ifndef XCAPTCHA_CAPTCHA_CONFIG_H
#define XCAPTCHA_CAPTCHA_CONFIG_H
#include <string>
#include <utility>
#include <vector>
#include <map>
#include <typeinfo>
#include "config/detail/ref.h"
#include "config/detail/value_type.h"
#include "config/detail/exception.h"
#include "config/detail/utils.h"
#include "config/detail/iterator.h"
#include "config/detail/typed_constructor.h"
#include "config/detail/serializer.h"
#include "config/detail/declaration.h"
#include "config/rules.h"
#include "config/config_path.h"

namespace captcha_config {

XCAPTCHA_BASIC_CONFIG_TPL_DECLARATION
class basic_config {
 private:
  friend config_path;
  template<detail::value_t> friend
  struct detail::typed_constructor;
  template<typename BasicConfigType> friend
  class detail::iter_impl;

  using primitive_iterator_t = detail::primitive_iterator_t;
  template<typename ConfigType>
  using internal_iterator = detail::internal_iterator<ConfigType>;
  template<typename ConfigType>
  using iter_impl = detail::iter_impl<ConfigType>;
  template<typename Base> using conofig_reverse_iterator = detail::config_reverse_iterator<Base>;

 public:
  using value_t = detail::value_t;
  template<typename T, typename SFINAE>
  using serializer = Serializer<T, SFINAE>;

  using initializer_list_t = std::initializer_list<detail::config_ref<basic_config>>;


  using exception = detail::exception;
  using parse_error = detail::parse_error;
  using invalid_iterator = detail::invalid_iterator;
  using type_error = detail::type_error;
  using out_of_range = detail::out_of_range;
  using other_error = detail::other_error;


  using value_type = basic_config;
  using reference = value_type &;
  using const_reference = const value_type &;

  using difference_type = std::ptrdiff_t;
  using size_type = std::size_t;

  using allocator_type = AllocatorType<basic_config>;

  using pointer = typename std::allocator_traits<allocator_type>::pointer;
  using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

  using iterator = iter_impl<basic_config>;
  using const_iterator = iter_impl<const basic_config>;
  using reverse_iterator = conofig_reverse_iterator<typename basic_config::iterator>;
  using const_reverse_iterator = conofig_reverse_iterator<typename basic_config::const_iterator>;

  static allocator_type get_allocator() {
    return allocator_type();
  }

  using map_t = ObjectType<StringType,
                           basic_config,
                           std::less<StringType>,
                           AllocatorType<std::pair<const StringType, basic_config>>>;


  using array_t = ArrayType<basic_config, AllocatorType<basic_config>>;

  using string_t = StringType;

  using boolean_t = BooleanType;

  using number_integer_t = NumberIntegerType;

  using number_unsigned_t = NumberUnsignedType;

  using number_float_t = NumberFloatType;

 private:

  template<typename T, typename... Args>
  static T *create(Args &&... args) {
    AllocatorType<T> alloc;
    auto deleter = [&](T *object) {
      alloc.deallocate(object, 1);
    };
    std::unique_ptr<T, decltype(deleter)> object(alloc.allocate(1), deleter);
    alloc.construct(object.get(), std::forward<Args>(args)...);
    assert(object != nullptr);
    return object.release();
  }

  union config_value {
    map_t *map;
    array_t *array;
    string_t *string;
    boolean_t boolean;
    number_integer_t number_integer;
    number_unsigned_t number_unsigned;
    number_float_t number_float;

    config_value() = default;
    config_value(boolean_t v) noexcept : boolean(v) {}
    config_value(number_integer_t v) noexcept : number_integer(v) {}
    config_value(number_unsigned_t v) noexcept : number_unsigned(v) {}
    config_value(number_float_t v) noexcept : number_float(v) {}
    config_value(value_t t) {
      switch (t) {
        case value_t::map: {
          map = create<map_t>();
          break;
        }

        case value_t::array: {
          array = create<array_t>();
          break;
        }

        case value_t::string: {
          string = create<string_t>("");
          break;
        }

        case value_t::boolean: {
          boolean = boolean_t(false);
          break;
        }

        case value_t::number_integer: {
          number_integer = number_integer_t(0);
          break;
        }

        case value_t::number_unsigned: {
          number_unsigned = number_unsigned_t(0);
          break;
        }

        case value_t::number_float: {
          number_float = number_float_t(0.0);
          break;
        }

        case value_t::null: {
          break;
        }

        default: {
          if (JSON_UNLIKELY(t == value_t::null)) {
            JSON_THROW(other_error::create(500, "961c151d2e87f2686a955a9be24d316f1362bf21 2.1.1")); // LCOV_EXCL_LINE
          }
          break;
        }
      }
    }

    /// constructor for strings
    config_value(const string_t &value) {
      string = create<string_t>(value);
    }

    /// constructor for rvalue strings
    config_value(string_t &&value) {
      string = create<string_t>(std::move(value));
    }

    /// constructor for objects
    config_value(const map_t &value) {
      map = create<map_t>(value);
    }

    /// constructor for rvalue objects
    config_value(map_t &&value) {
      map = create<map_t>(std::move(value));
    }

    /// constructor for arrays
    config_value(const array_t &value) {
      array = create<array_t>(value);
    }

    /// constructor for rvalue arrays
    config_value(array_t &&value) {
      array = create<array_t>(std::move(value));
    }

    void destroy(value_t t) {
      switch (t) {
        case value_t::map: {
          AllocatorType<map_t> alloc;
          alloc.destroy(map);
          alloc.deallocate(map, 1);
          break;
        }

        case value_t::array: {
          AllocatorType<array_t> alloc;
          alloc.destroy(array);
          alloc.deallocate(array, 1);
          break;
        }

        case value_t::string: {
          AllocatorType<string_t> alloc;
          alloc.destroy(string);
          alloc.deallocate(string, 1);
          break;
        }

        default: {
          break;
        }
      }
    }
  };

  void assert_invariant() const {
    assert(m_type != value_t::map or m_value.map != nullptr);
    assert(m_type != value_t::array or m_value.array != nullptr);
    assert(m_type != value_t::string or m_value.string != nullptr);
  }

 public:
  basic_config(const value_t v)
      : m_type(v), m_value(v) {
    assert_invariant();
  }

  basic_config(std::nullptr_t = nullptr) noexcept
      : basic_config(value_t::null) {
    assert_invariant();
  }

  template<typename CompatibleType, typename U = utils::uncvref_t<CompatibleType>,
      utils::enable_if_t<not std::is_base_of<std::istream, U>::value and
          not std::is_same<U, basic_config>::value and
          not utils::is_basic_json_nested_type<basic_config, U>::value and
          utils::has_to_json<basic_config, U>::value,
                         int> = 0>
  basic_config(CompatibleType &&val) noexcept(noexcept(Serializer<U>::to_json(
      std::declval<basic_config &>(), std::forward<CompatibleType>(val)))) {
    Serializer<U>::to_json(*this, std::forward<CompatibleType>(val));
    assert_invariant();
  }

  basic_config(initializer_list_t init,
               bool type_deduction = true,
               value_t manual_type = value_t::array) {
    // check if each element is an array with two elements whose first
    // element is a string
    bool is_an_object = std::all_of(init.begin(), init.end(),
                                    [](const detail::config_ref<basic_config> &element_ref) {
                                      return (element_ref->is_array() and element_ref->size() == 2
                                          and (*element_ref)[0].is_string());
                                    });

    // adjust type if type deduction is not wanted
    if (not type_deduction) {
      // if array is wanted, do not create an map though possible
      if (manual_type == value_t::array) {
        is_an_object = false;
      }

      // if map is wanted but impossible, throw an exception
      if (JSON_UNLIKELY(manual_type == value_t::map and not is_an_object)) {
        JSON_THROW(type_error::create(301, "cannot create map from initializer list"));
      }
    }

    if (is_an_object) {
      // the initializer list is a list of pairs -> create map
      m_type = value_t::map;
      m_value = value_t::map;

      std::for_each(init.begin(), init.end(), [this](const detail::config_ref<basic_config> &element_ref) {
        auto element = element_ref.moved_or_copied();
        m_value.map->emplace(
            std::move(*((*element.m_value.array)[0].m_value.string)),
            std::move((*element.m_value.array)[1]));
      });
    } else {
      // the initializer list describes an array -> create array
      m_type = value_t::array;
      m_value.array = create<array_t>(init.begin(), init.end());
    }

    assert_invariant();
  }

  static basic_config array(initializer_list_t init = {}) {
    return basic_config(init, false, value_t::array);
  }

  static basic_config object(initializer_list_t init = {}) {
    return basic_config(init, false, value_t::map);
  }

  basic_config(size_type cnt, const basic_config &val)
      : m_type(value_t::array) {
    m_value.array = create<array_t>(cnt, val);
    assert_invariant();
  }

  template<class InputIT, typename std::enable_if<
      std::is_same<InputIT, typename basic_config::iterator>::value or
          std::is_same<InputIT, typename basic_config::const_iterator>::value, int>::type = 0>
  basic_config(InputIT first, InputIT last) {
    assert(first.m_object != nullptr);
    assert(last.m_object != nullptr);

    // make sure iterator fits the current value
    if (JSON_UNLIKELY(first.m_object != last.m_object)) {
      JSON_THROW(invalid_iterator::create(201, "iterators are not compatible"));
    }

    // copy type from first iterator
    m_type = first.m_object->m_type;

    // check if iterator range is complete for primitive values
    switch (m_type) {
      case value_t::boolean:
      case value_t::number_float:
      case value_t::number_integer:
      case value_t::number_unsigned:
      case value_t::string: {
        if (JSON_UNLIKELY(not first.m_it.primitive_iterator.is_begin()
                              or not last.m_it.primitive_iterator.is_end())) {
          JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
        }
        break;
      }

      default:break;
    }

    switch (m_type) {
      case value_t::number_integer: {
        m_value.number_integer = first.m_object->m_value.number_integer;
        break;
      }

      case value_t::number_unsigned: {
        m_value.number_unsigned = first.m_object->m_value.number_unsigned;
        break;
      }

      case value_t::number_float: {
        m_value.number_float = first.m_object->m_value.number_float;
        break;
      }

      case value_t::boolean: {
        m_value.boolean = first.m_object->m_value.boolean;
        break;
      }

      case value_t::string: {
        m_value = *first.m_object->m_value.string;
        break;
      }

      case value_t::map: {
        m_value.map = create<map_t>(first.m_it.object_iterator,
                                    last.m_it.object_iterator);
        break;
      }

      case value_t::array: {
        m_value.array = create<array_t>(first.m_it.array_iterator,
                                        last.m_it.array_iterator);
        break;
      }

      default:JSON_THROW(invalid_iterator::create(206, "cannot construct with iterators from " +
            std::string(first.m_object->type_name())));
    }

    assert_invariant();
  }

  basic_config(const detail::config_ref<basic_config> &ref)
      : basic_config(ref.moved_or_copied()) {}

  basic_config(const basic_config &other)
      : m_type(other.m_type) {
    // check of passed value is valid
    other.assert_invariant();

    switch (m_type) {
      case value_t::map: {
        m_value = *other.m_value.map;
        break;
      }

      case value_t::array: {
        m_value = *other.m_value.array;
        break;
      }

      case value_t::string: {
        m_value = *other.m_value.string;
        break;
      }

      case value_t::boolean: {
        m_value = other.m_value.boolean;
        break;
      }

      case value_t::number_integer: {
        m_value = other.m_value.number_integer;
        break;
      }

      case value_t::number_unsigned: {
        m_value = other.m_value.number_unsigned;
        break;
      }

      case value_t::number_float: {
        m_value = other.m_value.number_float;
        break;
      }

      default:break;
    }

    assert_invariant();
  }

  basic_config(basic_config &&other) noexcept
      : m_type(std::move(other.m_type)),
        m_value(std::move(other.m_value)) {
    // check that passed value is valid
    other.assert_invariant();

    // invalidate payload
    other.m_type = value_t::null;
    other.m_value = {};

    assert_invariant();
  }

  reference &operator=(basic_config other) noexcept(
  std::is_nothrow_move_constructible<value_t>::value and
      std::is_nothrow_move_assignable<value_t>::value and
      std::is_nothrow_move_constructible<config_value>::value and
      std::is_nothrow_move_assignable<config_value>::value
  ) {
    // check that passed value is valid
    other.assert_invariant();

    using std::swap;
    swap(m_type, other.m_type);
    swap(m_value, other.m_value);

    assert_invariant();
    return *this;
  }

  ~basic_config() {
    assert_invariant();
    m_value.destroy(m_type);
  }

 public:
  constexpr value_t type() const noexcept {
    return m_type;
  }

  constexpr bool is_primitive() const noexcept {
    return is_null() or is_string() or is_boolean() or is_number();
  }

  constexpr bool is_structured() const noexcept {
    return is_array() or is_map();
  }

  constexpr bool is_null() const noexcept {
    return (m_type == value_t::null);
  }

  constexpr bool is_boolean() const noexcept {
    return (m_type == value_t::boolean);
  }

  constexpr bool is_number() const noexcept {
    return is_number_integer() or is_number_float();
  }

  constexpr bool is_number_integer() const noexcept {
    return (m_type == value_t::number_integer or m_type == value_t::number_unsigned);
  }

  constexpr bool is_number_unsigned() const noexcept {
    return (m_type == value_t::number_unsigned);
  }

  constexpr bool is_number_float() const noexcept {
    return (m_type == value_t::number_float);
  }

  constexpr bool is_map() const noexcept {
    return (m_type == value_t::map);
  }

  constexpr bool is_array() const noexcept {
    return (m_type == value_t::array);
  }

  constexpr bool is_string() const noexcept {
    return (m_type == value_t::string);
  }

  constexpr bool is_discarded() const noexcept {
    return (m_type == value_t::discarded);
  }

  constexpr operator value_t() const noexcept {
    return m_type;
  }

 private:
  boolean_t get_impl(boolean_t * /*unused*/) const {
    if (JSON_LIKELY(is_boolean())) {
      return m_value.boolean;
    }

    JSON_THROW(type_error::create(302, "type must be boolean, but is " + std::string(type_name())));
  }

  map_t *get_impl_ptr(map_t * /*unused*/) noexcept {
    return is_map() ? m_value.map : nullptr;
  }

  constexpr const map_t *get_impl_ptr(const map_t * /*unused*/) const noexcept {
    return is_map() ? m_value.map : nullptr;
  }

  array_t *get_impl_ptr(array_t * /*unused*/) noexcept {
    return is_array() ? m_value.array : nullptr;
  }

  constexpr const array_t *get_impl_ptr(const array_t * /*unused*/) const noexcept {
    return is_array() ? m_value.array : nullptr;
  }

  string_t *get_impl_ptr(string_t * /*unused*/) noexcept {
    return is_string() ? m_value.string : nullptr;
  }

  constexpr const string_t *get_impl_ptr(const string_t * /*unused*/) const noexcept {
    return is_string() ? m_value.string : nullptr;
  }

  boolean_t *get_impl_ptr(boolean_t * /*unused*/) noexcept {
    return is_boolean() ? &m_value.boolean : nullptr;
  }

  constexpr const boolean_t *get_impl_ptr(const boolean_t * /*unused*/) const noexcept {
    return is_boolean() ? &m_value.boolean : nullptr;
  }

  number_integer_t *get_impl_ptr(number_integer_t * /*unused*/) noexcept {
    return is_number_integer() ? &m_value.number_integer : nullptr;
  }

  constexpr const number_integer_t *get_impl_ptr(const number_integer_t * /*unused*/) const noexcept {
    return is_number_integer() ? &m_value.number_integer : nullptr;
  }

  number_unsigned_t *get_impl_ptr(number_unsigned_t * /*unused*/) noexcept {
    return is_number_unsigned() ? &m_value.number_unsigned : nullptr;
  }

  constexpr const number_unsigned_t *get_impl_ptr(const number_unsigned_t * /*unused*/) const noexcept {
    return is_number_unsigned() ? &m_value.number_unsigned : nullptr;
  }

  number_float_t *get_impl_ptr(number_float_t * /*unused*/) noexcept {
    return is_number_float() ? &m_value.number_float : nullptr;
  }

  constexpr const number_float_t *get_impl_ptr(const number_float_t * /*unused*/) const noexcept {
    return is_number_float() ? &m_value.number_float : nullptr;
  }

  template<typename ReferenceType, typename ThisType>
  static ReferenceType get_ref_impl(ThisType &obj) {
    // delegate the call to get_ptr<>()
    auto ptr = obj.template get_ptr<typename std::add_pointer<ReferenceType>::type>();

    if (JSON_LIKELY(ptr != nullptr)) {
      return *ptr;
    }

    JSON_THROW(type_error::create(303,
                                  "incompatible ReferenceType for get_ref, actual type is "
                                      + std::string(obj.type_name())));
  }

 public:
  template<
      typename BasicConfigType,
      utils::enable_if_t<std::is_same<typename std::remove_const<BasicConfigType>::type,
                                      basic_config>::value,
                         int> = 0>
  basic_config get() const {
    return *this;
  }

  template<
      typename ValueTypeCV,
      typename ValueType = utils::uncvref_t<ValueTypeCV>,
      utils::enable_if_t<
          not std::is_same<basic_config, ValueType>::value and
              utils::has_from_json<basic_config, ValueType>::value and
              not utils::has_non_default_from_json<basic_config, ValueType>::value,
          int> = 0>
  ValueType get() const noexcept(noexcept(
  Serializer<ValueType>::from_json(std::declval<const basic_config &>(), std::declval<ValueType &>()))) {
    // we cannot static_assert on ValueTypeCV being non-const, because
    // there is support for get<const basic_config_define_t>(), which is why we
    // still need the uncvref
    static_assert(not std::is_reference<ValueTypeCV>::value,
                  "get() cannot be used with reference types, you might want to use get_ref()");
    static_assert(std::is_default_constructible<ValueType>::value,
                  "types must be DefaultConstructible when used with get()");

    ValueType ret;
    Serializer<ValueType>::from_json(*this, ret);
    return ret;
  }

  template<
      typename ValueTypeCV,
      typename ValueType = utils::uncvref_t<ValueTypeCV>,
      utils::enable_if_t<not std::is_same<basic_config, ValueType>::value and
          utils::has_non_default_from_json<basic_config,
                                           ValueType>::value, int> = 0>
  ValueType get() const noexcept(noexcept(
  Serializer<ValueTypeCV>::from_json(std::declval<const basic_config &>()))) {
    static_assert(not std::is_reference<ValueTypeCV>::value,
                  "get() cannot be used with reference types, you might want to use get_ref()");
    return Serializer<ValueTypeCV>::from_json(*this);
  }

  template<typename PointerType, typename std::enable_if<
      std::is_pointer<PointerType>::value, int>::type = 0>
  PointerType get() noexcept {
    // delegate the call to get_ptr
    return get_ptr<PointerType>();
  }

  template<typename PointerType, typename std::enable_if<
      std::is_pointer<PointerType>::value, int>::type = 0>
  constexpr const PointerType get() const noexcept {
    // delegate the call to get_ptr
    return get_ptr<PointerType>();
  }

  template<typename PointerType, typename std::enable_if<
      std::is_pointer<PointerType>::value, int>::type = 0>
  PointerType get_ptr() noexcept {
    // get the type of the PointerType (remove pointer and const)
    using pointee_t = typename std::remove_const<typename
                                                 std::remove_pointer<typename
                                                                     std::remove_const<PointerType>::type>::type>::type;
    // make sure the type matches the allowed types
    static_assert(
        std::is_same<map_t, pointee_t>::value
            or std::is_same<array_t, pointee_t>::value
            or std::is_same<string_t, pointee_t>::value
            or std::is_same<boolean_t, pointee_t>::value
            or std::is_same<number_integer_t, pointee_t>::value
            or std::is_same<number_unsigned_t, pointee_t>::value
            or std::is_same<number_float_t, pointee_t>::value, "incompatible pointer type");

    // delegate the call to get_impl_ptr<>()
    return get_impl_ptr(static_cast<PointerType>(nullptr));
  }

  template<typename PointerType, typename std::enable_if<
      std::is_pointer<PointerType>::value and
          std::is_const<typename std::remove_pointer<PointerType>::type>::value, int>::type = 0>
  constexpr const PointerType get_ptr() const noexcept {
    // get the type of the PointerType (remove pointer and const)
    using pointee_t = typename std::remove_const<typename
                                                 std::remove_pointer<typename
                                                                     std::remove_const<PointerType>::type>::type>::type;
    // make sure the type matches the allowed types
    static_assert(
        std::is_same<map_t, pointee_t>::value
            or std::is_same<array_t, pointee_t>::value
            or std::is_same<string_t, pointee_t>::value
            or std::is_same<boolean_t, pointee_t>::value
            or std::is_same<number_integer_t, pointee_t>::value
            or std::is_same<number_unsigned_t, pointee_t>::value
            or std::is_same<number_float_t, pointee_t>::value, "incompatible pointer type");

    // delegate the call to get_impl_ptr<>() const
    return get_impl_ptr(static_cast<const PointerType>(nullptr));
  }

  template<typename ReferenceType, typename std::enable_if<
      std::is_reference<ReferenceType>::value, int>::type = 0>
  ReferenceType get_ref() {
    // delegate call to get_ref_impl
    return get_ref_impl<ReferenceType>(*this);
  }

  template<typename ReferenceType, typename std::enable_if<
      std::is_reference<ReferenceType>::value and
          std::is_const<typename std::remove_reference<ReferenceType>::type>::value, int>::type = 0>
  ReferenceType get_ref() const {
    // delegate call to get_ref_impl
    return get_ref_impl<ReferenceType>(*this);
  }

  template<typename ValueType, typename std::enable_if<
      not std::is_pointer<ValueType>::value and
          not std::is_same<ValueType, detail::config_ref<basic_config>>::value and
          not std::is_same<ValueType, typename string_t::value_type>::value
#ifndef _MSC_VER  // fix for issue #167 operator<< ambiguity under VS2015
          and not std::is_same<ValueType, std::initializer_list<typename string_t::value_type>>::value
#endif
#if (defined(__cplusplus) && __cplusplus >= 201703L) || (defined(_MSC_VER) && _MSC_VER > 1900 && defined(_HAS_CXX17) && _HAS_CXX17 == 1) // fix for issue #464
      and not std::is_same<ValueType, typename std::string_view>::value
#endif
      , int>::type = 0>
  operator ValueType() const {
    // delegate the call to get<>() const
    return get<ValueType>();
  }

  reference at(size_type idx) {
    // at only works for arrays
    if (JSON_LIKELY(is_array())) {
      JSON_TRY {
        return m_value.array->at(idx);
      }
      JSON_CATCH (std::out_of_range &) {
        // create better exception explanation
        JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
      }
    } else {
      JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
    }
  }

  const_reference at(size_type idx) const {
    // at only works for arrays
    if (JSON_LIKELY(is_array())) {
      JSON_TRY {
        return m_value.array->at(idx);
      }
      JSON_CATCH (std::out_of_range &) {
        // create better exception explanation
        JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
      }
    } else {
      JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
    }
  }

  reference at(const typename map_t::key_type &key) {
    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      JSON_TRY {
        return m_value.map->at(key);
      }
      JSON_CATCH (std::out_of_range &) {
        // create better exception explanation
        JSON_THROW(out_of_range::create(403, "key '" + key + "' not found"));
      }
    } else {
      JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
    }
  }

  const_reference at(const typename map_t::key_type &key) const {
    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      JSON_TRY {
        return m_value.map->at(key);
      }
      JSON_CATCH (std::out_of_range &) {
        // create better exception explanation
        JSON_THROW(out_of_range::create(403, "key '" + key + "' not found"));
      }
    } else {
      JSON_THROW(type_error::create(304, "cannot use at() with " + std::string(type_name())));
    }
  }

  reference operator[](size_type idx) {
    // implicitly convert null value to an empty array
    if (is_null()) {
      m_type = value_t::array;
      m_value.array = create<array_t>();
      assert_invariant();
    }

    // operator[] only works for arrays
    if (JSON_LIKELY(is_array())) {
      // fill up array with null values if given idx is outside range
      if (idx >= m_value.array->size()) {
        m_value.array->insert(m_value.array->end(),
                              idx - m_value.array->size() + 1,
                              basic_config());
      }

      return m_value.array->operator[](idx);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  const_reference operator[](size_type idx) const {
    // const operator[] only works for arrays
    if (JSON_LIKELY(is_array())) {
      return m_value.array->operator[](idx);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  reference operator[](const typename map_t::key_type &key) {
    // implicitly convert null value to an empty map
    if (is_null()) {
      m_type = value_t::map;
      m_value.map = create<map_t>();
      assert_invariant();
    }

    // operator[] only works for objects
    if (JSON_LIKELY(is_map())) {
      return m_value.map->operator[](key);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  const_reference operator[](const typename map_t::key_type &key) const {
    // const operator[] only works for objects
    if (JSON_LIKELY(is_map())) {
      assert(m_value.map->find(key) != m_value.map->end());
      return m_value.map->find(key)->second;
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  template<typename T>
  reference operator[](T *key) {
    // implicitly convert null to map
    if (is_null()) {
      m_type = value_t::map;
      m_value = value_t::map;
      assert_invariant();
    }

    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      return m_value.map->operator[](key);
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  template<typename T>
  const_reference operator[](T *key) const {
    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      assert(m_value.map->find(key) != m_value.map->end());
      return m_value.map->find(key)->second;
    }

    JSON_THROW(type_error::create(305, "cannot use operator[] with " + std::string(type_name())));
  }

  template<class ValueType, typename std::enable_if<
      std::is_convertible<basic_config, ValueType>::value, int>::type = 0>
  ValueType value(const typename map_t::key_type &key, const ValueType &default_value) const {
    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      // if key is found, return value and given default value otherwise
      const auto it = find(key);
      if (it != end()) {
        return *it;
      }

      return default_value;
    } else {
      JSON_THROW(type_error::create(306, "cannot use value() with " + std::string(type_name())));
    }
  }

  string_t value(const typename map_t::key_type &key, const char *default_value) const {
    return value(key, string_t(default_value));
  }

  template<class ValueType, typename std::enable_if<
      std::is_convertible<basic_config, ValueType>::value, int>::type = 0>
  ValueType value(const config_path &ptr, const ValueType &default_value) const {
    // at only works for objects
    if (JSON_LIKELY(is_map())) {
      // if pointer resolves a value, return it or use default value
      JSON_TRY {
        return ptr.get_checked(this);
      }
      JSON_CATCH (out_of_range &) {
        return default_value;
      }
    }

    JSON_THROW(type_error::create(306, "cannot use value() with " + std::string(type_name())));
  }

  string_t value(const config_path &ptr, const char *default_value) const {
    return value(ptr, string_t(default_value));
  }

  reference front() {
    return *begin();
  }

  const_reference front() const {
    return *cbegin();
  }

  reference back() {
    auto tmp = end();
    --tmp;
    return *tmp;
  }

  const_reference back() const {
    auto tmp = cend();
    --tmp;
    return *tmp;
  }

  template<class IteratorType, typename std::enable_if<
      std::is_same<IteratorType, typename basic_config::iterator>::value or
          std::is_same<IteratorType, typename basic_config::const_iterator>::value, int>::type
  = 0>
  IteratorType erase(IteratorType pos) {
    // make sure iterator fits the current value
    if (JSON_UNLIKELY(this != pos.m_object)) {
      JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    IteratorType result = end();

    switch (m_type) {
      case value_t::boolean:
      case value_t::number_float:
      case value_t::number_integer:
      case value_t::number_unsigned:
      case value_t::string: {
        if (JSON_UNLIKELY(not pos.m_it.primitive_iterator.is_begin())) {
          JSON_THROW(invalid_iterator::create(205, "iterator out of range"));
        }

        if (is_string()) {
          AllocatorType<string_t> alloc;
          alloc.destroy(m_value.string);
          alloc.deallocate(m_value.string, 1);
          m_value.string = nullptr;
        }

        m_type = value_t::null;
        assert_invariant();
        break;
      }

      case value_t::map: {
        result.m_it.object_iterator = m_value.map->erase(pos.m_it.object_iterator);
        break;
      }

      case value_t::array: {
        result.m_it.array_iterator = m_value.array->erase(pos.m_it.array_iterator);
        break;
      }

      default:JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
    }

    return result;
  }

  template<class IteratorType, typename std::enable_if<
      std::is_same<IteratorType, typename basic_config::iterator>::value or
          std::is_same<IteratorType, typename basic_config::const_iterator>::value, int>::type
  = 0>
  IteratorType erase(IteratorType first, IteratorType last) {
    // make sure iterator fits the current value
    if (JSON_UNLIKELY(this != first.m_object or this != last.m_object)) {
      JSON_THROW(invalid_iterator::create(203, "iterators do not fit current value"));
    }

    IteratorType result = end();

    switch (m_type) {
      case value_t::boolean:
      case value_t::number_float:
      case value_t::number_integer:
      case value_t::number_unsigned:
      case value_t::string: {
        if (JSON_LIKELY(not first.m_it.primitive_iterator.is_begin()
                            or not last.m_it.primitive_iterator.is_end())) {
          JSON_THROW(invalid_iterator::create(204, "iterators out of range"));
        }

        if (is_string()) {
          AllocatorType<string_t> alloc;
          alloc.destroy(m_value.string);
          alloc.deallocate(m_value.string, 1);
          m_value.string = nullptr;
        }

        m_type = value_t::null;
        assert_invariant();
        break;
      }

      case value_t::map: {
        result.m_it.object_iterator = m_value.map->erase(first.m_it.object_iterator,
                                                         last.m_it.object_iterator);
        break;
      }

      case value_t::array: {
        result.m_it.array_iterator = m_value.array->erase(first.m_it.array_iterator,
                                                          last.m_it.array_iterator);
        break;
      }

      default:JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
    }

    return result;
  }

  size_type erase(const typename map_t::key_type &key) {
    // this erase only works for objects
    if (JSON_LIKELY(is_map())) {
      return m_value.map->erase(key);
    }

    JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
  }

  void erase(const size_type idx) {
    // this erase only works for arrays
    if (JSON_LIKELY(is_array())) {
      if (JSON_UNLIKELY(idx >= size())) {
        JSON_THROW(out_of_range::create(401, "array index " + std::to_string(idx) + " is out of range"));
      }

      m_value.array->erase(m_value.array->begin() + static_cast<difference_type>(idx));
    } else {
      JSON_THROW(type_error::create(307, "cannot use erase() with " + std::string(type_name())));
    }
  }

  iterator find(typename map_t::key_type key) {
    auto result = end();

    if (is_map()) {
      result.m_it.object_iterator = m_value.map->find(key);
    }

    return result;
  }

  const_iterator find(typename map_t::key_type key) const {
    auto result = cend();

    if (is_map()) {
      result.m_it.map_iterator = m_value.map->find(key);
    }

    return result;
  }

  size_type count(typename map_t::key_type key) const {
    // return 0 for all nonobject types
    return is_map() ? m_value.map->count(key) : 0;
  }

  iterator begin() noexcept {
    iterator result(this);
    result.set_begin();
    return result;
  }

  const_iterator begin() const noexcept {
    return cbegin();
  }

  const_iterator cbegin() const noexcept {
    const_iterator result(this);
    result.set_begin();
    return result;
  }

  iterator end() noexcept {
    iterator result(this);
    result.set_end();
    return result;
  }

  const_iterator end() const noexcept {
    return cend();
  }

  const_iterator cend() const noexcept {
    const_iterator result(this);
    result.set_end();
    return result;
  }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }

  const_reverse_iterator rbegin() const noexcept {
    return crbegin();
  }

  reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }

  const_reverse_iterator rend() const noexcept {
    return crend();
  }

  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }

  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

 public:
  bool empty() const noexcept {
    switch (m_type) {
      case value_t::null: {
        // null values are empty
        return true;
      }

      case value_t::array: {
        // delegate call to array_t::empty()
        return m_value.array->empty();
      }

      case value_t::map: {
        // delegate call to map_t::empty()
        return m_value.map->empty();
      }

      default: {
        // all other types are nonempty
        return false;
      }
    }
  }

  size_type size() const noexcept {
    switch (m_type) {
      case value_t::null: {
        // null values are empty
        return 0;
      }

      case value_t::array: {
        // delegate call to array_t::size()
        return m_value.array->size();
      }

      case value_t::map: {
        // delegate call to map_t::size()
        return m_value.map->size();
      }

      default: {
        // all other types have size 1
        return 1;
      }
    }
  }

  size_type max_size() const noexcept {
    switch (m_type) {
      case value_t::array: {
        // delegate call to array_t::max_size()
        return m_value.array->max_size();
      }

      case value_t::map: {
        // delegate call to map_t::max_size()
        return m_value.map->max_size();
      }

      default: {
        // all other types have max_size() == size()
        return size();
      }
    }
  }

  void clear() noexcept {
    switch (m_type) {
      case value_t::number_integer: {
        m_value.number_integer = 0;
        break;
      }

      case value_t::number_unsigned: {
        m_value.number_unsigned = 0;
        break;
      }

      case value_t::number_float: {
        m_value.number_float = 0.0;
        break;
      }

      case value_t::boolean: {
        m_value.boolean = false;
        break;
      }

      case value_t::string: {
        m_value.string->clear();
        break;
      }

      case value_t::array: {
        m_value.array->clear();
        break;
      }

      case value_t::map: {
        m_value.map->clear();
        break;
      }

      default:break;
    }
  }

  void push_back(basic_config &&val) {
    // push_back only works for null objects or arrays
    if (JSON_UNLIKELY(not(is_null() or is_array()))) {
      JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
    }

    // transform null map into an array
    if (is_null()) {
      m_type = value_t::array;
      m_value = value_t::array;
      assert_invariant();
    }

    // add element to array (move semantics)
    m_value.array->push_back(std::move(val));
    // invalidate map
    val.m_type = value_t::null;
  }

  reference operator+=(basic_config &&val) {
    push_back(std::move(val));
    return *this;
  }

  void push_back(const basic_config &val) {
    // push_back only works for null objects or arrays
    if (JSON_UNLIKELY(not(is_null() or is_array()))) {
      JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
    }

    // transform null map into an array
    if (is_null()) {
      m_type = value_t::array;
      m_value = value_t::array;
      assert_invariant();
    }

    // add element to array
    m_value.array->push_back(val);
  }

  reference operator+=(const basic_config &val) {
    push_back(val);
    return *this;
  }

  void push_back(const typename map_t::value_type &val) {
    // push_back only works for null objects or objects
    if (JSON_UNLIKELY(not(is_null() or is_map()))) {
      JSON_THROW(type_error::create(308, "cannot use push_back() with " + std::string(type_name())));
    }

    // transform null map into an map
    if (is_null()) {
      m_type = value_t::map;
      m_value = value_t::map;
      assert_invariant();
    }

    // add element to array
    m_value.map->insert(val);
  }

  reference operator+=(const typename map_t::value_type &val) {
    push_back(val);
    return *this;
  }

  void push_back(initializer_list_t init) {
    if (is_map() and init.size() == 2 and (*init.begin())->is_string()) {
      basic_config &&key = init.begin()->moved_or_copied();
      push_back(typename map_t::value_type(
          std::move(key.get_ref<string_t &>()), (init.begin() + 1)->moved_or_copied()));
    } else {
      push_back(basic_config(init));
    }
  }

  reference operator+=(initializer_list_t init) {
    push_back(init);
    return *this;
  }

  template<class... Args>
  void emplace_back(Args &&... args) {
    // emplace_back only works for null objects or arrays
    if (JSON_UNLIKELY(not(is_null() or is_array()))) {
      JSON_THROW(type_error::create(311, "cannot use emplace_back() with " + std::string(type_name())));
    }

    // transform null map into an array
    if (is_null()) {
      m_type = value_t::array;
      m_value = value_t::array;
      assert_invariant();
    }

    // add element to array (perfect forwarding)
    m_value.array->emplace_back(std::forward<Args>(args)...);
  }

  template<class... Args>
  std::pair<iterator, bool> emplace(Args &&... args) {
    // emplace only works for null objects or arrays
    if (JSON_UNLIKELY(not(is_null() or is_map()))) {
      JSON_THROW(type_error::create(311, "cannot use emplace() with " + std::string(type_name())));
    }

    // transform null map into an map
    if (is_null()) {
      m_type = value_t::map;
      m_value = value_t::map;
      assert_invariant();
    }

    // add element to array (perfect forwarding)
    auto res = m_value.map->emplace(std::forward<Args>(args)...);
    // create result iterator and set iterator to the result of emplace
    auto it = begin();
    it.m_it.object_iterator = res.first;

    // return pair of iterator and boolean
    return {it, res.second};
  }

  iterator insert(const_iterator pos, const basic_config &val) {
    // insert only works for arrays
    if (JSON_LIKELY(is_array())) {
      // check if iterator pos fits to this JSON value
      if (JSON_UNLIKELY(pos.m_object != this)) {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
      }

      // insert to array and return iterator
      iterator result(this);
      result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, val);
      return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
  }

  iterator insert(const_iterator pos, basic_config &&val) {
    return insert(pos, val);
  }

  iterator insert(const_iterator pos, size_type cnt, const basic_config &val) {
    // insert only works for arrays
    if (JSON_LIKELY(is_array())) {
      // check if iterator pos fits to this JSON value
      if (JSON_UNLIKELY(pos.m_object != this)) {
        JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
      }

      // insert to array and return iterator
      iterator result(this);
      result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, cnt, val);
      return result;
    }

    JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
  }

  iterator insert(const_iterator pos, const_iterator first, const_iterator last) {
    // insert only works for arrays
    if (JSON_UNLIKELY(not is_array())) {
      JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }

    // check if iterator pos fits to this JSON value
    if (JSON_UNLIKELY(pos.m_object != this)) {
      JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // check if range iterators belong to the same JSON map
    if (JSON_UNLIKELY(first.m_object != last.m_object)) {
      JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    if (JSON_UNLIKELY(first.m_object == this or last.m_object == this)) {
      JSON_THROW(invalid_iterator::create(211, "passed iterators may not belong to container"));
    }

    // insert to array and return iterator
    iterator result(this);
    result.m_it.array_iterator = m_value.array->insert(
        pos.m_it.array_iterator,
        first.m_it.array_iterator,
        last.m_it.array_iterator);
    return result;
  }

  iterator insert(const_iterator pos, initializer_list_t ilist) {
    // insert only works for arrays
    if (JSON_UNLIKELY(not is_array())) {
      JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }

    // check if iterator pos fits to this JSON value
    if (JSON_UNLIKELY(pos.m_object != this)) {
      JSON_THROW(invalid_iterator::create(202, "iterator does not fit current value"));
    }

    // insert to array and return iterator
    iterator result(this);
    result.m_it.array_iterator = m_value.array->insert(pos.m_it.array_iterator, ilist.begin(), ilist.end());
    return result;
  }

  void insert(const_iterator first, const_iterator last) {
    // insert only works for objects
    if (JSON_UNLIKELY(not is_map())) {
      JSON_THROW(type_error::create(309, "cannot use insert() with " + std::string(type_name())));
    }

    // check if range iterators belong to the same JSON map
    if (JSON_UNLIKELY(first.m_object != last.m_object)) {
      JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    // passed iterators must belong to objects
    if (JSON_UNLIKELY(not first.m_object->is_object()
                          or not last.m_object->is_object())) {
      JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
    }

    m_value.map->insert(first.m_it.map_iterator, last.m_it.map_iterator);
  }

  void update(const_reference j) {
    // implicitly convert null value to an empty map
    if (is_null()) {
      m_type = value_t::map;
      m_value.map = create<map_t>();
      assert_invariant();
    }

    if (JSON_UNLIKELY(not is_map())) {
      JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
    }
    if (JSON_UNLIKELY(not j.is_map())) {
      JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(j.type_name())));
    }

    for (auto it = j.begin(); it != j.end(); ++it) {
      m_value.map->operator[](it.key()) = it.value();
    }
  }

  void update(const_iterator first, const_iterator last) {
    // implicitly convert null value to an empty map
    if (is_null()) {
      m_type = value_t::map;
      m_value.map = create<map_t>();
      assert_invariant();
    }

    if (JSON_UNLIKELY(not is_map())) {
      JSON_THROW(type_error::create(312, "cannot use update() with " + std::string(type_name())));
    }

    // check if range iterators belong to the same JSON map
    if (JSON_UNLIKELY(first.m_object != last.m_object)) {
      JSON_THROW(invalid_iterator::create(210, "iterators do not fit"));
    }

    // passed iterators must belong to objects
    if (JSON_UNLIKELY(not first.m_object->is_object()
                          or not first.m_object->is_object())) {
      JSON_THROW(invalid_iterator::create(202, "iterators first and last must point to objects"));
    }

    for (auto it = first; it != last; ++it) {
      m_value.map->operator[](it.key()) = it.value();
    }
  }

  void swap(reference other) noexcept(
  std::is_nothrow_move_constructible<value_t>::value and
      std::is_nothrow_move_assignable<value_t>::value and
      std::is_nothrow_move_constructible<config_value>::value and
      std::is_nothrow_move_assignable<config_value>::value
  ) {
    std::swap(m_type, other.m_type);
    std::swap(m_value, other.m_value);
    assert_invariant();
  }

  void swap(array_t &other) {
    // swap only works for arrays
    if (JSON_LIKELY(is_array())) {
      std::swap(*(m_value.array), other);
    } else {
      JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
    }
  }

  void swap(map_t &other) {
    // swap only works for objects
    if (JSON_LIKELY(is_map())) {
      std::swap(*(m_value.map), other);
    } else {
      JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
    }
  }

  void swap(string_t &other) {
    // swap only works for strings
    if (JSON_LIKELY(is_string())) {
      std::swap(*(m_value.string), other);
    } else {
      JSON_THROW(type_error::create(310, "cannot use swap() with " + std::string(type_name())));
    }
  }

 public:
  friend bool operator==(const_reference lhs, const_reference rhs) noexcept {
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type) {
      switch (lhs_type) {
        case value_t::array:return (*lhs.m_value.array == *rhs.m_value.array);

        case value_t::map:return (*lhs.m_value.map == *rhs.m_value.map);

        case value_t::null:return true;

        case value_t::string:return (*lhs.m_value.string == *rhs.m_value.string);

        case value_t::boolean:return (lhs.m_value.boolean == rhs.m_value.boolean);

        case value_t::number_integer:return (lhs.m_value.number_integer == rhs.m_value.number_integer);

        case value_t::number_unsigned:return (lhs.m_value.number_unsigned == rhs.m_value.number_unsigned);

        case value_t::number_float:return (lhs.m_value.number_float == rhs.m_value.number_float);

        default:return false;
      }
    } else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_float) {
      return (static_cast<number_float_t>(lhs.m_value.number_integer) == rhs.m_value.number_float);
    } else if (lhs_type == value_t::number_float and rhs_type == value_t::number_integer) {
      return (lhs.m_value.number_float == static_cast<number_float_t>(rhs.m_value.number_integer));
    } else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_float) {
      return (static_cast<number_float_t>(lhs.m_value.number_unsigned) == rhs.m_value.number_float);
    } else if (lhs_type == value_t::number_float and rhs_type == value_t::number_unsigned) {
      return (lhs.m_value.number_float == static_cast<number_float_t>(rhs.m_value.number_unsigned));
    } else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_integer) {
      return (static_cast<number_integer_t>(lhs.m_value.number_unsigned) == rhs.m_value.number_integer);
    } else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_unsigned) {
      return (lhs.m_value.number_integer == static_cast<number_integer_t>(rhs.m_value.number_unsigned));
    }

    return false;
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator==(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs == basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator==(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) == rhs);
  }

  friend bool operator!=(const_reference lhs, const_reference rhs) noexcept {
    return not(lhs == rhs);
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator!=(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs != basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator!=(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) != rhs);
  }

  friend bool operator<(const_reference lhs, const_reference rhs) noexcept {
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    if (lhs_type == rhs_type) {
      switch (lhs_type) {
        case value_t::array:return (*lhs.m_value.array) < (*rhs.m_value.array);

        case value_t::map:return *lhs.m_value.map < *rhs.m_value.map;

        case value_t::null:return false;

        case value_t::string:return *lhs.m_value.string < *rhs.m_value.string;

        case value_t::boolean:return lhs.m_value.boolean < rhs.m_value.boolean;

        case value_t::number_integer:return lhs.m_value.number_integer < rhs.m_value.number_integer;

        case value_t::number_unsigned:return lhs.m_value.number_unsigned < rhs.m_value.number_unsigned;

        case value_t::number_float:return lhs.m_value.number_float < rhs.m_value.number_float;

        default:return false;
      }
    } else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_float) {
      return static_cast<number_float_t>(lhs.m_value.number_integer) < rhs.m_value.number_float;
    } else if (lhs_type == value_t::number_float and rhs_type == value_t::number_integer) {
      return lhs.m_value.number_float < static_cast<number_float_t>(rhs.m_value.number_integer);
    } else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_float) {
      return static_cast<number_float_t>(lhs.m_value.number_unsigned) < rhs.m_value.number_float;
    } else if (lhs_type == value_t::number_float and rhs_type == value_t::number_unsigned) {
      return lhs.m_value.number_float < static_cast<number_float_t>(rhs.m_value.number_unsigned);
    } else if (lhs_type == value_t::number_integer and rhs_type == value_t::number_unsigned) {
      return lhs.m_value.number_integer < static_cast<number_integer_t>(rhs.m_value.number_unsigned);
    } else if (lhs_type == value_t::number_unsigned and rhs_type == value_t::number_integer) {
      return static_cast<number_integer_t>(lhs.m_value.number_unsigned) < rhs.m_value.number_integer;
    }

    // We only reach this line if we cannot compare values. In that case,
    // we compare types. Note we have to call the operator explicitly,
    // because MSVC has problems otherwise.
    return operator<(lhs_type, rhs_type);
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator<(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs < basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator<(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) < rhs);
  }

  friend bool operator<=(const_reference lhs, const_reference rhs) noexcept {
    return not(rhs < lhs);
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator<=(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs <= basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator<=(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) <= rhs);
  }

  friend bool operator>(const_reference lhs, const_reference rhs) noexcept {
    return not(lhs <= rhs);
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator>(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs > basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator>(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) > rhs);
  }

  friend bool operator>=(const_reference lhs, const_reference rhs) noexcept {
    return not(lhs < rhs);
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator>=(const_reference lhs, const ScalarType rhs) noexcept {
    return (lhs >= basic_config(rhs));
  }

  template<typename ScalarType, typename std::enable_if<
      std::is_scalar<ScalarType>::value, int>::type = 0>
  friend bool operator>=(const ScalarType lhs, const_reference rhs) noexcept {
    return (basic_config(lhs) >= rhs);
  }

  const char *type_name() const {
    {
      switch (m_type) {
        case value_t::null:return "null";
        case value_t::map:return "map";
        case value_t::array:return "array";
        case value_t::string:return "string";
        case value_t::boolean:return "boolean";
        case value_t::discarded:return "discarded";
        default:return "number";
      }
    }
  }

 private:
  value_t m_type = value_t::null;

  config_value m_value = {};

  reference operator[](const config_path &ptr) {
    return ptr.get_unchecked(this);
  }

  const_reference operator[](const config_path &ptr) const {
    return ptr.get_unchecked(this);
  }

  reference at(const config_path &ptr) {
    return ptr.get_checked(this);
  }

  const_reference at(const config_path &ptr) const {
    return ptr.get_checked(this);
  }

  basic_config flatten() const {
    basic_config result(value_t::map);
    config_path::flatten("", *this, result);
    return result;
  }

  basic_config unflatten() const {
    return config_path::unflatten(*this);
  }
};

using config = basic_config<>;

}

#endif //XCAPTCHA_CAPTCHA_CONFIG_H
