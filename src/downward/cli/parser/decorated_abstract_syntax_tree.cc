#include "downward/cli/parser/decorated_abstract_syntax_tree.h"

#include "downward/cli/plugins/options.h"
#include "downward/cli/plugins/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/memory.h"

#include <any>
#include <functional>
#include <limits>
#include <ranges>

using namespace std;

namespace downward::cli::parser {
void ConstructContext::set_variable(const string& name, const std::any& value)
{
    variables[name] = value;
}

void ConstructContext::remove_variable(const string& name)
{
    variables.erase(name);
}

bool ConstructContext::has_variable(const string& name) const
{
    return variables.count(name);
}

std::any ConstructContext::get_variable(const string& name) const
{
    std::any variable = variables.at(name);
    return variable;
}

LazyValue::LazyValue(
    const DecoratedASTNode& node,
    const ConstructContext& context)
    : context(context)
    , node(node.clone())
{
}

LazyValue::LazyValue(const LazyValue& other)
    : context(other.context)
    , node(other.node->clone())
{
}

std::any LazyValue::construct_any() const
{
    ConstructContext clean_context = context;
    utils::TraceBlock block(clean_context, "Delayed construction of LazyValue");
    return node->construct(clean_context);
}

vector<LazyValue> LazyValue::construct_lazy_list()
{
    utils::TraceBlock block(context, "Delayed construction of a list");
    const DecoratedListNode* list_node =
        dynamic_cast<const DecoratedListNode*>(node.get());
    if (!list_node) {
        context.error(
            "Delayed construction of a list failed because the parsed element "
            "was no list.");
    }

    vector<LazyValue> elements;
    elements.reserve(list_node->get_elements().size());
    int elem = 1;
    for (const DecoratedASTNodePtr& element : list_node->get_elements()) {
        utils::TraceBlock(
            context,
            "Create LazyValue for " + to_string(elem) + ". list element");
        elements.emplace_back(LazyValue(*element, context));
        elem++;
    }
    return elements;
}

VariableDefinition::VariableDefinition(
    std::string variable_name,
    DecoratedASTNodePtr variable_expression)
    : variable_name(std::move(variable_name))
    , variable_expression(std::move(variable_expression))
{
}

VariableDefinition::VariableDefinition(VariableDefinition&& other) noexcept
    : variable_name(std::move(other.variable_name))
    , variable_expression(std::move(other.variable_expression))
    , usages(std::move(other.usages))
{
    for (VariableNode* u : usages) { u->definition = this; }
}

VariableDefinition&
VariableDefinition::operator=(VariableDefinition&& other) noexcept
{
    variable_name = std::move(other.variable_name);
    variable_expression = std::move(other.variable_expression);
    usages = std::move(other.usages);

    for (VariableNode* u : usages) { u->definition = this; }

    return *this;
}

void DecoratedASTNode::prune_unused_definitions()
{
    std::vector<VariableDefinition> defs;
    prune_unused_definitions(defs);
}

std::any DecoratedASTNode::construct() const
{
    ConstructContext context;
    utils::TraceBlock block(context, "Constructing parsed object");
    return construct(context);
}

FunctionArgument::FunctionArgument(
    const string& key,
    DecoratedASTNodePtr value,
    bool is_default,
    bool lazy_construction)
    : key(key)
    , value(move(value))
    , is_default(is_default)
    , lazy_construction(lazy_construction)
{
}

string FunctionArgument::get_key() const
{
    return key;
}

DecoratedASTNode& FunctionArgument::get_value()
{
    return *value;
}

const DecoratedASTNode& FunctionArgument::get_value() const
{
    return *value;
}

bool FunctionArgument::is_default_argument() const
{
    return is_default;
}

void FunctionArgument::dump(const string& indent) const
{
    cout << indent << key << " = " << endl;
    value->dump("| " + indent);
}

bool FunctionArgument::is_lazily_constructed() const
{
    return lazy_construction;
}

DecoratedLetNode::DecoratedLetNode(
    std::vector<VariableDefinition> decorated_variable_definitions,
    DecoratedASTNodePtr nested_value)
    : decorated_variable_definitions(move(decorated_variable_definitions))
    , nested_value(move(nested_value))
{
}

void DecoratedLetNode::prune_unused_definitions(
    std::vector<VariableDefinition>& defs)
{
    nested_value->prune_unused_definitions(defs);

    for (auto& def : std::views::reverse(decorated_variable_definitions)) {
        if (def.usages.empty()) {
            def.variable_expression->remove_variable_usages();
        }
    }

    auto [beg, end] = std::ranges::stable_partition(
        decorated_variable_definitions,
        [](const auto& def) { return !def.usages.empty(); });

    defs.insert(
        defs.end(),
        std::make_move_iterator(beg),
        std::make_move_iterator(end));

    decorated_variable_definitions.erase(beg, end);
}

void DecoratedLetNode::remove_variable_usages()
{
    for (auto& def : decorated_variable_definitions) {
        def.variable_expression->remove_variable_usages();
    }
}

std::any DecoratedLetNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(context, "Constructing let-expression");
    for (const auto& [variable_name, variable_definition, _] :
         decorated_variable_definitions) {
        utils::TraceBlock block(
            context,
            "Constructing variable '" + variable_name + "'");
        std::any variable_value = variable_definition->construct(context);
        context.set_variable(variable_name, variable_value);
    }

