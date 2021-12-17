#ifndef STATE_H
#define STATE_H
class Thermofan;

class State {
  public: 
    Thermofan* context;
    // передается контекст, тоесть сам класс thermofan
    State(Thermofan* context);
    ~State();
    virtual void loop();
    virtual void encoder();
};

#endif
