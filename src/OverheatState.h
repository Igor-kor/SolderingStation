#ifndef OVERHEATSTATE_H
#define OVERHEATSTATE_H

#include "State.h"

// Аварийный перегрев: нагрев принудительно выключен, вентилятор на 100%.
// Возврат к работе — после остывания ниже TEMP_OVERHEAT_RESUME
class OverheatState : public State {
  public:
    explicit OverheatState(Thermofan& context);
    static OverheatState& instance(Thermofan& context);
    void entry() override;
    void loop() override;
};

#endif
