//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.2
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2022 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

/****************************************************************************\
 * Note on documentation: The source files contain links to the online      *
 * documentation of the public API at https://json.nlohmann.me. This URL    *
 * contains the most recent documentation and should also be applicable to  *
 * previous versions; documentation for deprecated functions is not         *
 * removed, but marked deprecated. See "Generate documentation" section in  *
 * file docs/README.md.                                                     *
\****************************************************************************/

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#define INCLUDE_NLOHMANN_JSON_HPP_

#include <algorithm> // all_of, find, for_each
#include <cstddef> // nullptr_t, ptrdiff_t, size_t
#include <functional> // hash, less
#include <initializer_list> // initializer_list
#ifndef JSON_NO_IO
    #include <iosfwd> // istream, ostream
#endif  // JSON_NO_IO
#include <iterator> // random_access_iterator_tag
#include <memory> // unique_ptr
#include <numeric> // accumulate
#include <string> // string, stoi, to_string
#include <utility> // declval, forward, move, pair, swap
#include <vector> // vector

#include <nlohmann/adl_serializer.hpp>
#include <nlohmann/byte_container_with_subtype.hpp>
#include <nlohmann/detail/conversions/from_json.hpp>
#include <nlohmann/detail/conversions/to_json.hpp>
#include <nlohmann/detail/exceptions.hpp>
#include <nlohmann/detail/hash.hpp>
#include <nlohmann/detail/input/binary_reader.hpp>
#include <nlohmann/detail/input/input_adapters.hpp>
#include <nlohmann/detail/input/lexer.hpp>
#include <nlohmann/detail/input/parser.hpp>
#include <nlohmann/detail/iterators/internal_iterator.hpp>
#include <nlohmann/detail/iterators/iter_impl.hpp>
#include <nlohmann/detail/iterators/iteration_proxy.hpp>
#include <nlohmann/detail/iterators/json_reverse_iterator.hpp>
#include <nlohmann/detail/iterators/primitive_iterator.hpp>
#include <nlohmann/detail/json_pointer.hpp>
#include <nlohmann/detail/json_ref.hpp>
#include <nlohmann/detail/macro_scope.hpp>
#include <nlohmann/detail/string_concat.hpp>
#include <nlohmann/detail/string_escape.hpp>
#include <nlohmann/detail/meta/cpp_future.hpp>
#include <nlohmann/detail/meta/type_traits.hpp>
#include <nlohmann/detail/output/binary_writer.hpp>
#include <nlohmann/detail/output/output_adapters.hpp>
#include <nlohmann/detail/output/serializer.hpp>
#include <nlohmann/detail/value_t.hpp>
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/ordered_map.hpp>

#if defined(JSON_HAS_CPP_17)
    #include <any>
    #include <string_view>
#endif

/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
NLOHMANN_JSON_NAMESPACE_BEGIN

/*!
@brief a class to store JSON values

@internal
@invariant The member variables @a m_value and @a m_type have the following
relationship:
- If `m_type == value_t::object`, then `m_value.object != nullptr`.
- If `m_type == value_t::array`, then `m_value.array != nullptr`.
- If `m_type == value_t::string`, then `m_value.string != nullptr`.
The invariants are checked by member function assert_invariant().

@note ObjectType trick from https://stackoverflow.com/a/9860911
@endinternal

@since version 1.0.0

@nosubgrouping
*/
NLOHMANN_BASIC_JSON_TPL_DECLARATION
class basic_json // NOLINT(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
{
  private:
    template<detail::value_t> friend struct detail::external_constructor;

    template<typename>
    friend class ::nlohmann::json_pointer;
    // can be restored when json_pointer backwards compatibility is removed
    // friend ::nlohmann::json_pointer<StringType>;

    template<typename BasicJsonType, typename InputType>
    friend class ::nlohmann::detail::parser;
    friend ::nlohmann::detail::serializer<basic_json>;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::iter_impl;
    template<typename BasicJsonType, typename CharType>
    friend class ::nlohmann::detail::binary_writer;
    template<typename BasicJsonType, typename InputType, typename SAX>
    friend class ::nlohmann::detail::binary_reader;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::json_sax_dom_parser;
    template<typename BasicJsonType>
    friend class ::nlohmann::detail::json_sax_dom_callback_parser;
    friend class ::nlohmann::detail::exception;

    /// workaround type for MSVC
    using basic_json_t = NLOHMANN_BASIC_JSON_TPL;

  JSON_PRIVATE_UNLESS_TESTED:
    // convenience aliases for types residing in namespace detail;
    using lexer = ::nlohmann::detail::lexer_base<basic_json>;

    template<typename InputAdapterType>
    static ::nlohmann::detail::parser<basic_json, InputAdapterType> parser(
        InputAdapterType adapter,
        detail::parser_callback_t<basic_json>cb = nullptr,
        const bool allow_exceptions = true,
        const bool ignore_comments = false
    )
    {
        return ::nlohmann::detail::parser<basic_json, InputAdapterType>(std::move(adapter),
                std::move(cb), allow_exceptions, ignore_comments);
    }

