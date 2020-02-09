#ifndef HOLEFINDER_HPP_INCLUDED
#define HOLEFINDER_HPP_INCLUDED


class HoleFinder {
private:
    int _minDistHole;
    int _defaultDist;

    int _angle;
    int _dist;

    bool _inHole;
    float _distIniHole;
    float _distEndHole;
    int _angleIniHole=0;
    int _angleEndHole=0;

    float _distLidarFiltered;
    float _distLidarOld;
    int _angleOld;

    float _angleCenterHole;
    float _angleHole;
    float _holeWidth;
    float _holeWidthMin; // Hole the attack angle is _angleHole. (projection of _holeWith)
    float _turnToDo;

    float _angleCenterHoleBest=0;
    float _angleHoleBest=0;
    float _holeWidthBest=0;
    float _holeWidthMinBest=0; // Hole the attack angle is _angleHole. (projection of _holeWith)
    float _turnToDoBest=0;

    bool _firstHoleFound=false; // First hole found
    bool _holeFound=false;

    bool _isHoleBigEnough=false;


public:
    HoleFinder(int minDistHole, int defaultDist = 500);
    void update (int angle, int dist, bool reversing);
    float getTurn();
    float getAngleCenterHole();
    bool isHoleFound();
};

#endif