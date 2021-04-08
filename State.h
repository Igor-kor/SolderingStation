#ifndef STATE_H
#define STATE_H
class Thermofan;

class State {
  public:
    char* StateName;
    Thermofan* context;
    // передается контекст, тоесть сам класс thermofan
    State(Thermofan* context);
    virtual void loop();
};


#endif
