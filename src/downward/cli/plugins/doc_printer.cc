#include "downward/cli/plugins/doc_printer.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "downward/utils/strings.h"

#include <iostream>
#include <map>
#include <print>
#include <stack>

using namespace std;

namespace downward::cli::plugins {
DocPrinter::DocPrinter(ostream& out, Registry& registry)
    : os(out)
    , registry(registry)
{
}

void DocPrinter::print_all() const
{
    const Namespace& n = registry.get_global_name_space();

    for (const auto& subcategory : registry.get_subcategory_plugins()) {
        print_subcategory(subcategory);
    }

    for (const auto categories = n.get_categories();
         const auto& category : categories) {
        print_category(category);
    }
}

void DocPrinter::print_subcategory(const string& name) const
{
    const SubcategoryPlugin& s = registry.get_subcategory_plugin(name);
    print_subcategory(s);
}

void DocPrinter::print_category(const string& qname) const
{
    auto parts = std::views::split(qname, ".") |
                 std::ranges::to<std::vector<std::string>>();
    const auto name = std::move(parts.back());
    parts.pop_back();

    const Namespace& n = registry.get_namespace(parts);
    const auto categories = n.get_categories();
    const auto it = std::ranges::lower_bound(
        categories,
        name,
        {},
        [](const auto& category_plugin) {
            return category_plugin.get_category_name();
        });

    if (it == categories.end() || it->get_category_name() != name) {
        throw MissingCategoryError(
            "could not find a category named '" + name + "' in the registry");
    }

    print_category(*it);
}

void DocPrinter::print_feature(const string& qname) const
{
    auto parts = std::views::split(qname, ".") |
                 std::ranges::to<std::vector<std::string>>();
    const auto name = std::move(parts.back());
    parts.pop_back();
    const auto& n = registry.get_namespace(parts);
    print_feature(n.get_feature(name));
}

void DocPrinter::print_category(const CategoryPlugin& category) const
{
    const auto& subcategories = registry.get_subcategory_plugins();

    std::vector sorted_subcategories(std::from_range, subcategories);
    std::ranges::sort(
        sorted_subcategories,
        {},
        &SubcategoryPlugin::get_subcategory_name);

    print_category_header(category);
    print_category_synopsis(category.get_synopsis());

    vector<const Feature*> features;

    std::stack<const Namespace*> s;
    s.push(&registry.get_global_name_space());

    while (!s.empty()) {
        const auto* n = s.top();
        s.pop();

        for (const auto& feature : n->get_features()) {
            if (const Type& type = feature->get_type();
                type.is_feature_type() &&
                static_cast<const FeatureType&>(type).get_type_index() ==
                    category.get_pointer_type()) {
                features.push_back(feature);
            }
        }

        for (const auto& [name, nested] : n->get_nested_namespaces()) {
            s.push(nested.get());
        }
    }

    std::vector sorted_features(std::from_range, features);
    std::ranges::sort(sorted_features, {}, [](const Feature* f) {
        return f->get_key();
    });

    print_category_members(category, sorted_features);

    print_category_footer();
}

void DocPrinter::print_subcategory(const SubcategoryPlugin& subcategory) const
{
    if (const auto& subcategory_name = subcategory.get_subcategory_name();
        !subcategory_name.empty()) {
        os << endl
           << "===== Topic " << subcategory.get_title() << " =====" << endl;
        if (!subcategory.get_synopsis().empty()) {
            os << subcategory.get_synopsis() << endl;
        }
        os << endl;
    }

    std::vector features(std::from_range, subcategory.get_features());

    std::ranges::sort(features, {}, [](const Feature* f) {
        return f->get_key();
    });

    print_subcategory_members(subcategory, features);
}

void DocPrinter::print_feature(const Feature& feature) const
{
    print_header(feature);
    print_synopsis(feature);
    print_usage(feature);
    print_arguments(feature);
    print_notes(feature);
    print_language_features(feature);
    print_properties(feature);
}

Txt2TagsPrinter::Txt2TagsPrinter(ostream& out, Registry& registry)
    : DocPrinter(out, registry)
{
}

void Txt2TagsPrinter::print_header(const Feature& feature) const
{
    string title = feature.get_title();
    if (title.empty()) { title = feature.get_key(); }
    os << "== " << title << " ==" << endl;
}

void Txt2TagsPrinter::print_synopsis(const Feature& feature) const
{
    if (!feature.get_synopsis().empty()) os << feature.get_synopsis() << endl;
}

void Txt2TagsPrinter::print_usage(const Feature& feature) const
{
    if (!feature.get_key().empty()) {
        os << "``` " << feature.get_key() << "(";
        vector<string> argument_help_strings;
        for (const ArgumentInfo& arg_info : feature.get_arguments()) {
            string arg_help = arg_info.key;
            if (!arg_info.default_value.empty()) {
                arg_help += "=" + arg_info.default_value;
            }
            argument_help_strings.push_back(arg_help);
        }
        std::print(os, "{}", argument_help_strings);
        os << ")" << endl << endl << endl;
    }
}

void Txt2TagsPrinter::print_arguments(const Feature& feature) const
{
    const auto& ftype = feature.get_type();

    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             ftype.get_argument_types(),
             feature.get_arguments(),
             feature.get_argument_docs())) {
        os << "- //" << arg_info.key << "// (" << arg_type->name()
           << "): " << arg_help << endl;
        if (arg_type->is_enum_type()) {
            for (const pair<string, string>& explanation :
                 static_cast<const EnumType&>(*arg_type)
                     .get_documented_enum_values()) {
                os << " - ``" << explanation.first
                   << "``: " << explanation.second << endl;
            }
        }
    }
}

