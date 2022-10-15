#ifndef LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_H
#define LANDMARKS_LANDMARK_FACTORY_RPG_EXHAUST_H

#include "landmarks/landmark_factory.h"
#include "landmarks/landmark_graph.h"

class LandmarkFactoryRpgExhaust : public LandmarkFactory {
public:
    LandmarkFactoryRpgExhaust(const options::Options& opts);
    virtual ~LandmarkFactoryRpgExhaust() {}

private:
    void generate_landmarks();
};

#endif
