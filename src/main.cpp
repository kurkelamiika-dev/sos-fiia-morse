#include <Arduino.h>

constexpr uint8_t LED_PIN = 2;
constexpr unsigned long UNIT_MS = 200;
constexpr char MESSAGE[] = "SOS FIIA TEST";

struct MorseEntry {
    char character;
    const char* code;
};

constexpr MorseEntry MORSE_TABLE[] = {
    {'A', ".-"},
    {'B', "-..."},
    {'C', "-.-."},
    {'D', "-.."},
    {'E', "."},
    {'F', "..-."},
    {'G', "--."},
    {'H', "...."},
    {'I', ".."},
    {'J', ".---"},
    {'K', "-.-"},
    {'L', ".-.."},
    {'M', "--"},
    {'N', "-."},
    {'O', "---"},
    {'P', ".--."},
    {'Q', "--.-"},
    {'R', ".-."},
    {'S', "..."},
    {'T', "-"},
    {'U', "..-"},
    {'V', "...-"},
    {'W', ".--"},
    {'X', "-..-"},
    {'Y', "-.--"},
    {'Z', "--.."}
};

const char* findMorseCode(char character) {
    for (const MorseEntry& entry : MORSE_TABLE) {
        if (entry.character == character) {
            return entry.code;
        }
    }

    return nullptr;
}

void ledOn(unsigned long durationMs) {
    digitalWrite(LED_PIN, HIGH);
    delay(durationMs);
    digitalWrite(LED_PIN, LOW);
}

void sendSymbol(char symbol) {
    if (symbol == '.') {
        ledOn(UNIT_MS);
    } else if (symbol == '-') {
        ledOn(3 * UNIT_MS);
    }
}

void sendLetter(char character) {
    const char* code = findMorseCode(character);

    if (code == nullptr) {
        return;
    }

    Serial.print(character);
    Serial.print(" ");
    Serial.println(code);

    for (size_t i = 0; code[i] != '\0'; ++i) {
        sendSymbol(code[i]);

        if (code[i + 1] != '\0') {
            delay(UNIT_MS);
        }
    }
}

void sendMessage(const char* message) {
    Serial.println();
    Serial.print("MESSAGE: ");
    Serial.println(message);

    for (size_t i = 0; message[i] != '\0'; ++i) {
        const char character = message[i];

        if (character == ' ') {
            Serial.println("WORD GAP");
            delay(7 * UNIT_MS);
            continue;
        }

        sendLetter(character);

        if (message[i + 1] != '\0' && message[i + 1] != ' ') {
            delay(3 * UNIT_MS);
        }
    }

    Serial.println("MESSAGE COMPLETE");
}

void setup() {
    Serial.begin(115200);
    delay(500);

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    Serial.println();
    Serial.println("ESP32 MORSE TEST READY");
    Serial.println("LED_PIN: GPIO2");
    Serial.println("MESSAGE: SOS FIIA TEST");
}

void loop() {
    sendMessage(MESSAGE);
    delay(3000);
}
