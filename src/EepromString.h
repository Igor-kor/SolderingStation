#ifndef EEPROMSTRING_H
#define EEPROMSTRING_H

/*
 * https://roboticsbackend.com/arduino-write-string-in-eeprom/
 */

int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}

int readStringFromEEPROM(int addrOffset, String *strToRead,int maxLenght = 0)
{
  int newStrLen = EEPROM.read(addrOffset);
  if(newStrLen > maxLenght && maxLenght != 0){
     *strToRead = String("err");
     return 0;
  }
  char data[newStrLen + 1];
  int countsym = 0;
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
    countsym++;
    if(countsym == maxLenght && maxLenght != 0){
      *strToRead = String("err");
      return 0;
    }
  }
  data[newStrLen] = '\0';
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}

#endif
