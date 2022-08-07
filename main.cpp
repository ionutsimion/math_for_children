#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

using namespace std::string_literals;

namespace
{
    using large_int_t = std::vector<std::uint8_t>;

    struct node_t
    {
        virtual ~node_t() = default;
        [[nodiscard]] virtual large_int_t operator()() const = 0;

        std::unique_ptr<node_t> left{};
        std::unique_ptr<node_t> right{};
    };

    struct number_t : node_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }
    };

    struct unknown_t : node_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }
    };

    struct operator_t : node_t
    {
        enum class type_t
        {
            unary
          , binary
        };

        [[nodiscard]] virtual type_t type() const noexcept = 0;
    };

    struct plus_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    struct minus_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    struct multiply_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    struct divide_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    struct power_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    struct equals_t : operator_t
    {
        [[nodiscard]] type_t type() const noexcept override  { return type_t::binary; }
    };

    auto read_expression()
    {
        std::string expression_string{};

        std::cout << "Operation: ";
        std::getline(std::cin, expression_string);

        return expression_string;
    }

    auto split_expression_string_into_tokens(std::string const &expression_string)
    {
        std::vector<std::string> tokens;

        std::string trimmed_expression_string(expression_string.size(), '\0');
        auto end = std::copy_if(expression_string.cbegin(), expression_string.cend(), trimmed_expression_string.begin(),
                     [](char const c){ return !std::isspace(c) && (std::isalnum(c) || "+-*/:^=()[]{}"s.find(c) != std::string::npos); });
        trimmed_expression_string.erase(end, trimmed_expression_string.end());

        std::string token{};
        for (char & c : trimmed_expression_string)
        {
            if ("+-*/:^=()[]{}"s.find(c) != std::string::npos)
            {
                if (token.empty() && c == '-' && (tokens.empty() || tokens.back().find("([{")))
                {
                    token += c;
                }
                else
                {
                    if (!token.empty())
                    {
                        tokens.emplace_back(token);
                    }
                    tokens.emplace_back(std::string{ c });
                    token.clear();
                }
            }
            else
            {
                token += c;
            }
        }
        if (!token.empty())
        {
            tokens.emplace_back(token);
        }

        return tokens;
    }

    auto is_number(std::string const &s) noexcept
    {
        return (s.size() > (s.front() == '-'))
            && std::all_of(std::next(s.cbegin()), s.cend(), [](char const c){ return std::isdigit(c); })
            && (s.front() == '-' || std::isdigit(s.front()));
    }

    auto is_unknown(std::string const &s) noexcept
    {
        return (s.size() > (s.front() == '-'))
            && std::all_of(std::next(s.cbegin()), s.cend(), [](char const c){ return std::isalnum(c); })
            && (s.front() == '-' || std::isalpha(s.front()));
    }

    auto is_open_parenthesis(std::string const &s) noexcept
    {
        return s.size() == 1 && "([{"s.find(s.front()) != std::string::npos;
    }

    auto is_closed_parenthesis(std::string const &s) noexcept
    {
        return s.size() == 1 && ")]}"s.find(s.front()) != std::string::npos;
    }

    auto is_operator(std::string const &s) noexcept
    {
        return s.size() == 1 && "+-*/:^="s.find(s.front()) != std::string::npos;
    }

    auto parse_expression(std::string const &expression_string)
    {
        std::unique_ptr<node_t> expression{};

        auto const tokens = split_expression_string_into_tokens(expression_string);
        for (auto const &token : tokens)
        {
            if (is_number(token))
            {
                std::cout << "number: ";
            }
            else if (is_unknown(token))
            {
                std::cout << "unknown: ";
            }
            else if (is_open_parenthesis(token))
            {
                std::cout << "open parenthesis: ";
            }
            else if (is_closed_parenthesis(token))
            {
                std::cout << "closed parenthesis: ";
            }
            else if (is_operator(token))
            {
                std::cout << "operator: ";
            }
            std::cout << token << std::endl;
        }

        return expression;
    }

    auto try_again()
    {
        std::cout << "Again? (y or Y for yes): ";
        char option;
        std::cin.get(option).ignore();

        return "yY"s.find(static_cast<char>(option)) != std::string::npos;
    }
} // anonymous namespace

int main()
{
    // TEST: { 100:5 - x * [(-1 + 77):4]} * 10 = 0
    for (;;)
    {
        auto expression = parse_expression(read_expression());
        if (!try_again())
        {
            break;
        }
    }

    return 0;
}
