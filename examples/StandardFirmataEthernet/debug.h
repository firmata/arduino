#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEBUG
  #define DEBUG_BEGIN(baud) Serial.begin(baud); while(!Serial) {;}
  #define DEBUG_PRINTLN(x)  Serial.println (x)
  #define DEBUG_PRINT(x)    Serial.print (x)
#else
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT(x)
#endif

#endif /* DEBUG_H */
