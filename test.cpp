// Argument parsing library
// License: UNLICENSE <https://www.unlicense.org>
// Website: https://github.com/ii14/argparse

#include "test.hpp"

int main() { return test_case_registry::run(); }

using err_t = argparse::error;

// flags short name

TEST_CASE {
    argv = {"prog"};
    opts = {
        new test_flag('a', nullptr, "A", false),
        new test_flag('b', nullptr, "B", false),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", false),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "-a"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", false),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-aa"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", false),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-b"};
    opts = {
        new test_flag('a', nullptr, "A", false),
        new test_flag('b', nullptr, "B", true),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-c"};
    opts = {
        new test_flag('a', nullptr, "A", false),
        new test_flag('b', nullptr, "B", false),
        new test_flag('c', nullptr, "C", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "-b"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", true),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-ab"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", true),
        new test_flag('c', nullptr, "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-ab", "-c"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", true),
        new test_flag('c', nullptr, "C", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-cba"};
    opts = {
        new test_flag('a', nullptr, "A", true),
        new test_flag('b', nullptr, "B", true),
        new test_flag('c', nullptr, "C", true),
    };
    args = {};
    error = err_t();
}

// flags long name

TEST_CASE {
    argv = {"prog"};
    opts = {
        new test_flag(0, "opt-a", "A", false),
        new test_flag(0, "opt-b", "B", false),
        new test_flag(0, "opt-c", "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {
        new test_flag(0, "opt-a", "A", true),
        new test_flag(0, "opt-b", "B", false),
        new test_flag(0, "opt-c", "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-b"};
    opts = {
        new test_flag(0, "opt-a", "A", false),
        new test_flag(0, "opt-b", "B", true),
        new test_flag(0, "opt-c", "C", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-c", "--opt-a"};
    opts = {
        new test_flag(0, "opt-a", "A", true),
        new test_flag(0, "opt-b", "B", false),
        new test_flag(0, "opt-c", "C", true),
    };
    args = {};
    error = err_t();
}

// params short name

TEST_CASE {
    argv = {"prog"};
    opts = {
        new test_param('a', nullptr, "A", nullptr),
        new test_param('b', nullptr, "B", nullptr),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "abc"};
    opts = {
        new test_param('a', nullptr, "A", "abc"),
        new test_param('b', nullptr, "B", nullptr),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-b", "def"};
    opts = {
        new test_param('a', nullptr, "A", nullptr),
        new test_param('b', nullptr, "B", "def"),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-b", "def", "-a", "abc"};
    opts = {
        new test_param('a', nullptr, "A", "abc"),
        new test_param('b', nullptr, "B", "def"),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-ba", "abc"};
    opts = {
        new test_param('a', nullptr, "A", "abc"),
        new test_flag('b', nullptr, "B", true),
    };
    args = {};
    error = err_t();
}

// params long name

TEST_CASE {
    argv = {"prog"};
    opts = {
        new test_param(0, "opt-a", "A", nullptr),
        new test_param(0, "opt-b", "B", nullptr),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-a", "abc"};
    opts = {
        new test_param(0, "opt-a", "A", "abc"),
        new test_param(0, "opt-b", "B", nullptr),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-b", "def"};
    opts = {
        new test_param(0, "opt-a", "A", nullptr),
        new test_param(0, "opt-b", "B", "def"),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-b", "def", "--opt-a", "abc"};
    opts = {
        new test_param(0, "opt-a", "A", "abc"),
        new test_param(0, "opt-b", "B", "def"),
    };
    args = {};
    error = err_t();
}

// missing argument error

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {
        new test_param('a', "opt-a", "A", nullptr),
    };
    args = {};
    error = err_t(err_t::missing_argument, "--opt-a");
}

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {
        new test_param('a', "opt-a", "A", nullptr),
    };
    args = {};
    error = err_t(err_t::missing_argument, 'a');
}

TEST_CASE {
    argv = {"prog", "-ab", "5"};
    opts = {
        new test_param('a', "opt-a", "A", nullptr),
    };
    args = {};
    error = err_t(err_t::missing_argument, 'a');
}

// option shadowing

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {
        new test_flag('a', nullptr, "A1", false),
        new test_flag('a', nullptr, "A2", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {
        new test_flag(0, "opt-a", "A1", false),
        new test_flag(0, "opt-a", "A2", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {
        new test_flag('a', "opt-a", "A1", true),
        new test_flag('b', "opt-a", "A2", false),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {
        new test_flag('a', "opt-a", "A1", false),
        new test_flag('b', "opt-a", "A2", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {
        new test_flag('a', "opt-a", "A1", false),
        new test_flag('a', "opt-b", "A2", true),
    };
    args = {};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {
        new test_flag('a', "opt-a", "A1", true),
        new test_flag('a', "opt-b", "A2", false),
    };
    args = {};
    error = err_t();
}

// unknown option error

TEST_CASE {
    argv = {"prog", "-a"};
    opts = {};
    args = {};
    error = err_t(err_t::unknown_option, 'a');
}

TEST_CASE {
    argv = {"prog", "-ab"};
    opts = {};
    args = {};
    error = err_t(err_t::unknown_option, 'a');
}

TEST_CASE {
    argv = {"prog", "-ab"};
    opts = {
        new test_flag('a', nullptr, "A", true),
    };
    args = {};
    error = err_t(err_t::unknown_option, 'b');
}

TEST_CASE {
    argv = {"prog", "--opt-a"};
    opts = {};
    args = {};
    error = err_t(err_t::unknown_option, "--opt-a");
}

// arguments

TEST_CASE {
    argv = {"prog", "a", "b", "c"};
    opts = {};
    args = {"a", "b", "c"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "b", "c"};
    opts = {
        new test_flag('a', nullptr, "A", true),
    };
    args = {"b", "c"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "b", "c"};
    opts = {
        new test_param('a', nullptr, "A", "b"),
    };
    args = {"c"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "b", "c", "-a"};
    opts = {
        new test_flag('a', nullptr, "A", true),
    };
    args = {"b", "c"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "c", "-a", "b"};
    opts = {
        new test_param('a', nullptr, "A", "b"),
    };
    args = {"c"};
    error = err_t();
}

// end parsing options with two dashes

TEST_CASE {
    argv = {"prog", "--", "-a", "-b"};
    opts = {
        new test_flag('a', "opt-a", "A", false),
        new test_flag('b', "opt-b", "B", false),
    };
    args = {"-a", "-b"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "--", "-b"};
    opts = {
        new test_flag('a', "opt-a", "A", true),
        new test_flag('b', "opt-b", "B", false),
    };
    args = {"-b"};
    error = err_t();
}

TEST_CASE {
    argv = {"prog", "-a", "-b", "--"};
    opts = {
        new test_flag('a', "opt-a", "A", true),
        new test_flag('b', "opt-b", "B", true),
    };
    args = {};
    error = err_t();
}

// pass single dash as argument

TEST_CASE {
    argv = {"prog", "-"};
    opts = {};
    args = {"-"};
    error = err_t();
}
