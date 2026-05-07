#ifndef SAMPLE_H
#define SAMPLE_H

#include <StandardDefines.h>
// #include "ISample.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

/*--@Component--*/
class Sample : public ISample {
    Public Virtual ~Sample() = default;

    Public Virtual Void Perform() override {
#ifdef ARDUINO
        Serial.println("Hello worlder");
#endif
    }
        public: static ISamplePtr GetInstance() {
            static ISamplePtr instance(new Sample());
            return instance;
        }
};

template <>
struct Implementation<ISample> {
    using type = Sample;
};

template <>
struct Implementation<ISample*> {
    using type = Sample*;
};

#endif // SAMPLE_H