  private:
    using primitive_iterator_t = ::nlohmann::detail::primitive_iterator_t;
    template<typename BasicJsonType>
    using internal_iterator = ::nlohmann::detail::internal_iterator<BasicJsonType>;
    template<typename BasicJsonType>
    using iter_impl = ::nlohmann::detail::iter_impl<BasicJsonType>;
    template<typename Iterator>
    using iteration_proxy = ::nlohmann::detail::iteration_proxy<Iterator>;
    template<typename Base> using json_reverse_iterator = ::nlohmann::detail::json_reverse_iterator<Base>;

    template<typename CharType>
    using output_adapter_t = ::nlohmann::detail::output_adapter_t<CharType>;

    template<typename InputType>
    using binary_reader = ::nlohmann::detail::binary_reader<basic_json, InputType>;
    template<typename CharType> using binary_writer = ::nlohmann::detail::binary_writer<basic_json, CharType>;

  JSON_PRIVATE_UNLESS_TESTED:
    using serializer = ::nlohmann::detail::serializer<basic_json>;

  public:
    using value_t = detail::value_t;
    /// JSON Pointer, see @ref nlohmann::json_pointer
    using json_pointer = ::nlohmann::json_pointer<StringType>;
    template<typename T, typename SFINAE>
    using json_serializer = JSONSerializer<T, SFINAE>;
    /// how to treat decoding errors
    using error_handler_t = detail::error_handler_t;
    /// how to treat CBOR tags
    using cbor_tag_handler_t = detail::cbor_tag_handler_t;
    /// helper type for initializer lists of basic_json values
    using initializer_list_t = std::initializer_list<detail::json_ref<basic_json>>;

    using input_format_t = detail::input_format_t;
    /// SAX interface type, see @ref nlohmann::json_sax
    using json_sax_t = json_sax<basic_json>;

    ////////////////
    // exceptions //
    ////////////////

    /// @name exceptions
    /// Classes to implement user-defined exceptions.
    /// @{

    using exception = detail::exception;
    using parse_error = detail::parse_error;
    using invalid_iterator = detail::invalid_iterator;
    using type_error = detail::type_error;
    using out_of_range = detail::out_of_range;
    using other_error = detail::other_error;

    /// @}


    /////////////////////
    // container types //
    /////////////////////

    /// @name container types
    /// The canonic container types to use @ref basic_json like any other STL
    /// container.
    /// @{

    /// the type of elements in a basic_json container
    using value_type = basic_json;

    /// the type of an element reference
    using reference = value_type&;
    /// the type of an element const reference
    using const_reference = const value_type&;

    /// a type to represent differences between iterators
    using difference_type = std::ptrdiff_t;
    /// a type to represent container sizes
    using size_type = std::size_t;

    /// the allocator type
    using allocator_type = AllocatorType<basic_json>;

    /// the type of an element pointer
    using pointer = typename std::allocator_traits<allocator_type>::pointer;
    /// the type of an element const pointer
    using const_pointer = typename std::allocator_traits<allocator_type>::const_pointer;

    /// an iterator for a basic_json container
    using iterator = iter_impl<basic_json>;
    /// a const iterator for a basic_json container
    using const_iterator = iter_impl<const basic_json>;
    /// a reverse iterator for a basic_json container
    using reverse_iterator = json_reverse_iterator<typename basic_json::iterator>;
    /// a const reverse iterator for a basic_json container
    using const_reverse_iterator = json_reverse_iterator<typename basic_json::const_iterator>;

    /// @}


    /// @brief returns the allocator associated with the container
    /// @sa https://json.nlohmann.me/api/basic_json/get_allocator/
    static allocator_type get_allocator()
    {
        return allocator_type();
    }

    /// @brief returns version information on the library
    /// @sa https://json.nlohmann.me/api/basic_json/meta/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json meta()
    {
        basic_json result;

        result["copyright"] = "(C) 2013-2022 Niels Lohmann";
        result["name"] = "JSON for Modern C++";
        result["url"] = "https://github.com/nlohmann/json";
        result["version"]["string"] =
            detail::concat(std::to_string(NLOHMANN_JSON_VERSION_MAJOR), '.',
                           std::to_string(NLOHMANN_JSON_VERSION_MINOR), '.',
                           std::to_string(NLOHMANN_JSON_VERSION_PATCH));
        result["version"]["major"] = NLOHMANN_JSON_VERSION_MAJOR;
        result["version"]["minor"] = NLOHMANN_JSON_VERSION_MINOR;
        result["version"]["patch"] = NLOHMANN_JSON_VERSION_PATCH;

#ifdef _WIN32
        result["platform"] = "win32";
#elif defined __linux__
        result["platform"] = "linux";
#elif defined __APPLE__
        result["platform"] = "apple";
#elif defined __unix__
        result["platform"] = "unix";
#else
        result["platform"] = "unknown";
#endif

#if defined(__ICC) || defined(__INTEL_COMPILER)
        result["compiler"] = {{"family", "icc"}, {"version", __INTEL_COMPILER}};
#elif defined(__clang__)
        result["compiler"] = {{"family", "clang"}, {"version", __clang_version__}};
#elif defined(__GNUC__) || defined(__GNUG__)
        result["compiler"] = {{"family", "gcc"}, {"version", detail::concat(
                    std::to_string(__GNUC__), '.',
                    std::to_string(__GNUC_MINOR__), '.',
                    std::to_string(__GNUC_PATCHLEVEL__))
            }
        };
#elif defined(__HP_cc) || defined(__HP_aCC)
        result["compiler"] = "hp"
#elif defined(__IBMCPP__)
        result["compiler"] = {{"family", "ilecpp"}, {"version", __IBMCPP__}};
#elif defined(_MSC_VER)
        result["compiler"] = {{"family", "msvc"}, {"version", _MSC_VER}};
#elif defined(__PGI)
        result["compiler"] = {{"family", "pgcpp"}, {"version", __PGI}};
#elif defined(__SUNPRO_CC)
        result["compiler"] = {{"family", "sunpro"}, {"version", __SUNPRO_CC}};
#else
        result["compiler"] = {{"family", "unknown"}, {"version", "unknown"}};
#endif


#if defined(_MSVC_LANG)
        result["compiler"]["c++"] = std::to_string(_MSVC_LANG);
#elif defined(__cplusplus)
        result["compiler"]["c++"] = std::to_string(__cplusplus);
#else
        result["compiler"]["c++"] = "unknown";
#endif
        return result;
    }


