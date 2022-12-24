#include <LedService.h>

int strToInt(const char* s){
    struct tm tm;
    strptime(s, "%F %T", &tm);
    time_t t = mktime(&tm);

    int t_unix = t;
    t_unix -= t_unix % 3600;

    return t_unix;
}

LedService::LedService(){
    stime1, stime2, stime3 = millis();

    payload = "{\"data\":[{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4},{ \"time\":\"2030-01-01 00:00:00\",\"led\":3},{ \"time\":\"2030-01-01 00:00:00\",\"led\":2},{ \"time\":\"2030-01-01 00:00:00\",\"led\":1},{ \"time\":\"2030-01-01 00:00:00\",\"led\":5},{ \"time\":\"2030-01-01 00:00:00\",\"led\":4}]}";
}

void LedService::setup() {
    FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);  // GRB ordering is assumed
    FastLED.setBrightness(10);
}

void LedService::updateLeds() {

    if (millis() - stime2 >= UPD_LEDS) {

        deserializeJson(doc, payload);

        time_t now = time(nullptr); // grab the current instant in unix seconds
        time_t t = mktime(localtime(&now)); // mktime Convert tm structure to time_t
        int t_now_unix = t;
        t_now_unix -= t_now_unix % 3600; // remove minutes, seconds
        //Serial.print("timeu now: ");
        //Serial.println(t_now_unix);

        int t_now_hr = localtime(&now)->tm_hour % 12;
        // Serial.print("t_now_hr now: ");
        // Serial.println(t_now_hr);

        int ct = 0;

        // 2x24hr entries in json
        for (int i=0; i<48; i++){
            const char* timestr = doc["data"][i]["time"];
            int t_json_unix = strToInt(timestr);

            if(t_json_unix == t_now_unix) ct = i;
            leds[i] = colors[0];
        }

        leds[t_now_hr*3] = CRGB::Teal;
        
        for (int i=0; i<12; i++){
            int val1 = doc["data"][i+ct]["led"];
            int val2 = doc["data"][i+ct+12]["led"];

            leds[(i*3 +1 +t_now_hr*3)%36] = colors[val1];
            leds[(i*3 +2 +t_now_hr*3)%36] = colors[val2];
        }

        FastLED.show(); // turn led on
        stime2 = millis(); // reset the timer
    }
}


void LedService::getJson() {

	if (millis() - stime3 >= UPD_JSON) {

        if (WiFi.isConnected()) {
            //Serial.print("[HTTP] wifi ir piesledzies...\n");
        
            HTTPClient http;
            //WiFiClient client; // 8266
            Serial.print("[HTTP] begin...\n");
            // configure traged server and url
            http.begin("http://172.104.138.226/ep/data.json"); //HTTP
            //http.begin(client, "http://172.104.138.226/ep/data.json"); //8266
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
