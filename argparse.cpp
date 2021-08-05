// Argument parsing library
// License: UNLICENSE <https://www.unlicense.org>
// Website: https://github.com/ii14/argparse

#include "argparse.hpp"

#include <cstring>
#include <cassert>
#include <stdexcept>
#include <sstream>
#include <atomic>

namespace argparse {

namespace detail {

enum class opt_type : std::uint8_t
{
    param,
    flag,
    category,
};

struct opt_base::opt_impl
{
    std::atomic<std::size_t> ref {1};
    opt_type type;
    char shortname {0};
    const char* longname {nullptr};
    const char* desc {nullptr};
    const char* value {nullptr};
};

opt_base::opt_base(const opt_base& b)
    : _ptr{b._ptr}
{
    if (_ptr != nullptr)
        ++_ptr->ref;
}

opt_base& opt_base::operator=(const opt_base& b)
{
    if (_ptr != nullptr && --_ptr->ref == 0)
        delete _ptr;
    _ptr = b._ptr;
    if (_ptr != nullptr)
        ++_ptr->ref;
    return *this;
}

opt_base::opt_base(opt_base&& b) noexcept
    : _ptr{b._ptr}
{
    b._ptr = nullptr;
}

opt_base& opt_base::operator=(opt_base&& b) noexcept
{
    if (_ptr != nullptr && --_ptr->ref == 0)
        delete _ptr;
    _ptr = b._ptr;
    b._ptr = nullptr;
    return *this;
}

opt_base::~opt_base()
{
    if (_ptr != nullptr) {
        if (--_ptr->ref == 0)
            delete _ptr;
        _ptr = nullptr;
    }
}

char opt_base::shortname() const noexcept
{
    return _ptr != nullptr ? _ptr->shortname : 0;
}

const char* opt_base::longname() const noexcept
{
    return _ptr != nullptr && _ptr->longname != nullptr ? _ptr->longname : nullptr;
}

const char* opt_base::description() const noexcept
{
    return _ptr != nullptr && _ptr->desc != nullptr ? _ptr->desc : nullptr;
}

bool opt_base::is_set() const noexcept
{
    return _ptr != nullptr && _ptr->value != nullptr;
}

const char* param_t::value(const char* fallback) const noexcept
{
    return _ptr == nullptr || _ptr->value == nullptr ? fallback : _ptr->value;
}

} // namespace detail

std::string error::str() const
{
    switch (_type) {
    case ok:
        return "ok";
    case unknown_option: {
        std::stringstream s;
        s << "unknown option '" << optname() << "'";
        return s.str();
    }
    case missing_argument: {
        std::stringstream s;
        s << "option '" << optname() << "' requires an argument";
        return s.str();
    }
    }
    assert(0 && "unexpected error type");
}

void parser::_remove_duplicates(const names_t& names)
{
    if (names.shortname != 0) {
        for (auto it = _opts.begin(); it != _opts.end(); ++it) {
            if (it->_ptr == nullptr)
                continue;
            if (it->_ptr->type == detail::opt_type::category)
                continue;
            if (it->_ptr->shortname == names.shortname) {
                if (it->_ptr->longname == nullptr) {
                    _opts.erase(it);
                } else {
                    it->_ptr->shortname = 0;
                }
                break;
            }
        }
    }

    if (names.longname != nullptr) {
        for (auto it = _opts.begin(); it != _opts.end(); ++it) {
            if (it->_ptr == nullptr || it->_ptr->longname == nullptr)
                continue;
            if (it->_ptr->type == detail::opt_type::category)
                continue;
            if (std::strcmp(it->_ptr->longname, names.longname) == 0) {
                if (it->_ptr->shortname == 0) {
                    _opts.erase(it);
                } else {
                    it->_ptr->longname = nullptr;
                }
                break;
            }
        }
    }
}

parser::flag_t parser::flag(names_t names, const char* desc)
{
    assert(names.shortname != 0 || names.longname != nullptr);
    if (names.shortname != 0) {
        assert((names.shortname >= '0' && names.shortname <= '9')
            || (names.shortname >= 'a' && names.shortname <= 'z')
            || (names.shortname >= 'A' && names.shortname <= 'Z'));
    }
    _remove_duplicates(names);

    flag_t o;
    o._ptr = new detail::opt_base::opt_impl;
    o._ptr->type = detail::opt_type::flag;
    o._ptr->shortname = names.shortname;
    o._ptr->longname = names.longname;
    o._ptr->desc = desc;
    _opts.push_back(o);
    return o;
}

parser::param_t parser::param(names_t names, const char* desc)
{
    assert(names.shortname != 0 || names.longname != nullptr);
    if (names.shortname != 0) {
        assert((names.shortname >= '0' && names.shortname <= '9')
            || (names.shortname >= 'a' && names.shortname <= 'z')
            || (names.shortname >= 'A' && names.shortname <= 'Z'));
    }
    _remove_duplicates(names);

    param_t o;
    o._ptr = new detail::opt_base::opt_impl;
    o._ptr->type = detail::opt_type::param;
    o._ptr->shortname = names.shortname;
    o._ptr->longname = names.longname;
    o._ptr->desc = desc;
    _opts.push_back(o);
    return o;
}

void parser::category(const char* name)
{
    assert(name != nullptr);
    opt_base o;
    o._ptr = new detail::opt_base::opt_impl;
    o._ptr->type = detail::opt_type::category;
    o._ptr->desc = name;
    _opts.push_back(o);
}

error parser::parse(int argc, const char* const* argv)
{
    if (_progname != nullptr)
        throw std::runtime_error("arguments already parsed");
    if (argc < 1)
        throw std::runtime_error("invalid argc value");
    if (argv == nullptr)
        throw std::runtime_error("invalid argv value");

    _progname = argv[0];

    auto lookup_short = [this](char c) -> opt_base {
        for (auto it = _opts.begin(); it != _opts.end(); ++it)
            if (it->_ptr->shortname != 0 && it->_ptr->shortname == c)
                return *it;
        return {};
    };

    auto lookup_long = [this](const char* n) -> opt_base {
        for (auto it = _opts.begin(); it != _opts.end(); ++it)
            if (it->_ptr->longname != nullptr && std::strcmp(it->_ptr->longname, n) == 0)
                return *it;
        return {};
    };

    int i = 1;

    for (; i < argc; ++i) {
        const char* arg = argv[i];
        if (arg == nullptr)
            throw std::runtime_error("invalid arg value");
        if (arg[0] != '-') {
            _args.push_back(arg);
        } else {
            if (arg[1] == '\0') {
                // single dash "-"
                _args.push_back(arg);
            } else if (arg[1] == '-') {
                if (arg[2] == '\0') {
                    // double dash "--"
                    ++i;
                    break;
                } else {
                    auto it = lookup_long(&arg[2]);
                    if (it._ptr == nullptr) {
                        return error(error::unknown_option, arg);
                    } else if (it._ptr->type == detail::opt_type::flag) {
                        it._ptr->value = reinterpret_cast<const char*>(1);
                    } else if (it._ptr->type == detail::opt_type::param) {
                        if (++i >= argc)
                            return error(error::missing_argument, arg);
                        it._ptr->value = argv[i];
                    } else {
                        assert(0 && "unexpected option type");
                    }
                }
            } else {
                for (const char* c = &arg[1]; *c != '\0'; ++c) {
                    auto it = lookup_short(*c);
                    if (it._ptr == nullptr) {
                        return error(error::unknown_option, *c);
                    } else if (it._ptr->type == detail::opt_type::flag) {
                        it._ptr->value = reinterpret_cast<const char*>(1);
                    } else if (it._ptr->type == detail::opt_type::param) {
                        if (*(c + 1) != '\0' || ++i >= argc)
                            return error(error::missing_argument, *c);
                        it._ptr->value = argv[i];
                    } else {
                        assert(0 && "unexpected option type");
                    }
                }
            }
        }
    }

    for (; i < argc; ++i)
        _args.push_back(argv[i]);

    return error();
}

} // namespace argparse
