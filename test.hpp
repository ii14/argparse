// Argument parsing library
// License: UNLICENSE <https://www.unlicense.org>
// Website: https://github.com/ii14/argparse

#ifndef ARGPARSE_TEST_HPP
#define ARGPARSE_TEST_HPP

#include "argparse.hpp"

#include <vector>
#include <cstring>
#include <cstdio>
#include <functional>
#include <stdexcept>

#define STR_(X) #X
#define STR(X) STR_(X)

#define CAT_(A, B) A ## B
#define CAT(A, B) CAT_(A, B)

#define ASSERT(X) \
    do { \
        if (!(X)) { \
            char buf[1024]; \
            snprintf(buf, 1024, "%s:%ld: ASSERT(%s)", \
                    __FILE__, (size_t)__LINE__, #X); \
            throw test_assert_error(buf); \
        } \
    } while (0)

struct test_assert_error : std::runtime_error {
    explicit test_assert_error(const std::string& str) : std::runtime_error(str) {}
};

struct test_case;

struct test_opt
{
    virtual ~test_opt() {}
    virtual void bind(argparse::parser& p) = 0;
    virtual void test_pre() {}
    virtual void test_post() {}
};

#define TEST_CASE_ARGS \
    std::vector<const char*>& argv, \
    std::vector<test_opt*>& opts, \
    std::vector<const char*>& args, \
    argparse::error& error

struct test_case
{
    size_t id;
    const char* file;
    size_t line;

    std::vector<const char*> argv;
    std::vector<test_opt*> opts;
    std::vector<const char*> args;
    argparse::error error;

    void run() {
        argparse::parser p;
        ASSERT(p.opts().empty());
        ASSERT(p.args().empty());
        ASSERT(p.progname() == nullptr);

        for (auto opt : opts)
            opt->bind(p);
        for (auto opt : opts)
            opt->test_pre();

        auto res = p.parse(argv.size(), argv.data());
        if (!error) {
            ASSERT(res == false);
            ASSERT(res.type() == error.type());
            ASSERT(strcmp(res.optname(), error.optname()) == 0);
        } else {
            ASSERT(res == true);
        }

        for (auto opt : opts)
            opt->test_post();

        ASSERT(p.args().size() == args.size());
        for (size_t i = 0; i < args.size(); ++i)
            ASSERT(strcmp(p.args()[i], args[i]) == 0);
    }

    ~test_case() {
        for (auto opt : opts)
            delete opt;
    }
};

struct test_param : test_opt
{
    char sname {0};
    const char* lname {nullptr};
    const char* desc {nullptr};
    const char* expected {nullptr};
    argparse::parser::param_t param;

    test_param(char sname, const char* lname, const char* desc, const char* expect)
        : sname{sname}, lname{lname}, desc{desc}, expected{expect} {}
    void bind(argparse::parser& p) override { param = p.param({sname, lname}, desc); }
    void test_pre() override { run(nullptr); }
    void test_post() override { run(expected); }

    void run(const char* expect) {
        // TODO: these checks break when shadowing options
        // ASSERT(param.shortname() == sname);
        // if (lname != nullptr) {
        //     ASSERT(param.longname() != nullptr);
        //     ASSERT(strcmp(param.longname(), lname) == 0);
        // } else {
        //     ASSERT(param.longname() == nullptr);
        // }
        // if (desc != nullptr) {
        //     ASSERT(param.description() != nullptr);
        //     ASSERT(strcmp(param.description(), desc) == 0);
        // } else {
        //     ASSERT(param.description() == nullptr);
        // }
        if (expect != nullptr) {
            ASSERT(param);
            ASSERT(param.is_set());
            ASSERT(strcmp(*param, expect) == 0);
            ASSERT(strcmp(param.value(), expect) == 0);
        } else {
            ASSERT(!param);
            ASSERT(!param.is_set());
            ASSERT(*param == nullptr);
            ASSERT(param.value() == nullptr);
        }
    }
};

struct test_flag : test_opt
{
    char sname {0};
    const char* lname {nullptr};
    const char* desc {nullptr};
    bool expected {false};
    argparse::parser::flag_t flag;

    test_flag(char sname, const char* lname, const char* desc, bool expect)
        : sname{sname}, lname{lname}, desc{desc}, expected{expect} {}
    void bind(argparse::parser& p) override { flag = p.flag({sname, lname}, desc); }
    void test_pre() override { run(false); }
    void test_post() override { run(expected); }

    void run(bool expect) {
        // TODO: these checks break when shadowing options
        // ASSERT(flag.shortname() == sname);
        // if (lname != nullptr) {
        //     ASSERT(flag.longname() != nullptr);
        //     ASSERT(strcmp(flag.longname(), lname) == 0);
        // } else {
        //     ASSERT(flag.longname() == nullptr);
        // }
        // if (desc != nullptr) {
        //     ASSERT(flag.description() != nullptr);
        //     ASSERT(strcmp(flag.description(), desc) == 0);
        // } else {
        //     ASSERT(flag.description() == nullptr);
        // }
        if (expect == true) {
            ASSERT(flag);
            ASSERT(flag.is_set());
            ASSERT(*flag);
            ASSERT(flag.value() == true);
        } else {
            ASSERT(!flag);
            ASSERT(!flag.is_set());
            ASSERT(*flag == false);
            ASSERT(flag.value() == false);
        }
    }
};

struct test_category : test_opt
{
    const char* desc {nullptr};

    test_category(const char* desc) : desc{desc} {}
    void bind(argparse::parser& p) override { p.category(desc); }
};

struct test_case_registry
{
    using test_t = void (*)(TEST_CASE_ARGS);

    struct item_t {
        const char* file;
        size_t line;
        test_t test;
    };

    static void add(const char* file, size_t line, test_t test) {
        instance().push_back({ file, line, test });
    }

    static int run() {
        auto reg = test_case_registry::instance();
        for (size_t i = 0; i < reg.size(); ++i) {
            auto& c = reg[i];
            test_case t;
            t.id = i + 1;
            t.file = c.file;
            t.line = c.line;
            fprintf(stderr, ".");
            try {
                c.test(t.argv, t.opts, t.args, t.error);
                t.run();
            } catch (const test_assert_error& e) {
                fprintf(stderr, "FAIL\n%s:%ld: Assertion failed in test case #%ld\n%s\n",
                        t.file, t.line, t.id, e.what());
                return 1;
            } catch (const std::exception& e) {
                fprintf(stderr, "FAIL\n%s:%ld: Uncaught exception in test case #%ld: %s\n",
                        t.file, t.line, t.id, e.what());
                return 1;
            }
        }
        fprintf(stderr, "OK\n");
        return 0;
    }

    static std::vector<item_t>& instance() {
        static std::vector<item_t> reg;
        return reg;
    }
};

struct dummy_t {};

#define TEST_CASE \
    static void CAT(test_case_, __LINE__)(TEST_CASE_ARGS); \
    static dummy_t CAT(test_case_dummy_, __LINE__) = []() -> dummy_t { \
        test_case_registry::add(__FILE__, __LINE__, CAT(test_case_, __LINE__)); \
        return {}; \
    }(); \
    static void CAT(test_case_, __LINE__)(TEST_CASE_ARGS)

#endif // ARGPARSE_TEST_HPP
