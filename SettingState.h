#ifndef SETTINGSTATE_H
#define SETTINGSTATE_H

class State;
class Thermofan;

// Настройки
class SettingState: public State {
  public:
    SettingState(Thermofan* context);
    ~SettingState();
    virtual void loop();
    void encoder();
};

#endif
