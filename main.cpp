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
        [[nodiscard]] virtual std::string to_string() const noexcept = 0;
        [[nodiscard]] virtual uint8_t priority() const noexcept = 0;

        std::shared_ptr<node_t> root{};
        std::shared_ptr<node_t> left{};
        std::shared_ptr<node_t> right{};
    };

    struct number_t : node_t
    {
        explicit number_t(std::string const &s)
        {
            if (s.empty())
            {
                data_ = { 0 };
                is_negative_ = false;
            }
            else
            {
                is_negative_ = s.front() == '-';
                for (auto const c : s)
                {
                    if (std::isdigit(c))
                    {
                        data_.emplace_back(c - '0');
                    }
                }
            }
        }

        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            std::string s{};
            if (is_negative_)
            {
                s += '-';
            }
            for (auto const digit : data_)
            {
                s += static_cast<char>(digit + '0');
            }

            return s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 255u;
        }

    private:
        large_int_t data_;
        bool is_negative_;
    };

    struct unknown_t : node_t
    {
        explicit unknown_t(std::string const &s)
        {
            if (s.empty())
            {
                data_ = "_";
                is_negative = false;
            }
            else
            {
                is_negative = s.front() == '-';
                data_ = s.substr(static_cast<std::size_t>(is_negative));
            }
        }

        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return is_negative ? "-"s + data_ : data_;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 255u;
        }

    private:
        std::string data_;
        bool is_negative;
    };

    struct operator_t : node_t
    {
    };

    struct parenthesis_t : operator_t
    {
        explicit parenthesis_t(std::string const &s)
        {
            if (s.empty())
            {
                symbol_.clear();
            }
            else
            {
                switch (s.front())
                {
                    case '(': symbol_ = "()"; break;
                    case '[': symbol_ = "[]"; break;
                    case '{': symbol_ = "{}"; break;
                    default: symbol_.clear();
                }
            }
        }

        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return symbol_;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 100u;
        }

    private:
        std::string symbol_;
    };

    struct plus_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "+"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 0u;
        }
    };

    struct minus_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "-"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 0u;
        }
    };

    struct multiply_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "*"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 10u;
        }
    };

    struct divide_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return ":"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 10u;
        }
    };

    struct power_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "^"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 20u;
        }
    };

    struct factorial_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "!"s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 30u;
        }
    };

    struct equals_t : operator_t
    {
        [[nodiscard]] large_int_t operator()() const override
        {
            return {};
        }

        [[nodiscard]] std::string to_string() const noexcept override
        {
            return "="s;
        }

        [[nodiscard]] uint8_t priority() const noexcept override
        {
            return 200u;
        }
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
                     [](char const c){ return !std::isspace(c) && (std::isalnum(c) || "+-*/:^!=()[]{}"s.find(c) != std::string::npos); });
        trimmed_expression_string.erase(end, trimmed_expression_string.end());

        std::string token{};
        for (char & c : trimmed_expression_string)
        {
            if ("+-*/:^!=()[]{}"s.find(c) != std::string::npos)
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
        return s.size() == 1 && "+-*/:^!="s.find(s.front()) != std::string::npos;
    }

    auto parse_expression(std::string const &expression_string)
    {
        std::shared_ptr<node_t> pointer{};

        auto const tokens = split_expression_string_into_tokens(expression_string);
        for (auto const &token : tokens)
        {
            if (is_number(token))
            {
                auto number = std::make_shared<number_t>(token);
                if (pointer)
                {
                    number->root = pointer;
                    if (!pointer->left)
                    {
                        pointer->left = number;
                    }
                    else
                    {
                        pointer->right = number;
                    }
                }
                pointer = number;
            }
            else if (is_unknown(token))
            {
                auto unknown = std::make_shared<unknown_t>(token);
                if (pointer)
                {
                    unknown->root = pointer;
                    if (!pointer->left)
                    {
                        pointer->left = unknown;
                    }
                    else
                    {
                        pointer->right = unknown;
                    }
                }
                pointer = unknown;
            }
            else if (is_open_parenthesis(token))
            {
                auto parenthesis = std::make_shared<parenthesis_t>(token);
                if (pointer)
                {
                    if (parenthesis->priority() >= pointer->priority())
                    {
                        parenthesis->root = pointer;
                        if (!pointer->left)
                        {
                            pointer->left = parenthesis;
                        }
                        else
                        {
                            pointer->right = parenthesis;
                        }
                    }
                    else
                    {
                        parenthesis->root = pointer->root;
                        if (pointer->root)
                        {
                            if (pointer->root->left == pointer)
                            {
                                pointer->root->left = parenthesis;
                            }
                            else if (pointer->root->right == pointer)
                            {
                                pointer->root->right = parenthesis;
                            }
                        }
                        parenthesis->left = pointer;
                        parenthesis->right = pointer->right;
                        pointer->root = parenthesis;
                    }
                }
                pointer = parenthesis;
            }
            else if (is_closed_parenthesis(token))
            {
                while (pointer->to_string().find(token.front()) == std::string::npos)
                    pointer = pointer->root;
            }
            else if (is_operator(token))
            {
                operator_t *op_pointer = nullptr;
                switch (token.front()) {
                    case '+': op_pointer = new plus_t(); break;
                    case '-': op_pointer = new minus_t(); break;
                    case '*': op_pointer = new multiply_t(); break;
                    case '/': [[fallthrough]];
                    case ':': op_pointer = new divide_t(); break;
                    case '^': op_pointer = new power_t(); break;
                    case '!': op_pointer = new factorial_t(); break;
                    case '=': op_pointer = new equals_t(); break;
                }
                std::shared_ptr<operator_t> op{op_pointer };
                if (pointer)
                {
                    if (op->priority() >= pointer->priority())
                    {
                        op->root = pointer;
                        if (!pointer->left)
                        {
                            pointer->left = op;
                        }
                        else
                        {
                            pointer->right = op;
                        }
                    }
                    else
                    {
                        op->root = pointer->root;
                        if (pointer->root)
                        {
                            if (pointer->root->left == pointer)
                            {
                                pointer->root->left = op;
                            }
                            else if (pointer->root->right == pointer)
                            {
                                pointer->root->right = op;
                            }
                        }
                        op->left = pointer;
                        op->right = pointer->right;
                        pointer->root = op;
                    }
                }
                pointer = op;
            }
        }

        while (pointer->root)
        {
            pointer = pointer->root;
        }

        return pointer;
    }

    auto try_again()
    {
        std::cout << "Again? (y or Y for yes): ";
        char option;
        std::cin.get(option).ignore();

        return "yY"s.find(static_cast<char>(option)) != std::string::npos;
    }

    void write_expression(std::shared_ptr<node_t> const &node)
    {
        std::cout << node->to_string();
        if (node->left)
        {
            write_expression(node->left);
        }
        if (node->right)
        {
            write_expression(node->right);
        }
    }
} // anonymous namespace

int main()
{
    // TEST: { 100:5 - x * [(-1 + 77):4]} * 10 - 3! = 0
    do
    {
        auto expression = parse_expression(read_expression());
        write_expression(expression);
        std::cout << std::endl;
    }
    while (try_again());

    return 0;
}
