#pragma once

#include <iostream>
#include <vector>

namespace probabilistic {

class IPrintable {
public:
    virtual ~IPrintable() = default;
    virtual void print(std::ostream& out) const = 0;
};

class IPrintables : public IPrintable {
public:
    IPrintables(std::vector<IPrintable*>&& printables);
    virtual ~IPrintables();
    virtual void print(std::ostream& out) const override;

private:
    std::vector<IPrintable*> printables_;
};

std::ostream& operator<<(std::ostream& out, const IPrintable&);

} // namespace probabilistic
