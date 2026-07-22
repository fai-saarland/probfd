create_library(
    NAME context
    HELP "context class"
    SOURCES
    language/context
)


create_library(
    NAME plugins
    HELP "Plugin definition"
    SOURCES
    language/plugins/bounds
    language/plugins/doc_printer
    language/plugins/options
    language/plugins/plugin
    language/plugins/plugin_info
    language/plugins/raw_registry
    language/plugins/registry
    language/plugins/registry_types
    language/plugins/types
    language/plugins/type_registry
    DEPENDS
    context
)

create_library(
    NAME parser
    HELP "Option parsing"
    SOURCES
    language/parser/abstract_syntax_tree
    language/parser/declaration
    language/parser/decorated_abstract_syntax_tree
    language/parser/lexical_analyzer
    language/parser/syntax_analyzer
    language/parser/token
    language/parser/token_stream
    DEPENDS
    context
)