void Txt2TagsPrinter::print_notes(const Feature& feature) const
{
    for (const NoteInfo& note : feature.get_notes()) {
        if (note.long_text) {
            os << "=== " << note.name << " ===" << endl
               << note.description << endl
               << endl;
        } else {
            os << "**" << note.name << ":** " << note.description << endl
               << endl;
        }
    }
}

void Txt2TagsPrinter::print_language_features(const Feature& feature) const
{
    if (!feature.get_language_support().empty()) {
        os << "Language features supported:" << endl;
        for (const LanguageSupportInfo& ls : feature.get_language_support()) {
            os << "- **" << ls.feature << ":** " << ls.description << endl;
        }
    }
}

void Txt2TagsPrinter::print_properties(const Feature& feature) const
{
    if (!feature.get_properties().empty()) {
        os << "Properties:" << endl;
        for (const PropertyInfo& prop : feature.get_properties()) {
            os << "- **" << prop.property << ":** " << prop.description << endl;
        }
    }
}

void Txt2TagsPrinter::print_category_header(
    const CategoryPlugin& category) const
{
    os << ">>>>CATEGORY: " << category.get_category_name() << "<<<<" << endl;
}

void Txt2TagsPrinter::print_category_synopsis(const string& synopsis) const
{
    if (!synopsis.empty()) { os << synopsis << endl; }
}

void Txt2TagsPrinter::print_category_footer() const
{
    os << endl << ">>>>CATEGORYEND<<<<" << endl;
}

PlainPrinter::PlainPrinter(ostream& out, Registry& registry, bool print_all)
    : DocPrinter(out, registry)
    , print_all(print_all)
{
}

void PlainPrinter::print_header(const Feature& feature) const
{
    string title = feature.get_title();
    if (title.empty()) { title = feature.get_key(); }
    os << "== " << title << " ==" << endl;
}

void PlainPrinter::print_synopsis(const Feature& feature) const
{
    if (print_all && !feature.get_synopsis().empty()) {
        os << feature.get_synopsis() << endl;
    }
}

void PlainPrinter::print_usage(const Feature& feature) const
{
    if (!feature.get_key().empty()) {
        std::println(
            os,
            "{}({:n:d})",
            feature.get_key(),
            feature.get_arguments());
    }
}

void PlainPrinter::print_arguments(const Feature& feature) const
{
    const auto& ftype = feature.get_type();

    for (const auto& [arg_type, arg_info, arg_help] : std::views::zip(
             ftype.get_argument_types(),
             feature.get_arguments(),
             feature.get_argument_docs())) {
        os << " " << arg_info.key << " (" << arg_type->name()
           << "): " << arg_help << endl;
        if (arg_type->is_enum_type()) {
            for (const pair<string, string>& explanation :
                 static_cast<const EnumType&>(*arg_type)
                     .get_documented_enum_values()) {
                os << " - " << explanation.first << ": " << explanation.second
                   << endl;
            }
        }
    }
}

void PlainPrinter::print_notes(const Feature& feature) const
{
    if (print_all) {
        for (const NoteInfo& note : feature.get_notes()) {
            if (note.long_text) {
                os << "=== " << note.name << " ===" << endl
                   << note.description << endl
                   << endl;
            } else {
                os << " * " << note.name << ": " << note.description << endl
                   << endl;
            }
        }
    }
}

void PlainPrinter::print_language_features(const Feature& feature) const
{
    if (print_all && !feature.get_language_support().empty()) {
        os << "Language features supported:" << endl;
        for (const LanguageSupportInfo& ls : feature.get_language_support()) {
            os << " * " << ls.feature << ": " << ls.description << endl;
        }
    }
}

void PlainPrinter::print_properties(const Feature& feature) const
{
    if (print_all && !feature.get_properties().empty()) {
        os << "Properties:" << endl;
        for (const PropertyInfo& prop : feature.get_properties()) {
            os << " * " << prop.property << ": " << prop.description << endl;
        }
    }
}

void PlainPrinter::print_category_header(const CategoryPlugin& category) const
{
    os << "Help for " << category.get_category_name() << endl << endl;
}

void PlainPrinter::print_category_synopsis(const string& synopsis) const
{
    if (!synopsis.empty()) { os << synopsis << endl; }
}

void PlainPrinter::print_category_footer() const
{
    os << endl;
}
} // namespace downward::cli::plugins
