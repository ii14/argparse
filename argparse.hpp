// Argument parsing library
// License: UNLICENSE <https://www.unlicense.org>
// Website: https://github.com/ii14/argparse

#ifndef ARGPARSE_HPP
#define ARGPARSE_HPP

#include <vector>
#include <string>

namespace argparse {

struct parser;

namespace detail {

struct opt_base
{
    opt_base() {}

    opt_base(const opt_base& b);
    opt_base& operator=(const opt_base& b);
    opt_base(opt_base&& b) noexcept;
    opt_base& operator=(opt_base&& b) noexcept;
    ~opt_base();

    /// Option short name, eg. "-a".
    /// Returns 0 if short name is not set.
    char shortname() const noexcept;

    /// Option long name, eg. "--opt-a".
    /// Returns nullptr if long name is not set.
    const char* longname() const noexcept;

    /// Option description.
    const char* description() const noexcept;

    /// True if option was present in arguments.
    bool is_set() const noexcept;

    /// True if option was present in arguments.
    operator bool() const noexcept { return is_set(); }

protected:
    struct opt_impl;
    opt_impl* _ptr {nullptr};
    friend struct argparse::parser;
};

struct param_t : opt_base
{
    param_t() {}

    /// Option value.
    /// Returns fallback value if option was not set.
    const char* value(const char* fallback = nullptr) const noexcept;

    /// Option value.
    /// Returns nullptr if option was not set.
    const char* operator*() const noexcept { return value(); }

    friend struct argparse::parser;
};

struct flag_t : opt_base
{
    flag_t() {}

    /// Option value. Same as is_set().
    bool value() const noexcept { return is_set(); }

    /// Option value. Same as is_set().
    bool operator*() const noexcept { return is_set(); }

    friend struct argparse::parser;
};

struct names_t
{
    /// Short name, eg. "-a".
    names_t(char shortname)
        : shortname{shortname} {}

    /// Long name, eg. "--opt-a".
    names_t(const char* longname)
        : longname{longname} {}

    /// Short and long name, eg. "-a" and "--opt-a".
    names_t(char shortname, const char* longname)
        : shortname{shortname}, longname{longname} {}

    char shortname {0};
    const char* longname {nullptr};
};

} // namespace detail

struct error
{
    enum error_type : std::uint8_t {
        ok = 0,
        unknown_option = 1,
        missing_argument = 2,
    };

    explicit error()
        : _type{error_type::ok} {}

    explicit error(error_type type, char shortname)
        : _type{type}, _shortname{'-', shortname, '\0'} {}

    /// Expects a full long name including two dashes eg. "--my-arg".
    /// It's inconsistent with error(error_type, char), but you're
    /// not supposed to construct it yourself.
    explicit error(error_type type, const char* longname)
        : _type{type}, _longname{longname} {}

    /// True if parsing succeeded.
    operator bool() const { return _type == error_type::ok; }

    /// Error type.
    error_type type() const { return _type; }

    /// Option name where the error occurred.
    const char* optname() const { return _longname == nullptr ? _shortname : _longname; }

    /// String representation of the error.
    std::string str() const;

private:
    error_type _type {error_type::ok};
    char _shortname[3] {0};
    const char* _longname {nullptr};
};

struct parser
{
    parser() {}

    using param_t = detail::param_t;
    using flag_t = detail::flag_t;
    using names_t = detail::names_t;
    using opt_base = detail::opt_base;

    /// Creates a flag option.
    /// Either short or long name has to be set.
    /// Short name has to match [0-9A-Za-z].
    flag_t flag(names_t names, const char* desc = nullptr);

    /// Creates a parameter option.
    /// Either short or long name has to be set.
    /// Short name has to match [0-9A-Za-z].
    param_t param(names_t names, const char* desc = nullptr);

    /// Creates a category.
    /// name is required to be a valid string.
    void category(const char* name);

    /// Parses argv.
    /// Can throw exception on unexpected conditions, like invalid argc/argv.
    /// Can be called only once per instance of this class.
    error parse(int argc, const char* const* argv);

    /// Returns program name, argv[0].
    /// Returns nullptr if it's not known yet ie. parse() was not called yet.
    const char* progname() const { return _progname; }

    /// List of all options.
    const std::vector<opt_base>& opts() const { return _opts; }

    /// List of arguments.
    const std::vector<const char*>& args() const { return _args; }

private:
    void _remove_duplicates(const names_t& names);

private:
    const char* _progname {nullptr};
    std::vector<opt_base> _opts;
    std::vector<const char*> _args;
};

} // namespace argparse

#endif // ARGPARSE_HPP
