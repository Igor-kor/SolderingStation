#ifndef WARMINGSTATE_H
#define WARMINGSTATE_H

#include "State.h"

// Нагрев: фен в работе, мощность нагревателя задаёт ПИД
class WarmingState : public State {
  public:
    explicit WarmingState(Thermofan& context);
    static WarmingState& instance(Thermofan& context);
    void entry() override;
    void loop() override;
};

#endif