    ///////////////////////////
    // JSON value data types //
    ///////////////////////////

    /// @name JSON value data types
    /// The data types to store a JSON value. These types are derived from
    /// the template arguments passed to class @ref basic_json.
    /// @{

    /// @brief default object key comparator type
    /// The actual object key comparator type (@ref object_comparator_t) may be
    /// different.
    /// @sa https://json.nlohmann.me/api/basic_json/default_object_comparator_t/
#if defined(JSON_HAS_CPP_14)
    // use of transparent comparator avoids unnecessary repeated construction of temporaries
    // in functions involving lookup by key with types other than object_t::key_type (aka. StringType)
    using default_object_comparator_t = std::less<>;
#else
    using default_object_comparator_t = std::less<StringType>;
#endif

    /// @brief a type for an object
    /// @sa https://json.nlohmann.me/api/basic_json/object_t/
    using object_t = ObjectType<StringType,
          basic_json,
          default_object_comparator_t,
          AllocatorType<std::pair<const StringType,
          basic_json>>>;

    /// @brief a type for an array
    /// @sa https://json.nlohmann.me/api/basic_json/array_t/
    using array_t = ArrayType<basic_json, AllocatorType<basic_json>>;

    /// @brief a type for a string
    /// @sa https://json.nlohmann.me/api/basic_json/string_t/
    using string_t = StringType;

    /// @brief a type for a boolean
    /// @sa https://json.nlohmann.me/api/basic_json/boolean_t/
    using boolean_t = BooleanType;

    /// @brief a type for a number (integer)
    /// @sa https://json.nlohmann.me/api/basic_json/number_integer_t/
    using number_integer_t = NumberIntegerType;

    /// @brief a type for a number (unsigned)
    /// @sa https://json.nlohmann.me/api/basic_json/number_unsigned_t/
    using number_unsigned_t = NumberUnsignedType;

    /// @brief a type for a number (floating-point)
    /// @sa https://json.nlohmann.me/api/basic_json/number_float_t/
    using number_float_t = NumberFloatType;

    /// @brief a type for a packed binary type
    /// @sa https://json.nlohmann.me/api/basic_json/binary_t/
    using binary_t = nlohmann::byte_container_with_subtype<BinaryType>;

    /// @brief object key comparator type
    /// @sa https://json.nlohmann.me/api/basic_json/object_comparator_t/
    using object_comparator_t = detail::actual_object_comparator_t<basic_json>;

    /// @}

  private:

    /// helper for exception-safe object creation
    template<typename T, typename... Args>
    JSON_HEDLEY_RETURNS_NON_NULL
    static T* create(Args&& ... args)
    {
        AllocatorType<T> alloc;
        using AllocatorTraits = std::allocator_traits<AllocatorType<T>>;

        auto deleter = [&](T * obj)
        {
            AllocatorTraits::deallocate(alloc, obj, 1);
        };
        std::unique_ptr<T, decltype(deleter)> obj(AllocatorTraits::allocate(alloc, 1), deleter);
        AllocatorTraits::construct(alloc, obj.get(), std::forward<Args>(args)...);
        JSON_ASSERT(obj != nullptr);
        return obj.release();
    }

    ////////////////////////
    // JSON value storage //
    ////////////////////////

  JSON_PRIVATE_UNLESS_TESTED:
    /*!
    @brief a JSON value

    The actual storage for a JSON value of the @ref basic_json class. This
    union combines the different storage types for the JSON value types
    defined in @ref value_t.

    JSON type | value_t type    | used type
    --------- | --------------- | ------------------------
    object    | object          | pointer to @ref object_t
    array     | array           | pointer to @ref array_t
    string    | string          | pointer to @ref string_t
    boolean   | boolean         | @ref boolean_t
    number    | number_integer  | @ref number_integer_t
    number    | number_unsigned | @ref number_unsigned_t
    number    | number_float    | @ref number_float_t
    binary    | binary          | pointer to @ref binary_t
    null      | null            | *no value is stored*

    @note Variable-length types (objects, arrays, and strings) are stored as
    pointers. The size of the union should not exceed 64 bits if the default
    value types are used.

    @since version 1.0.0
    */
    union json_value
    {
        /// object (stored with pointer to save storage)
        object_t* object;
        /// array (stored with pointer to save storage)
        array_t* array;
        /// string (stored with pointer to save storage)
        string_t* string;
        /// binary (stored with pointer to save storage)
        binary_t* binary;
        /// boolean
        boolean_t boolean;
        /// number (integer)
        number_integer_t number_integer;
        /// number (unsigned integer)
        number_unsigned_t number_unsigned;
        /// number (floating-point)
        number_float_t number_float;

