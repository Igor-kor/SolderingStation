#ifndef WAITINGSTATE_H
#define WAITINGSTATE_H

class State;
class Thermofan;

// в этом состоянии будет ожидание
class WaitingState: public State {
  public:
  int oldspeedfan = 0;
    WaitingState(Thermofan* context);
    ~WaitingState();
    virtual void loop();
};

#endif
