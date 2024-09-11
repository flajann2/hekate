#pragma once

#include <cctype>
#include <functional>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
#include <algorithm>

/// Hekate Library includes
/// Order is important for combiner script
namespace hekate {
  /// For some parmeters, we need to denote
  /// an unbounded quantity.
  constexpr int unbounded = -1;

  /// opt types:
  ///   base
  ///   numeric
  ///   string
  ///   flag
  ///   unitary
  enum class type { base, numeric, string, flag, unitary };

  /// qualifiers:
  ///   required
  ///   optional (the default)
  enum class qual { required, optional };

  // forward references
  class cli;
  class cmd;

  using main_args_t = std::vector<std::string>;
  using main_args_iterator_t = main_args_t::const_iterator;

  using nested_f = std::function<void(cmd &)>;
  using command_f = std::function<void(cmd &)>;

  /// Helper functions
  std::string str_tolower(const std::string &s_) {
    std::string s = s_;
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
  }

  std::string str_toupper(const std::string &s_) {
    std::string s = s_;
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return s;
  }

  /// Syntax

  using key_t = std::string;

  // syntax_ is a designator for syntax type.
  /// opt uses visitor pattern to work here
  using syntax_opt_t = std::vector<std::function<bool(cmd &cmd, key_t tok)>>;
  /// param uses the visitor pattern
  using syntax_param_t = std::vector<std::function<void(cmd &cmd, key_t tok)>>;
  /// Alas, forward refercing issues with map necessitates this!!!!
  using cmd_ptr_t = std::shared_ptr<cmd>;
  /// subcommands in actuality
  using syntax_cmd_t = std::unordered_map<key_t, cmd_ptr_t>;

  /** The hetake base class
   */
  class base {
  protected:
    /// does its parsing and returns the new position of the iterator.
    virtual main_args_iterator_t parse(main_args_iterator_t it,
                                       main_args_iterator_t it_end){};

    virtual bool is_flag(const std::string &token) { return token[0] == '-'; }

    virtual bool is_command(const std::string &token) { return false; }

    virtual bool is_parameter(const std::string &token) {
      return !is_flag(token) && !is_command(token);
    }
  };

  /// Optional
  template <type T, qual Q = qual::optional> class opt : public base {
  public:
    template <typename... Args> opt(const Args &... args) {}

    type get_type() { return T; }
    qual get_qual() { return Q; }

    bool operator()(cmd &cmd, std::string tok) {}
  };

  /// Parameter
  template <type T, int min = 0, int max = std::numeric_limits<int>::max()>
  class param : public base {
    std::string description_;

  public:
    explicit param(const std::string &description)
        : description_(description) {}
    void operator()(cmd &cmd, std::string tok) {}

    type get_type() { return T; }
    int get_min() { return min; }
    int get_max() { return max; }

  };

  /**
   * Base class for cmd like cmd and cli.
   */
  class cmd : public base {

    friend std::ostream &operator<<(std::ostream &os, cmd const &c);

  protected:
    nested_f m_fcmd;
    /// command that will be executed iff there are no errors
    /// in the options and parameters for this cmd.
    command_f m_frun;

    syntax_opt_t m_sopts;
    syntax_cmd_t m_scmds;
    syntax_param_t m_sparams;
    bool m_visited = false;

  public:
    const std::string m_name;

    cmd(const std::string &name, nested_f fcmd, command_f frun = nullptr)
        : m_name(name), m_fcmd(fcmd), m_frun(frun) {
      m_fcmd(*this);
    }
    cmd() : m_name("<cmd>") {}
    cmd(cmd &&c)
        : m_name(std::move(c.m_name)), m_fcmd(std::move(c.m_fcmd)),
          m_frun(std::move(c.m_frun)), m_sopts(std::move(c.m_sopts)),
          m_scmds(std::move(c.m_scmds)), m_sparams(std::move(c.m_sparams)) {}
    cmd(cmd &c)
        : m_name(c.m_name), m_fcmd(c.m_fcmd), m_frun(c.m_frun),
          m_sopts(c.m_sopts), m_scmds(c.m_scmds), m_sparams(c.m_sparams) {}

    virtual bool is_command(const key_t &token) override {
      return m_scmds.find(token) != m_scmds.end();
    }

    virtual void parse_flag(std::string flag) {
      std::cout << "FLAG: " << flag << '\n';
    }

    virtual void parse_parameter(std::string parameter) {
      std::cout << "PARM: " << parameter << '\n';
    }

    void parse_command(main_args_iterator_t &it, main_args_iterator_t &it_end) {
      std::cout << "CMD: " << *it << '\n';
      auto local_it = it++;
      m_scmds[*local_it]->parse(it, it_end);
    }

    /**
     * Indeed, it points to either a flag, a new command, or a parameter.
     * parameters and flags are consumed, and commands are given to the
     * matching child cmd.
     */
    main_args_iterator_t parse(main_args_iterator_t it,
                               main_args_iterator_t it_end) override {
      m_visited = true;
      while (it != it_end) {
        if (is_flag(*it)) {
          parse_flag(*it);
          ++it;
        } else if (is_parameter(*it)) {
          parse_parameter(*it);
          ++it;
        } else if (is_command(*it)) {
          parse_command(it, it_end);
        } else { // should never get here!
          throw it;
        }
      }
      return it;
    }

    /// Note that this is a synchronous call, as it will not return
    /// unless the callbacks return themselves!
    void invoke_callbacks() {
      if (m_frun && m_visited) {
        m_frun(*this);
      }
      for (auto [key, sub_frun] : m_scmds) {
        sub_frun->invoke_callbacks();
      }
    }

    /// opt DSL
    template <type T, qual Q> cmd &operator<<(opt<T, Q> &&o) {
      m_sopts.emplace_back(std::function<bool(cmd & c, std::string tok)>(o));
      return *this;
    }

    /// param DSL
    template <type T, int... NX> cmd &operator<<(param<T, NX...> &&p) {
      m_sparams.emplace_back(std::function<void(cmd &, std::string tok)>(p));
      return *this;
    }

    /// cmd DSL
    cmd &operator<<(cmd &&m) {
      m_scmds[m.m_name] = std::make_shared<cmd>(m);
      return *this;
    }

    /// runnable DSL for the cli object
    cmd &operator<<(command_f &&run) {
      m_frun = std::move(run);
      return *this;
    }
  };

  /**
   * Main Hekate cli declaration.
   */
  class cli : public cmd {
    friend std::ostream &operator<<(std::ostream &os, cli const &c);

    // program name (from ac, av)
    std::string progname;

  public:
    cli(const std::string &name, nested_f fcmd, command_f frun = nullptr)
        : cmd(name, fcmd, frun) {}
    cli(cmd &&c) : cmd(c) {}
    cli(cmd &c) : cmd(c) {}
    cli(){};

    /// Process directly from the ac,av from main()
    template <typename AC, typename AV> AC operator()(AC argc, AV argv) {
      main_args_t args;

      progname = argv[0];
      for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
      }
      return (*this)(args);
    }

    /// this parses the incoming arguments from command-line
    int operator()(const main_args_t &args, bool do_callbacks = true) {
      for (auto s : args) {
        std::cout << "-> " << s << '\n';
      }
      parse(args.begin(), args.end());
      if (do_callbacks) {
        invoke_callbacks();
      }
      return 0;
    }
  };

  /// Dump the cli to cout
  std::ostream &operator<<(std::ostream &os, cmd const &c) {
    for (auto [key, cmd] : c.m_scmds) {
      os << *cmd;
    }
    return os;
  }

} // namespace hekate