        /// default constructor (for null values)
        json_value() = default;
        /// constructor for booleans
        json_value(boolean_t v) noexcept : boolean(v) {}
        /// constructor for numbers (integer)
        json_value(number_integer_t v) noexcept : number_integer(v) {}
        /// constructor for numbers (unsigned)
        json_value(number_unsigned_t v) noexcept : number_unsigned(v) {}
        /// constructor for numbers (floating-point)
        json_value(number_float_t v) noexcept : number_float(v) {}
        /// constructor for empty values of a given type
        json_value(value_t t)
        {
            switch (t)
            {
                case value_t::object:
                {
                    object = create<object_t>();
                    break;
                }

                case value_t::array:
                {
                    array = create<array_t>();
                    break;
                }

                case value_t::string:
                {
                    string = create<string_t>("");
                    break;
                }

                case value_t::binary:
                {
                    binary = create<binary_t>();
                    break;
                }

                case value_t::boolean:
                {
                    boolean = static_cast<boolean_t>(false);
                    break;
                }

                case value_t::number_integer:
                {
                    number_integer = static_cast<number_integer_t>(0);
                    break;
                }

                case value_t::number_unsigned:
                {
                    number_unsigned = static_cast<number_unsigned_t>(0);
                    break;
                }

                case value_t::number_float:
                {
                    number_float = static_cast<number_float_t>(0.0);
                    break;
                }

                case value_t::null:
                {
                    object = nullptr;  // silence warning, see #821
                    break;
                }

                case value_t::discarded:
                default:
                {
                    object = nullptr;  // silence warning, see #821
                    if (JSON_HEDLEY_UNLIKELY(t == value_t::null))
                    {
                        JSON_THROW(other_error::create(500, "961c151d2e87f2686a955a9be24d316f1362bf21 3.11.2", nullptr)); // LCOV_EXCL_LINE
                    }
                    break;
                }
            }
        }

        /// constructor for strings
        json_value(const string_t& value) : string(create<string_t>(value)) {}

        /// constructor for rvalue strings
        json_value(string_t&& value) : string(create<string_t>(std::move(value))) {}

        /// constructor for objects
        json_value(const object_t& value) : object(create<object_t>(value)) {}

        /// constructor for rvalue objects
        json_value(object_t&& value) : object(create<object_t>(std::move(value))) {}

        /// constructor for arrays
        json_value(const array_t& value) : array(create<array_t>(value)) {}

        /// constructor for rvalue arrays
        json_value(array_t&& value) : array(create<array_t>(std::move(value))) {}

        /// constructor for binary arrays
        json_value(const typename binary_t::container_type& value) : binary(create<binary_t>(value)) {}

        /// constructor for rvalue binary arrays
        json_value(typename binary_t::container_type&& value) : binary(create<binary_t>(std::move(value))) {}

        /// constructor for binary arrays (internal type)
        json_value(const binary_t& value) : binary(create<binary_t>(value)) {}

        /// constructor for rvalue binary arrays (internal type)
        json_value(binary_t&& value) : binary(create<binary_t>(std::move(value))) {}

        void destroy(value_t t)
        {
            if (t == value_t::array || t == value_t::object)
            {
                // flatten the current json_value to a heap-allocated stack
                std::vector<basic_json> stack;

                // move the top-level items to stack
                if (t == value_t::array)
                {
                    stack.reserve(array->size());
                    std::move(array->begin(), array->end(), std::back_inserter(stack));
                }
                else
                {
                    stack.reserve(object->size());
                    for (auto&& it : *object)
                    {
                        stack.push_back(std::move(it.second));
                    }
                }

                while (!stack.empty())
                {
                    // move the last item to local variable to be processed
                    basic_json current_item(std::move(stack.back()));
                    stack.pop_back();

                    // if current_item is array/object, move
                    // its children to the stack to be processed later
                    if (current_item.is_array())
                    {
                        std::move(current_item.m_value.array->begin(), current_item.m_value.array->end(), std::back_inserter(stack));

                        current_item.m_value.array->clear();
                    }
                    else if (current_item.is_object())
                    {
                        for (auto&& it : *current_item.m_value.object)
                        {
                            stack.push_back(std::move(it.second));
                        }

                        current_item.m_value.object->clear();
                    }

                    // it's now safe that current_item get destructed
                    // since it doesn't have any children
                }
            }

            switch (t)
            {
                case value_t::object:
                {
                    AllocatorType<object_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, object);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, object, 1);
                    break;
                }

