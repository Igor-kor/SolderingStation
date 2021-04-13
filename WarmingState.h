#ifndef WARNINGSTATE_H
#define WARNINGSTATE_H

class State;
class Thermofan;

// в этом состоянии будет нагрев
class WarmingState: public State {
  public:
    WarmingState(Thermofan* context);
    ~WarmingState();
    virtual void loop();
};

#endif
