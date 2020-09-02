#ifndef LANDMARKS_LANDMARK_GRAPH_MERGED_H
#define LANDMARKS_LANDMARK_GRAPH_MERGED_H

#include "landmark_factory.h"
#include "landmark_graph.h"
#include <vector>
#include <memory>

class LandmarkGraphMerged : public LandmarkFactory {
    std::vector<std::shared_ptr<LandmarkGraph> > lm_graphs;
    void generate_landmarks();
    LandmarkNode *get_matching_landmark(const LandmarkNode &lm) const;
public:
    LandmarkGraphMerged(const options::Options &opts);
    virtual ~LandmarkGraphMerged();
};

#endif