                case value_t::array:
                {
                    AllocatorType<array_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, array);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, array, 1);
                    break;
                }

                case value_t::string:
                {
                    AllocatorType<string_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, string);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, string, 1);
                    break;
                }

                case value_t::binary:
                {
                    AllocatorType<binary_t> alloc;
                    std::allocator_traits<decltype(alloc)>::destroy(alloc, binary);
                    std::allocator_traits<decltype(alloc)>::deallocate(alloc, binary, 1);
                    break;
                }

                case value_t::null:
                case value_t::boolean:
                case value_t::number_integer:
                case value_t::number_unsigned:
                case value_t::number_float:
                case value_t::discarded:
                default:
                {
                    break;
                }
            }
        }
    };

  private:
    /*!
    @brief checks the class invariants

    This function asserts the class invariants. It needs to be called at the
    end of every constructor to make sure that created objects respect the
    invariant. Furthermore, it has to be called each time the type of a JSON
    value is changed, because the invariant expresses a relationship between
    @a m_type and @a m_value.

    Furthermore, the parent relation is checked for arrays and objects: If
    @a check_parents true and the value is an array or object, then the
    container's elements must have the current value as parent.

    @param[in] check_parents  whether the parent relation should be checked.
               The value is true by default and should only be set to false
               during destruction of objects when the invariant does not
               need to hold.
    */
    void assert_invariant(bool check_parents = true) const noexcept
    {
        JSON_ASSERT(m_type != value_t::object || m_value.object != nullptr);
        JSON_ASSERT(m_type != value_t::array || m_value.array != nullptr);
        JSON_ASSERT(m_type != value_t::string || m_value.string != nullptr);
        JSON_ASSERT(m_type != value_t::binary || m_value.binary != nullptr);

#if JSON_DIAGNOSTICS
        JSON_TRY
        {
            // cppcheck-suppress assertWithSideEffect
            JSON_ASSERT(!check_parents || !is_structured() || std::all_of(begin(), end(), [this](const basic_json & j)
            {
                return j.m_parent == this;
            }));
        }
        JSON_CATCH(...) {} // LCOV_EXCL_LINE
#endif
        static_cast<void>(check_parents);
    }

    void set_parents()
    {
#if JSON_DIAGNOSTICS
        switch (m_type)
        {
            case value_t::array:
            {
                for (auto& element : *m_value.array)
                {
                    element.m_parent = this;
                }
                break;
            }

            case value_t::object:
            {
                for (auto& element : *m_value.object)
                {
                    element.second.m_parent = this;
                }
                break;
            }

            case value_t::null:
            case value_t::string:
            case value_t::boolean:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::number_float:
            case value_t::binary:
            case value_t::discarded:
            default:
                break;
        }
#endif
    }

    iterator set_parents(iterator it, typename iterator::difference_type count_set_parents)
    {
#if JSON_DIAGNOSTICS
        for (typename iterator::difference_type i = 0; i < count_set_parents; ++i)
        {
            (it + i)->m_parent = this;
        }
#else
        static_cast<void>(count_set_parents);
#endif
        return it;
    }

    reference set_parent(reference j, std::size_t old_capacity = static_cast<std::size_t>(-1))
    {
#if JSON_DIAGNOSTICS
        if (old_capacity != static_cast<std::size_t>(-1))
        {
            // see https://github.com/nlohmann/json/issues/2838
            JSON_ASSERT(type() == value_t::array);
            if (JSON_HEDLEY_UNLIKELY(m_value.array->capacity() != old_capacity))
            {
                // capacity has changed: update all parents
                set_parents();
                return j;
            }
        }

        // ordered_json uses a vector internally, so pointers could have
        // been invalidated; see https://github.com/nlohmann/json/issues/2962
#ifdef JSON_HEDLEY_MSVC_VERSION
#pragma warning(push )
#pragma warning(disable : 4127) // ignore warning to replace if with if constexpr
#endif
        if (detail::is_ordered_map<object_t>::value)
        {
            set_parents();
            return j;
        }
#ifdef JSON_HEDLEY_MSVC_VERSION
#pragma warning( pop )
#endif

        j.m_parent = this;
#else
        static_cast<void>(j);
        static_cast<void>(old_capacity);
#endif
        return j;
    }

  public:
    //////////////////////////
    // JSON parser callback //
    //////////////////////////

    /// @brief parser event types
    /// @sa https://json.nlohmann.me/api/basic_json/parse_event_t/
    using parse_event_t = detail::parse_event_t;

    /// @brief per-element parser callback type
    /// @sa https://json.nlohmann.me/api/basic_json/parser_callback_t/
    using parser_callback_t = detail::parser_callback_t<basic_json>;

    //////////////////
    // constructors //
    //////////////////

    /// @name constructors and destructors
    /// Constructors of class @ref basic_json, copy/move constructor, copy
    /// assignment, static functions creating objects, and the destructor.
    /// @{

    /// @brief create an empty value with a given type
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(const value_t v)
        : m_type(v), m_value(v)
    {
        assert_invariant();
    }

    /// @brief create a null object
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(std::nullptr_t = nullptr) noexcept // NOLINT(bugprone-exception-escape)
        : basic_json(value_t::null)
    {
        assert_invariant();
    }

    /// @brief create a JSON value from compatible types
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    template < typename CompatibleType,
               typename U = detail::uncvref_t<CompatibleType>,
               detail::enable_if_t <
                   !detail::is_basic_json<U>::value && detail::is_compatible_type<basic_json_t, U>::value, int > = 0 >
    basic_json(CompatibleType && val) noexcept(noexcept( // NOLINT(bugprone-forwarding-reference-overload,bugprone-exception-escape)
                JSONSerializer<U>::to_json(std::declval<basic_json_t&>(),
                                           std::forward<CompatibleType>(val))))
    {
        JSONSerializer<U>::to_json(*this, std::forward<CompatibleType>(val));
        set_parents();
        assert_invariant();
    }

    /// @brief create a JSON value from an existing one
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    template < typename BasicJsonType,
               detail::enable_if_t <
                   detail::is_basic_json<BasicJsonType>::value&& !std::is_same<basic_json, BasicJsonType>::value, int > = 0 >
    basic_json(const BasicJsonType& val)
    {
        using other_boolean_t = typename BasicJsonType::boolean_t;
        using other_number_float_t = typename BasicJsonType::number_float_t;
        using other_number_integer_t = typename BasicJsonType::number_integer_t;
        using other_number_unsigned_t = typename BasicJsonType::number_unsigned_t;
        using other_string_t = typename BasicJsonType::string_t;
        using other_object_t = typename BasicJsonType::object_t;
        using other_array_t = typename BasicJsonType::array_t;
        using other_binary_t = typename BasicJsonType::binary_t;

        switch (val.type())
        {
            case value_t::boolean:
                JSONSerializer<other_boolean_t>::to_json(*this, val.template get<other_boolean_t>());
                break;
            case value_t::number_float:
                JSONSerializer<other_number_float_t>::to_json(*this, val.template get<other_number_float_t>());
                break;
            case value_t::number_integer:
                JSONSerializer<other_number_integer_t>::to_json(*this, val.template get<other_number_integer_t>());
                break;
            case value_t::number_unsigned:
                JSONSerializer<other_number_unsigned_t>::to_json(*this, val.template get<other_number_unsigned_t>());
                break;
            case value_t::string:
                JSONSerializer<other_string_t>::to_json(*this, val.template get_ref<const other_string_t&>());
                break;
            case value_t::object:
                JSONSerializer<other_object_t>::to_json(*this, val.template get_ref<const other_object_t&>());
                break;
            case value_t::array:
                JSONSerializer<other_array_t>::to_json(*this, val.template get_ref<const other_array_t&>());
                break;
            case value_t::binary:
                JSONSerializer<other_binary_t>::to_json(*this, val.template get_ref<const other_binary_t&>());
                break;
            case value_t::null:
                *this = nullptr;
                break;
            case value_t::discarded:
                m_type = value_t::discarded;
                break;
            default:            // LCOV_EXCL_LINE
                JSON_ASSERT(false); // NOLINT(cert-dcl03-c,hicpp-static-assert,misc-static-assert) LCOV_EXCL_LINE
        }
        JSON_ASSERT(m_type == val.type());
        set_parents();
        assert_invariant();
    }

    /// @brief create a container (array or object) from an initializer list
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(initializer_list_t init,
               bool type_deduction = true,
               value_t manual_type = value_t::array)
    {
        // check if each element is an array with two elements whose first
        // element is a string
        bool is_an_object = std::all_of(init.begin(), init.end(),
                                        [](const detail::json_ref<basic_json>& element_ref)
        {
            return element_ref->is_array() && element_ref->size() == 2 && (*element_ref)[0].is_string();
        });

        // adjust type if type deduction is not wanted
        if (!type_deduction)
        {
            // if array is wanted, do not create an object though possible
            if (manual_type == value_t::array)
            {
                is_an_object = false;
            }

            // if object is wanted but impossible, throw an exception
            if (JSON_HEDLEY_UNLIKELY(manual_type == value_t::object && !is_an_object))
            {
                JSON_THROW(type_error::create(301, "cannot create object from initializer list", nullptr));
            }
        }

        if (is_an_object)
        {
            // the initializer list is a list of pairs -> create object
            m_type = value_t::object;
            m_value = value_t::object;

            for (auto& element_ref : init)
            {
                auto element = element_ref.moved_or_copied();
                m_value.object->emplace(
                    std::move(*((*element.m_value.array)[0].m_value.string)),
                    std::move((*element.m_value.array)[1]));
            }
        }
        else
        {
            // the initializer list describes an array -> create array
            m_type = value_t::array;
            m_value.array = create<array_t>(init.begin(), init.end());
        }

        set_parents();
        assert_invariant();
    }

    /// @brief explicitly create a binary array (without subtype)
    /// @sa https://json.nlohmann.me/api/basic_json/binary/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json binary(const typename binary_t::container_type& init)
    {
        auto res = basic_json();
        res.m_type = value_t::binary;
        res.m_value = init;
        return res;
    }

    /// @brief explicitly create a binary array (with subtype)
    /// @sa https://json.nlohmann.me/api/basic_json/binary/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json binary(const typename binary_t::container_type& init, typename binary_t::subtype_type subtype)
    {
        auto res = basic_json();
        res.m_type = value_t::binary;
        res.m_value = binary_t(init, subtype);
        return res;
    }

    /// @brief explicitly create a binary array
    /// @sa https://json.nlohmann.me/api/basic_json/binary/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json binary(typename binary_t::container_type&& init)
    {
        auto res = basic_json();
        res.m_type = value_t::binary;
        res.m_value = std::move(init);
        return res;
    }

    /// @brief explicitly create a binary array (with subtype)
    /// @sa https://json.nlohmann.me/api/basic_json/binary/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json binary(typename binary_t::container_type&& init, typename binary_t::subtype_type subtype)
    {
        auto res = basic_json();
        res.m_type = value_t::binary;
        res.m_value = binary_t(std::move(init), subtype);
        return res;
    }

    /// @brief explicitly create an array from an initializer list
    /// @sa https://json.nlohmann.me/api/basic_json/array/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json array(initializer_list_t init = {})
    {
        return basic_json(init, false, value_t::array);
    }

    /// @brief explicitly create an object from an initializer list
    /// @sa https://json.nlohmann.me/api/basic_json/object/
    JSON_HEDLEY_WARN_UNUSED_RESULT
    static basic_json object(initializer_list_t init = {})
    {
        return basic_json(init, false, value_t::object);
    }

    /// @brief construct an array with count copies of given value
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(size_type cnt, const basic_json& val)
        : m_type(value_t::array)
    {
        m_value.array = create<array_t>(cnt, val);
        set_parents();
        assert_invariant();
    }

    /// @brief construct a JSON container given an iterator range
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    template < class InputIT, typename std::enable_if <
                   std::is_same<InputIT, typename basic_json_t::iterator>::value ||
                   std::is_same<InputIT, typename basic_json_t::const_iterator>::value, int >::type = 0 >
    basic_json(InputIT first, InputIT last)
    {
        JSON_ASSERT(first.m_object != nullptr);
        JSON_ASSERT(last.m_object != nullptr);

        // make sure iterator fits the current value
        if (JSON_HEDLEY_UNLIKELY(first.m_object != last.m_object))
        {
            JSON_THROW(invalid_iterator::create(201, "iterators are not compatible", nullptr));
        }

        // copy type from first iterator
        m_type = first.m_object->m_type;

        // check if iterator range is complete for primitive values
        switch (m_type)
        {
            case value_t::boolean:
            case value_t::number_float:
            case value_t::number_integer:
            case value_t::number_unsigned:
            case value_t::string:
            {
                if (JSON_HEDLEY_UNLIKELY(!first.m_it.primitive_iterator.is_begin()
                                         || !last.m_it.primitive_iterator.is_end()))
                {
                    JSON_THROW(invalid_iterator::create(204, "iterators out of range", first.m_object));
                }
                break;
            }

            case value_t::null:
            case value_t::object:
            case value_t::array:
            case value_t::binary:
            case value_t::discarded:
            default:
                break;
        }

        switch (m_type)
        {
            case value_t::number_integer:
            {
                m_value.number_integer = first.m_object->m_value.number_integer;
                break;
            }

            case value_t::number_unsigned:
            {
                m_value.number_unsigned = first.m_object->m_value.number_unsigned;
                break;
            }

            case value_t::number_float:
            {
                m_value.number_float = first.m_object->m_value.number_float;
                break;
            }

            case value_t::boolean:
            {
                m_value.boolean = first.m_object->m_value.boolean;
                break;
            }

            case value_t::string:
            {
                m_value = *first.m_object->m_value.string;
                break;
            }

            case value_t::object:
            {
                m_value.object = create<object_t>(first.m_it.object_iterator,
                                                  last.m_it.object_iterator);
                break;
            }

            case value_t::array:
            {
                m_value.array = create<array_t>(first.m_it.array_iterator,
                                                last.m_it.array_iterator);
                break;
            }

            case value_t::binary:
            {
                m_value = *first.m_object->m_value.binary;
                break;
            }

            case value_t::null:
            case value_t::discarded:
            default:
                JSON_THROW(invalid_iterator::create(206, detail::concat("cannot construct with iterators from ", first.m_object->type_name()), first.m_object));
        }

        set_parents();
        assert_invariant();
    }


    ///////////////////////////////////////
    // other constructors and destructor //
    ///////////////////////////////////////

    template<typename JsonRef,
             detail::enable_if_t<detail::conjunction<detail::is_json_ref<JsonRef>,
                                 std::is_same<typename JsonRef::value_type, basic_json>>::value, int> = 0 >
    basic_json(const JsonRef& ref) : basic_json(ref.moved_or_copied()) {}

    /// @brief copy constructor
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(const basic_json& other)
        : m_type(other.m_type)
    {
        // check of passed value is valid
        other.assert_invariant();

        switch (m_type)
        {
            case value_t::object:
            {
                m_value = *other.m_value.object;
                break;
            }

            case value_t::array:
            {
                m_value = *other.m_value.array;
                break;
            }

            case value_t::string:
            {
                m_value = *other.m_value.string;
                break;
            }

            case value_t::boolean:
            {
                m_value = other.m_value.boolean;
                break;
            }

            case value_t::number_integer:
            {
                m_value = other.m_value.number_integer;
                break;
            }

            case value_t::number_unsigned:
            {
                m_value = other.m_value.number_unsigned;
                break;
            }

            case value_t::number_float:
            {
                m_value = other.m_value.number_float;
                break;
            }

            case value_t::binary:
            {
                m_value = *other.m_value.binary;
                break;
            }

            case value_t::null:
            case value_t::discarded:
            default:
                break;
        }

        set_parents();
        assert_invariant();
    }

    /// @brief move constructor
    /// @sa https://json.nlohmann.me/api/basic_json/basic_json/
    basic_json(basic_json&& other) noexcept
        : m_type(std::move(other.m_type)),
          m_value(std::move(other.m_value))
    {
        // check that passed value is valid
        other.assert_invariant(false);

        // invalidate payload
        other.m_type = value_t::null;
        other.m_value = {};

        set_parents();
        assert_invariant();
    }

    /// @brief copy assignment
    /// @sa https://json.nlohmann.me/api/basic_json/operator=/
    basic_json& operator=(basic_json other) noexcept (
        std::is_nothrow_move_constructible<value_t>::value&&
        std::is_nothrow_move_assignable<value_t>::value&&
        std::is_nothrow_move_constructible<json_value>::value&&
        std::is_nothrow_move_assignable<json_value>::value
    )
    {
        // check that passed value is valid
        other.assert_invariant();

        using std::swap;
        swap(m_type, other.m_type);
        swap(m_value, other.m_value);

        set_parents();
        assert_invariant();
        return *this;
    }

    /// @brief destructor
    /// @sa https://json.nlohmann.me/api/basic_json/~basic_json/
    ~basic_json() noexcept
    {
        assert_invariant(false);
        m_value.destroy(m_type);
    }

    /// @}

  public:
    ///////////////////////
    // object inspection //
    ///////////////////////

    /// @name object inspection
    /// Functions to inspect the type of a JSON value.
    /// @{

    /// @brief serialization
    /// @sa https://json.nlohmann.me/api/basic_json/dump/
    string_t dump(const int indent = -1,
                  const char indent_char = ' ',
                  const bool ensure_ascii = false,
                  const error_handler_t error_handler = error_handler_t::strict) const
    {
        string_t result;
        serializer s(detail::output_adapter<char, string_t>(result), indent_char, error_handler);

        if (indent >= 0)
        {
            s.dump(*this, true, ensure_ascii, static_cast<unsigned int>(indent));
        }
        else
        {
            s.dump(*this, false, ensure_ascii, 0);
        }

        return result;
    }

    /// @brief return the type of the JSON value (explicit)
    /// @sa https://json.nlohmann.me/api/basic_json/type/
    constexpr value_t type() const noexcept
    {
        return m_type;
    }

    /// @brief return whether type is primitive
    /// @sa https://json.nlohmann.me/api/basic_json/is_primitive/
    constexpr bool is_primitive() const noexcept
    {
        return is_null() || is_string() || is_boolean() || is_number() || is_binary();
    }

    /// @brief return whether type is structured
    /// @sa https://json.nlohmann.me/api/basic_json/is_structured/
    constexpr bool is_structured() const noexcept
    {
        return is_array() || is_object();
    }

    /// @brief return whether value is null
    /// @sa https://json.nlohmann.me/api/basic_json/is_null/
    constexpr bool is_null() const noexcept
    {
        return m_type == value_t::null;
    }

    /// @brief return whether value is a boolean
    /// @sa https://json.nlohmann.me/api/basic_json/is_boolean/
    constexpr bool is_boolean() const noexcept
    {
        return m_type == value_t::boolean;
    }

    /// @brief return whether value is a number
    /// @sa https://json.nlohmann.me/api/basic_json/is_number/
    constexpr bool is_number() const noexcept
    {
        return is_number_integer() || is_number_float();
    }

    /// @brief return whether value is an integer number
    /// @sa https://json.nlohmann.me/api/basic_json/is_number_integer/
    constexpr bool is_number_integer() const noexcept
    {
        return m_type == value_t::number_integer || m_type == value_t::number_unsigned;
    }

    /// @brief return whether value is an unsigned integer number
    /// @sa https://json.nlohmann.me/api/basic_json/is_number_unsigned/
    constexpr bool is_number_unsigned() const noexcept
    {
        return m_type == value_t::number_unsigned;
    }

    /// @brief return whether value is a floating-point number
    /// @sa https://json.nlohmann.me/api/basic_json/is_number_float/
    constexpr bool is_number_float() const noexcept
    {
        return m_type == value_t::number_float;
    }

    /// @brief return whether value is an object
    /// @sa https://json.nlohmann.me/api/basic_json/is_object/
    constexpr bool is_object() const noexcept
    {
        return m_type == value_t::object;
    }

    /// @brief return whether value is an array
    /// @sa https://json.nlohmann.me/api/basic_json/is_array/
    constexpr bool is_array() const noexcept
    {
        return m_type == value_t::array;
    }

    /// @brief return whether value is a string
    /// @sa https://json.nlohmann.me/api/basic_json/is_string/
    constexpr bool is_string() const noexcept
    {
        return m_type == value_t::string;
    }

    /// @brief return whether value is a binary array
    /// @sa https://json.nlohmann.me/api/basic_json/is_binary/
    constexpr bool is_binary() const noexcept
    {
        return m_type == value_t::binary;
    }

    /// @brief return whether value is discarded
    /// @sa https://json.nlohmann.me/api/basic_json/is_discarded/
    constexpr bool is_discarded() const noexcept
    {
        return m_type == value_t::discarded;
    }

    /// @brief return the type of the JSON value (implicit)
    /// @sa https://json.nlohmann.me/api/basic_json/operator_value_t/
    constexpr operator value_t() const noexcept
    {
        return m_type;
    }

    /// @}

  private:
    //////////////////
    // value access //
    //////////////////

    /// get a boolean (explicit)
    boolean_t get_impl(boolean_t* /*unused*/) const
    {
        if (JSON_HEDLEY_LIKELY(is_boolean()))
        {
            return m_value.boolean;
        }

        JSON_THROW(type_error::create(302, detail::concat("type must be boolean, but is ", type_name()), this));
    }

#endif  // INCLUDE_NLOHMANN_JSON_HPP_
