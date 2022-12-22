#include <LedService.h>

int strToInt(const char* s){
    struct tm tm;
    strptime(s, "%F %T", &tm);
    time_t t = mktime(&tm);

    int tju = t;
    tju -= tju % 3600;

    return tju;
}

LedService::LedService(){
    stime1, stime2, stime3 = millis();
}

void LedService::setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    FastLED.setBrightness(10);
}

void LedService::updateLeds() {
    if (millis() - stime2 >= 6000) {

        deserializeJson(doc, payload);
        // grab the current instant in unix seconds
        time_t now = time(nullptr);

        // char time_string[25];
        // strftime(time_string, 25, "%F %T", localtime(&now));
        // Serial.print("time now: ");
        // Serial.println(String(time_string));

        // mktime Convert tm structure to time_t
        time_t t = mktime(localtime(&now));
        // int tnu = now;
        int tnu = t;
        tnu -= tnu % 3600; // remove minutes, seconds

        //Serial.print("timeu now: ");
        //Serial.println(tnu);


        int tnhr = localtime(&now)->tm_hour % 12;
        // Serial.print("hr now: ");
        // Serial.println(tnhr);

        int co = 0;
        int co2 = 1;
        int co3 = 2;

        int cc = 0;
        int hrs = 0;

        for (int i=0; i<LED_COUNT; i++){

            if(i>=36){
                leds[i] = CRGB::Black;
            }
            
        
            const char* timestr = doc["data"][i]["time"];
            int tju = strToInt(timestr);

            // if(tnu == tju){hrs = i;}

            if(tju >= tnu && cc <=11){
                int val = doc["data"][i]["led"];
                int val2 = doc["data"][i+12]["led"];

                leds[(co+tnhr*3)%36] = ledc[val];
                leds[(co2+tnhr*3)%36] = ledc[val2];

                leds[co3] = CRGB::Black;

                if(cc==tnhr){leds[co3] = CRGB::Teal;}

                co +=3;
                co2 +=3;
                co3 +=3;

                cc++;
            }
        }

        FastLED.show(); // turn led on
        stime2 = millis(); // reset the timer
    }
}


void LedService::getJson() {

	if (millis() - stime3 >= 60000) {
        // wait for WiFi connection
        if (WiFi.isConnected()) {
            //Serial.print("[HTTP] wifi ir piesledzies...\n");
        
            HTTPClient http;
            Serial.print("[HTTP] begin...\n");
            // configure traged server and url
            http.begin("http://172.104.138.226/ep/data.json"); //HTTP
            Serial.print("[HTTP] GET...\n");
            // start connection and send HTTP header
            int httpCode = http.GET();
            // httpCode will be negative on error
            if(httpCode > 0) {
                // HTTP header has been send and Server response header has been handled
                Serial.printf("[HTTP] GET... code: %d\n", httpCode);
                // file found at server
                if(httpCode == HTTP_CODE_OK) {
                    payload = http.getString();
                    Serial.println(payload);
                }
            } else {
                Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
            }
            http.end();

        }

		stime3 = millis();  // reset the timer
	}
}

