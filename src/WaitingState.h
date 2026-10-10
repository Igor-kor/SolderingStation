#ifndef WAITINGSTATE_H
#define WAITINGSTATE_H

#include "State.h"

// Ожидание: фен на подставке, нагрев выключен, вентилятор остановлен
class WaitingState : public State {
  public:
    explicit WaitingState(Thermofan& context);
    static WaitingState& instance(Thermofan& context);
    void entry() override;
    void loop() override;
};

#endif
