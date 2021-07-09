#include "printable.h"

#include <utility>

namespace utils {

Printables::Printables(std::vector<Printable*>&& printables)
    : printables_(std::move(printables))
{
}

Printables::~Printables()
{
    for (int i = printables_.size() - 1; i >= 0; --i) {
        delete (printables_[i]);
    }
}

void
Printables::print(std::ostream& out) const
{
    for (unsigned i = 0; i < printables_.size(); ++i) {
        printables_[i]->print(out);
    }
}

std::ostream&
operator<<(std::ostream& out, const Printable& i)
{
    i.print(out);
    return out;
}

} // namespace probabilistic
