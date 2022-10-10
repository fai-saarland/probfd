#pragma once

#include <iostream>
#include <vector>

namespace utils {

class Printable {
public:
    virtual ~Printable() = default;
    virtual void print(std::ostream& out) const = 0;
};

class Printables : public Printable {
public:
    Printables(std::vector<Printable*>&& printables);
    virtual ~Printables();
    virtual void print(std::ostream& out) const override;

private:
    std::vector<Printable*> printables_;
};

std::ostream& operator<<(std::ostream& out, const Printable&);

} // namespace utils