    std::any result;
    {
        utils::TraceBlock block(context, "Constructing nested value");
        result = nested_value->construct(context);
    }

    for (const auto& [variable_name, variable_definition, _] :
         decorated_variable_definitions) {
        context.remove_variable(variable_name);
    }

    return result;
}

void DecoratedLetNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::println(out, "{:>{}}", "let", indent + 3);

    if (!decorated_variable_definitions.empty()) {
        {
            const auto& [variable_name, variable_definition, _] =
                decorated_variable_definitions.front();
            variable_definition->print(out, indent + 4, print_default_args);
            std::print(out, " as {}", variable_name);
        }

        for (const auto& [variable_name, variable_definition, _] :
             decorated_variable_definitions | std::views::drop(1)) {
            std::println(out, ",");
            variable_definition->print(out, indent + 4, print_default_args);
            std::print(out, " as {}", variable_name);
        }
    }

    std::println(out);
    std::println(out, "{:>{}}", "in", indent + 2);

    nested_value->print(out, indent + 4, print_default_args);
}

void DecoratedLetNode::dump(string indent) const
{
    cout << indent << "LET:";
    indent = "| " + indent;
    for (const auto& [variable_name, variable_definition, _] :
         decorated_variable_definitions) {
        cout << variable_name << " = " << endl;
        variable_definition->dump(indent);
    }
    cout << indent << "IN:" << endl;
    nested_value->dump("| " + indent);
}

DecoratedFunctionCallNode::DecoratedFunctionCallNode(
    const shared_ptr<const plugins::Feature>& feature,
    vector<FunctionArgument>&& arguments,
    const string& unparsed_config)
    : feature(feature)
    , arguments(move(arguments))
    , unparsed_config(unparsed_config)
{
}

void DecoratedFunctionCallNode::remove_variable_usages()
{
    for (auto& arg : arguments) { arg.get_value().remove_variable_usages(); }
}

std::any DecoratedFunctionCallNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing feature '" + feature->get_key() +
            "': " + unparsed_config);
    plugins::Options opts;
    opts.set_unparsed_config(unparsed_config);
    for (const FunctionArgument& arg : arguments) {
        utils::TraceBlock block(
            context,
            "Constructing argument '" + arg.get_key() + "'");
        if (arg.is_lazily_constructed()) {
            opts.set(arg.get_key(), LazyValue(arg.get_value(), context));
        } else {
            opts.set(arg.get_key(), arg.get_value().construct(context));
        }
    }
    return feature->construct(opts, context);
}

void DecoratedFunctionCallNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(
        out,
        "{:>{}}(",
        feature->get_key(),
        indent + feature->get_key().size());

    auto filter =
        print_default_args
            ? static_cast<std::function<bool(const FunctionArgument&)>>(
                  [](const auto&) { return true; })
            : [](const FunctionArgument& arg) {
                  return !arg.is_default_argument();
              };

    if (auto args = arguments | std::views::filter(filter); !args.empty()) {
        {
            const FunctionArgument& arg = args.front();
            std::print(out, "{}=", arg.get_key());
            arg.get_value().print(out, 0, print_default_args);
        }

        for (const FunctionArgument& arg : args | std::views::drop(1)) {
            std::print(out, ", {}=", arg.get_key());
            arg.get_value().print(out, 0, print_default_args);
        }
    }

    std::print(out, ")");
}

void DecoratedFunctionCallNode::dump(string indent) const
{
    cout << indent << "FUNC:" << feature->get_title() << " (returns "
         << feature->get_type().name() << ")" << endl;
    indent = "| " + indent;
    cout << indent << "ARGUMENTS:" << endl;
    for (const FunctionArgument& arg : arguments) { arg.dump("| " + indent); }
}

DecoratedListNode::DecoratedListNode(vector<DecoratedASTNodePtr>&& elements)
    : elements(move(elements))
{
}

void DecoratedListNode::remove_variable_usages()
{
    for (const auto& el : elements) { el->remove_variable_usages(); }
}

std::any DecoratedListNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(context, "Constructing list");
    vector<std::any> result;
    int i = 0;
    for (const DecoratedASTNodePtr& element : elements) {
        utils::TraceBlock block(
            context,
            "Constructing element " + to_string(i));
        result.push_back(element->construct(context));
        ++i;
    }
    return result;
}

void DecoratedListNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    std::print(out, "{:>{}}", "[", indent + 1);

    if (!elements.empty()) {
        {
            const auto& el = elements.front();
            el->print(out, 0, print_default_args);
        }

        for (const auto& el : elements | std::views::drop(1)) {
            std::print(out, ", ");
            el->print(out, 0, print_default_args);
        }
    }

    std::print(out, "]");
}

void DecoratedListNode::dump(string indent) const
{
    cout << indent << "LIST:" << endl;
    indent = "| " + indent;
    for (const DecoratedASTNodePtr& element : elements) {
        element->dump(indent);
    }
}

VariableNode::VariableNode(VariableDefinition& definition)
    : definition(&definition)
{
}

void VariableNode::remove_variable_usages()
{
    const auto it = std::ranges::find(definition->usages, this);
    assert(it != definition->usages.end());
    definition->usages.erase(it);
}

std::any VariableNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Looking up variable '" + definition->variable_name + "'");
    if (!context.has_variable(definition->variable_name)) {
        context.error(
            "Variable '" + definition->variable_name + "' is not defined.");
    }
    return context.get_variable(definition->variable_name);
}

void VariableNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(
        out,
        "{:>{}}",
        definition->variable_name,
        indent + definition->variable_name.size());
}

void VariableNode::dump(string indent) const
{
    cout << indent << "VAR: " << definition->variable_name << endl;
}

BoolLiteralNode::BoolLiteralNode(const string& value)
    : value(value)
{
}

std::any BoolLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing bool value from '" + value + "'");
    istringstream stream(value);
    bool x;
    if ((stream >> boolalpha >> x).fail()) {
        ABORT(
            "Could not parse bool constant '" + value +
            "'"
            " (this should have been caught before constructing this node).");
    }
    return x;
}

void BoolLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

void BoolLiteralNode::dump(string indent) const
{
    cout << indent << "BOOL: " << value << endl;
}

StringLiteralNode::StringLiteralNode(const string& value)
    : value(value)
{
}

