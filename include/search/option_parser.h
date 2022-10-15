#ifndef OPTION_PARSER_H
#define OPTION_PARSER_H

/*
  TODO: This file was intended to be a temporary stub. We should think
  of getting rid of it. This would entail including "options/options.h"
  and "options/option_parser.h" instead and explicitly using the options
  namespace. See also option_parser_util.h, plugin.h and issue588 for a
  discussion.
*/
#include "options/errors.h"
#include "options/option_parser.h"

using options::Bounds;
using options::OptionParser;
using options::OptionParserError;
using options::Options;
using options::ParseError;

namespace options {

template<class BaseClass, class ActualClass>
std::shared_ptr<BaseClass>
parse(OptionParser& parser)
{
    ActualClass::add_options_to_parser(parser);
    Options opts = parser.parse();
    if (!parser.dry_run()) {
        return std::make_shared<ActualClass>(opts);
    }
    return nullptr;
}

template<class BaseClass, class ActualClass>
std::shared_ptr<BaseClass>
parse_without_options(OptionParser& parser)
{
    parser.parse();
    if (parser.dry_run()) {
        return nullptr;
    }
    return std::make_shared<ActualClass>();
}

} // namespace options

#endif
