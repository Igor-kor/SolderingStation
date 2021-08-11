#ifndef COOLINGSTATE_H
#define COOLINGSTATE_H

class State;
class Thermofan;

// в этом состоянии будет остывание
class CoolingState: public State {
  public:
    int oldspeedfan = 0;
    CoolingState(Thermofan* context);
    ~CoolingState();
    virtual void loop();
};
#endif
