#ifndef LANDMARKS_LANDMARK_GRAPH_MERGED_H
#define LANDMARKS_LANDMARK_GRAPH_MERGED_H

#include "landmarks/landmark_factory.h"
#include "landmarks/landmark_graph.h"

#include <memory>
#include <vector>

class LandmarkGraphMerged : public LandmarkFactory {
    std::vector<std::shared_ptr<LandmarkGraph> > lm_graphs;
    void generate_landmarks();
    LandmarkNode *get_matching_landmark(const LandmarkNode &lm) const;
public:
    LandmarkGraphMerged(const options::Options &opts);
    virtual ~LandmarkGraphMerged();
};

#endif
