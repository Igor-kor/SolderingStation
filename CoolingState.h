#ifndef COOLINGSTATE_H
#define COOLINGSTATE_H

class State;
class Thermofan;

// в этом состоянии будет остывание
class CoolingState: public State {
   public:
    CoolingState(Thermofan* context);
    virtual void loop();
};
#endif
