#ifndef COOLINGSTATE_H
#define COOLINGSTATE_H

#include "State.h"

// Остывание: фен на подставке, нагрев выключен, вентилятор на 100%
class CoolingState : public State {
  public:
    explicit CoolingState(Thermofan& context);
    static CoolingState& instance(Thermofan& context);
    void entry() override;
    void loop() override;
};

#endif