std::any StringLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing string value from '" + value + "'");
    if (!(value.starts_with('"') && value.ends_with('"'))) {
        ABORT(
            "String literal value is not enclosed in quotation marks"
            " (this should have been caught before constructing this node).");
    }
    /*
      We are not doing any further syntax checking. Escaped symbols other than
      \n will just ignore the escaping \ (e.g., \t is treated as t, not as a
      tab). Strings ending in \ will not produce an error but should be excluded
      by the previous steps.
    */
    string result;
    result.reserve(value.length() - 2);
    bool escaped = false;
    for (char c : value.substr(1, value.size() - 2)) {
        if (escaped) {
            escaped = false;
            if (c == 'n') {
                result += '\n';
            } else {
                result += c;
            }
        } else if (c == '\\') {
            escaped = true;
        } else {
            result += c;
        }
    }
    return result;
}

void StringLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{:>{}}", value, indent + value.size());
}

void StringLiteralNode::dump(string indent) const
{
    cout << indent << "STRING: " << value << endl;
}

IntLiteralNode::IntLiteralNode(const string& value)
    : value(value)
{
}

std::any IntLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing int value from '" + value + "'");
    if (value.empty()) {
        ABORT(
            "Empty value in int constant '" + value +
            "'"
            " (this should have been caught before constructing this node).");
    } else if (value == "infinity") {
        return numeric_limits<int>::max();
    }

    char suffix = value.back();
    string prefix = value;
    int factor = 1;
    if (isalpha(suffix)) {
        suffix = static_cast<char>(tolower(suffix));
        if (suffix == 'k') {
            factor = 1000;
        } else if (suffix == 'm') {
            factor = 1000000;
        } else if (suffix == 'g') {
            factor = 1000000000;
        } else {
            ABORT(
                "Invalid suffix in int constant '" + value +
                "'"
                " (this should have been caught before constructing this "
                "node).");
        }
        prefix.pop_back();
    }

    istringstream stream(prefix);
    int x;
    stream >> noskipws >> x;
    if (stream.fail() || !stream.eof()) {
        ABORT(
            "Could not parse int constant '" + value +
            "'"
            " (this should have been caught before constructing this node).");
    }

    int min_int = numeric_limits<int>::min();
    // Reserve highest value for "infinity".
    int max_int = numeric_limits<int>::max() - 1;
    if (!utils::is_product_within_limits(x, factor, min_int, max_int)) {
        context.error(
            "Absolute value of integer constant too large: '" + value + "'");
    }
    return x * factor;
}

void IntLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

void IntLiteralNode::dump(string indent) const
{
    cout << indent << "INT: " << value << endl;
}

FloatLiteralNode::FloatLiteralNode(const string& value)
    : value(value)
{
}

std::any FloatLiteralNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing float value from '" + value + "'");
    if (value == "infinity") {
        return numeric_limits<double>::infinity();
    } else {
        istringstream stream(value);
        double x;
        stream >> noskipws >> x;
        if (stream.fail() || !stream.eof()) {
            ABORT(
                "Could not parse double constant '" + value +
                "'"
                " (this should have been caught before constructing this "
                "node).");
        }
        return x;
    }
}

void FloatLiteralNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

void FloatLiteralNode::dump(string indent) const
{
    cout << indent << "FLOAT: " << value << endl;
}

SymbolNode::SymbolNode(const string& value)
    : value(value)
{
}

std::any SymbolNode::construct(ConstructContext&) const
{
    return std::any(value);
}

void SymbolNode::print(std::ostream& out, std::size_t indent, bool) const
{
    std::print(out, "{}", std::string(indent, ' '));
    std::print(out, "{}", value);
}

void SymbolNode::dump(string indent) const
{
    cout << indent << "SYMBOL: " << value << endl;
}

ConvertNode::ConvertNode(
    DecoratedASTNodePtr value,
    const plugins::Type& from_type,
    const plugins::Type& to_type)
    : value(move(value))
    , from_type(from_type)
    , to_type(to_type)
{
}

void ConvertNode::remove_variable_usages()
{
    value->remove_variable_usages();
}

std::any ConvertNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(
        context,
        "Constructing value that requires conversion");
    std::any constructed_value;
    {
        utils::TraceBlock block(
            context,
            "Constructing value of type '" + from_type.name() + "'");
        constructed_value = value->construct(context);
    }
    std::any converted_value;
    {
        utils::TraceBlock block(
            context,
            "Converting constructed value from '" + from_type.name() +
                "' to '" + to_type.name() + "'");
        converted_value =
            plugins::convert(constructed_value, from_type, to_type, context);
    }
    return converted_value;
}

void ConvertNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

void ConvertNode::dump(string indent) const
{
    cout << indent << "CONVERT: " << from_type.name() << " to "
         << to_type.name() << endl;
    value->dump("| " + indent);
}

CheckBoundsNode::CheckBoundsNode(
    DecoratedASTNodePtr value,
    DecoratedASTNodePtr min_value,
    DecoratedASTNodePtr max_value)
    : value(move(value))
    , min_value(move(min_value))
    , max_value(move(max_value))
{
}

template <typename T>
static bool
satisfies_bounds(const std::any& v_, const std::any& min_, const std::any& max_)
{
    T v = std::any_cast<T>(v_);
    T min = std::any_cast<T>(min_);
    T max = std::any_cast<T>(max_);
    return (min <= v) && (v <= max);
}

std::any CheckBoundsNode::construct(ConstructContext& context) const
{
    utils::TraceBlock block(context, "Constructing value with bounds");
    std::any v;
    {
        utils::TraceBlock block(context, "Constructing value");
        v = value->construct(context);
    }
    std::any min;
    {
        utils::TraceBlock block(context, "Constructing lower bound");
        min = min_value->construct(context);
    }
    std::any max;
    {
        utils::TraceBlock block(context, "Constructing upper bound");
        max = max_value->construct(context);
    }
    {
        utils::TraceBlock block(context, "Checking bounds");
        const type_info& type = v.type();
        if (min.type() != type || max.type() != type) {
            ABORT(
                "Types of bounds (" + string(min.type().name()) + ", " +
                max.type().name() + ") do not match type of value (" +
                type.name() + ")" +
                " (this should have been caught before constructing this "
                "node).");
        }

        bool bounds_satisfied = true;
        if (type == typeid(int)) {
            bounds_satisfied = satisfies_bounds<int>(v, min, max);
        } else if (type == typeid(double)) {
            bounds_satisfied = satisfies_bounds<double>(v, min, max);
        } else {
            ABORT(
                "Bounds are only supported for arguments of type int or "
                "double.");
        }
        if (!bounds_satisfied) { context.error("Value is not in bounds."); }
    }
    return v;
}

void CheckBoundsNode::print(
    std::ostream& out,
    std::size_t indent,
    bool print_default_args) const
{
    value->print(out, indent, print_default_args);
}

void CheckBoundsNode::dump(string indent) const
{
    cout << indent << "CHECK-BOUNDS: " << endl;
    value->dump("| " + indent);
    min_value->dump("| " + indent);
    max_value->dump("| " + indent);
}

// We are keeping all copy functionality together because it should be removed
// soon.
FunctionArgument::FunctionArgument(const FunctionArgument& other)
    : key(other.key)
    , value(other.value->clone())
    , is_default(other.is_default)
    , lazy_construction(other.lazy_construction)
{
}

DecoratedLetNode::DecoratedLetNode(const DecoratedLetNode& other)
    : nested_value(other.nested_value->clone())
{
    for (const auto& [name, nested_value, _] :
         other.decorated_variable_definitions) {
        decorated_variable_definitions.emplace_back(
            name,
            nested_value->clone());
    }
}

shared_ptr<DecoratedASTNode> DecoratedLetNode::clone_shared() const
{
    return make_shared<DecoratedLetNode>(*this);
}

unique_ptr<DecoratedASTNode> DecoratedLetNode::clone() const
{
    return std::make_unique<DecoratedLetNode>(*this);
}

DecoratedFunctionCallNode::DecoratedFunctionCallNode(
    const DecoratedFunctionCallNode& other)
    : feature(other.feature)
    , arguments(other.arguments)
    , unparsed_config(other.unparsed_config)
{
}

shared_ptr<DecoratedASTNode> DecoratedFunctionCallNode::clone_shared() const
{
    return make_shared<DecoratedFunctionCallNode>(*this);
}

