#ifndef SEARCH_ALGORITHM_FACTORY_H
#define SEARCH_ALGORITHM_FACTORY_H

#include <memory>

class SearchAlgorithm;

class SearchAlgorithmFactory {
public:
    virtual ~SearchAlgorithmFactory() = default;

    virtual std::unique_ptr<SearchAlgorithm> create_algorithm() = 0;
};

#endif
