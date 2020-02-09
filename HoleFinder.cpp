#include <Arduino.h>
#include "HoleFinder.hpp"

#define MINWIDTH 280

HoleFinder::HoleFinder (int minDistHole, int defaultDist){
    _minDistHole = minDistHole;
    _defaultDist = defaultDist;

    _distLidarOld = _defaultDist;
    _angleOld = 0;
}

void HoleFinder::update (int angleRadar, int distLidar, bool reversing){

    // Needed to get the best hole in each movement. If only one movement, the better is the first.


    if (_inHole && (distLidar<=_minDistHole  || reversing)) {
        
        _distEndHole = !reversing ? distLidar : _defaultDist;
        _angleEndHole = angleRadar;

        // Calculs a fer
        //https://www.edu.xunta.es/espazoAbalar/sites/espazoAbalar/files/datos/1491480889/contido/ud11_trigonometria_II/7_resolucin_de_tringulos.html
        
        _angleCenterHole = (_angleIniHole + _angleEndHole)/2.0;
        _angleHole = abs(_angleIniHole - _angleEndHole);
        _holeWidth = sqrt(pow(_distIniHole,2) + pow(_distEndHole,2) - 2*_distIniHole*_distEndHole*cos(_angleHole * (PI / 180.0)));
        _holeWidthMin = 2.0 * min(_distIniHole, _distEndHole) * sin(_angleHole * (PI/180.0) / 2.0);
        _turnToDo = _angleCenterHole - 90.0;

        // Best hole criteria
        // TODO: holeWithMin or holeWidth?

        _isHoleBigEnough = _holeWidthMin >= MINWIDTH;

        if (_isHoleBigEnough) {
            if ((_holeWidthMin > _holeWidthMinBest) || !_firstHoleFound) {
                _angleCenterHoleBest = _angleCenterHole;
                _angleHoleBest = _angleHole;
                _holeWidthBest = _holeWidth;
                _holeWidthMinBest = _holeWidthMin;
                _turnToDoBest = _turnToDo;

                _firstHoleFound = true;
                _holeFound = true;
                Serial.println("Best found");
            }
        }

        /*
        Serial.print("C -> ");
        Serial.print(_angleCenterHole);
        Serial.print(" A - ");
        Serial.print(_angleHole);
        Serial.print(" DI - ");
        Serial.print(_distIniHole);
        Serial.print(" DE - ");
        Serial.print(_distEndHole);
        Serial.print(" AI - ");
        Serial.print(_angleIniHole);
        Serial.print(" AE - ");
        Serial.print(_angleEndHole);
        Serial.print(" HW - ");
        Serial.print(_holeWidth);
        Serial.print(" HWM - ");
        Serial.println(_holeWidthMin);
        */

        _inHole = false;

    } else if (!_inHole && distLidar>_minDistHole) {
        _distIniHole = _distLidarOld;
        _angleIniHole = _angleOld;
        _inHole = true;
    }

    _distLidarOld = distLidar;
    _angleOld = angleRadar;


    if (reversing) {

        if (!_firstHoleFound){
            _angleCenterHoleBest = 0;
            _angleHoleBest = 0;
            _holeWidthBest = 0;
            _holeWidthMinBest = 0;
            _turnToDoBest = 0;
            _holeFound=false;

        }
        _firstHoleFound = false;
    }
}


float HoleFinder::getTurn(){
    return (_turnToDoBest);
}

float HoleFinder::getAngleCenterHole(){
    return (_angleCenterHole);
}

bool HoleFinder::isHoleFound(){
    return (_holeFound);
}