unique_ptr<DecoratedASTNode> DecoratedFunctionCallNode::clone() const
{
    return std::make_unique<DecoratedFunctionCallNode>(*this);
}

DecoratedListNode::DecoratedListNode(const DecoratedListNode& other)
{
    elements.reserve(other.elements.size());
    for (const DecoratedASTNodePtr& element : other.elements) {
        elements.push_back(element->clone());
    }
}

unique_ptr<DecoratedASTNode> DecoratedListNode::clone() const
{
    return std::make_unique<DecoratedListNode>(*this);
}

shared_ptr<DecoratedASTNode> DecoratedListNode::clone_shared() const
{
    return make_shared<DecoratedListNode>(*this);
}

unique_ptr<DecoratedASTNode> VariableNode::clone() const
{
    return std::make_unique<VariableNode>(*definition);
}

shared_ptr<DecoratedASTNode> VariableNode::clone_shared() const
{
    return make_shared<VariableNode>(*this);
}

BoolLiteralNode::BoolLiteralNode(const BoolLiteralNode& other)
    : value(other.value)
{
}

unique_ptr<DecoratedASTNode> BoolLiteralNode::clone() const
{
    return std::make_unique<BoolLiteralNode>(*this);
}

shared_ptr<DecoratedASTNode> BoolLiteralNode::clone_shared() const
{
    return make_shared<BoolLiteralNode>(*this);
}

StringLiteralNode::StringLiteralNode(const StringLiteralNode& other)
    : value(other.value)
{
}

unique_ptr<DecoratedASTNode> StringLiteralNode::clone() const
{
    return std::make_unique<StringLiteralNode>(*this);
}

shared_ptr<DecoratedASTNode> StringLiteralNode::clone_shared() const
{
    return make_shared<StringLiteralNode>(*this);
}

IntLiteralNode::IntLiteralNode(const IntLiteralNode& other)
    : value(other.value)
{
}

unique_ptr<DecoratedASTNode> IntLiteralNode::clone() const
{
    return std::make_unique<IntLiteralNode>(*this);
}

shared_ptr<DecoratedASTNode> IntLiteralNode::clone_shared() const
{
    return make_shared<IntLiteralNode>(*this);
}

FloatLiteralNode::FloatLiteralNode(const FloatLiteralNode& other)
    : value(other.value)
{
}

unique_ptr<DecoratedASTNode> FloatLiteralNode::clone() const
{
    return std::make_unique<FloatLiteralNode>(*this);
}

shared_ptr<DecoratedASTNode> FloatLiteralNode::clone_shared() const
{
    return make_shared<FloatLiteralNode>(*this);
}

SymbolNode::SymbolNode(const SymbolNode& other)
    : value(other.value)
{
}

unique_ptr<DecoratedASTNode> SymbolNode::clone() const
{
    return std::make_unique<SymbolNode>(*this);
}

shared_ptr<DecoratedASTNode> SymbolNode::clone_shared() const
{
    return make_shared<SymbolNode>(*this);
}

ConvertNode::ConvertNode(const ConvertNode& other)
    : value(other.value->clone())
    , from_type(other.from_type)
    , to_type(other.to_type)
{
}

unique_ptr<DecoratedASTNode> ConvertNode::clone() const
{
    return std::make_unique<ConvertNode>(*this);
}

shared_ptr<DecoratedASTNode> ConvertNode::clone_shared() const
{
    return make_shared<ConvertNode>(*this);
}

CheckBoundsNode::CheckBoundsNode(const CheckBoundsNode& other)
    : value(other.value->clone())
    , min_value(other.min_value->clone())
    , max_value(other.max_value->clone())
{
}

unique_ptr<DecoratedASTNode> CheckBoundsNode::clone() const
{
    return std::make_unique<CheckBoundsNode>(*this);
}

shared_ptr<DecoratedASTNode> CheckBoundsNode::clone_shared() const
{
    return make_shared<CheckBoundsNode>(*this);
}
} // namespace downward::cli::parser