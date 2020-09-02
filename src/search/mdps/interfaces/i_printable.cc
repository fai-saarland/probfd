#include "i_printable.h"

#include <utility>

namespace probabilistic {

IPrintables::IPrintables(std::vector<IPrintable*>&& printables)
    : printables_(std::move(printables))
{
}

IPrintables::~IPrintables()
{
    for (int i = printables_.size() - 1; i >= 0; --i) {
        delete (printables_[i]);
    }
}

void
IPrintables::print(std::ostream& out) const
{
    for (unsigned i = 0; i < printables_.size(); ++i) {
        printables_[i]->print(out);
    }
}

std::ostream&
operator<<(std::ostream& out, const IPrintable& i)
{
    i.print(out);
    return out;
}

} // namespace probabilistic
