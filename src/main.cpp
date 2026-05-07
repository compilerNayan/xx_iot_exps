#include <Arduino.h>
#include "internal/00-public/01-IoTCoreApp.h"
#include "ISample.h"

/*--@Autowired--*/
ISamplePtr sample = Implementation<ISample>::type::GetInstance();


void setup() {
    // Initialize serial communication at 115200 baud
    Serial.begin(115200);
    // Give time for the serial connection to establish
    delay(1000); 
    Serial.println("Starting IoT Tester...");
}

void loop() {
    sample->Perform();
    delay(1000);
}
