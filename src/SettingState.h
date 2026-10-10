#ifndef SETTINGSTATE_H
#define SETTINGSTATE_H

#include "State.h"

// Настройка коэффициентов ПИД (вход: зажать кнопку сохранения при включении)
class SettingState : public State {
  public:
    explicit SettingState(Thermofan& context);
    static SettingState& instance(Thermofan& context);
    void entry() override;
    void loop() override;
    void encoder() override;

  private:
    bool oldButtonChange = false;  // прошлое состояние кнопки энкодера
    uint8_t ksel = 0;              // 0 — P, 1 — I, 2 — D
    void showK();                  // вывести выбранный коэффициент на экран
};

#endif
