#include <iarduino_OLED.h>

class Myiarduino_OLED: public iarduino_OLED {
    char* echoBuffer;
    char* oldEchoBuffer;
 
  public:
     iarduino_OLED* parent;
    Myiarduino_OLED(int addr):iarduino_OLED(addr) {
      iarduino_OLED::autoUpdate(false);
      echoBuffer = new char[200];
      oldEchoBuffer = new char[200];
    };

    char* delchar(char* str, size_t n)
    {
      int len = strlen(str) - n;
      if (len > 0)
        memmove(str + n, str + n + 1, len);
      return str;
    }

    void print(int i) {
      char z[40]; 
      sprintf(z, "%d", i);
      echoBuffer = strcat(echoBuffer, z);
      //если различий нет то и не надо выводить
      if (strncmp(oldEchoBuffer, z, strlen(z)) == 0) {
        oldEchoBuffer =  delchar(oldEchoBuffer, strlen(z));
      } else {
        digitalWrite(13, HIGH);
        iarduino_OLED::print(i);
      }
      digitalWrite(13, LOW);
    }

    void print(char* i) {
       iarduino_OLED::print(i);
       return;
      echoBuffer = strcat(echoBuffer, i);
      //если различий нет то и не надо выводить
      if (strncmp(oldEchoBuffer, i, strlen(i)) == 0) {
        oldEchoBuffer = delchar(oldEchoBuffer, strlen(i));
      } else {
//        digitalWrite(13, HIGH);
        iarduino_OLED::print(i);
      }
//      digitalWrite(13, LOW);
    }

     void printold(int i) {
        iarduino_OLED::print(i);
     }

    void setCursor(int x, int y) {
      iarduino_OLED::setCursor(x, y);
    }

    void update() {
      delete []oldEchoBuffer;
     iarduino_OLED::update();

      oldEchoBuffer = new char[200];
      strcat(oldEchoBuffer, echoBuffer);
      delete []echoBuffer;
      echoBuffer = new char[200];
    }
};
