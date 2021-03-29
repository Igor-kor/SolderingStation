#ifndef WAITINGSTATE_H
#define WAITINGSTATE_H

class State;
class Thermofan;

// в этом состоянии будет ожидание
class WaitingState: public State {
   public:
    WaitingState(Thermofan* context);
    void loop();
};

#endif
