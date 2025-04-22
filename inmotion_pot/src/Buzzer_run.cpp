#include "Buzzer_run.h"


void playSound(int state)
{
    if (BUZZER_PASSIVE)
    {
        switch (state)
        {
        case 1: // Đủ nước (Super Happy! 😃) - Âm thanh vui vẻ
            for (int i = 0; i < 3; i++)
            {
                tone(BUZZER_PIN, 1000, 200); // Tần số 1000Hz, thời gian 200ms
                delay(250);
                tone(BUZZER_PIN, 1200, 200); // Tần số 1200Hz, thời gian 200ms
                delay(250);
            }
            noTone(BUZZER_PIN);
            break;

        case 2: // Vừa nước (Smiling! :)) - Âm thanh nhẹ nhàng
            for (int i = 0; i < 2; i++)
            {
                tone(BUZZER_PIN, 800, 300); // Tần số 800Hz, thời gian 300ms
                delay(350);
                tone(BUZZER_PIN, 900, 300); // Tần số 900Hz, thời gian 300ms
                delay(350);
            }
            noTone(BUZZER_PIN);
            break;

        case 3: // Thiếu nước (Sad... 🙁) - Âm thanh buồn
            for (int i = 0; i < 2; i++)
            {
                tone(BUZZER_PIN, 400, 500); // Tần số 400Hz, thời gian 500ms
                delay(600);
                tone(BUZZER_PIN, 300, 500); // Tần số 300Hz, thời gian 500ms
                delay(600);
            }
            noTone(BUZZER_PIN);
            break;

        case 4: // Khô (Crying!!! 😭) - Âm thanh cảnh báo
            for (int i = 0; i < 5; i++)
            {
                tone(BUZZER_PIN, 1500, 100); // Tần số 1500Hz, thời gian 100ms
                delay(150);
                tone(BUZZER_PIN, 1800, 100); // Tần số 1800Hz, thời gian 100ms
                delay(150);
            }
            noTone(BUZZER_PIN);
            break;
        }
    }
    else
    {
        // Buzzer chủ động (dùng digitalWrite())
        switch (state)
        {
        case 1: // Đủ nước (Super Happy! 😃)
            for (int i = 0; i < 3; i++)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                delay(200);
                digitalWrite(BUZZER_PIN, LOW);
                delay(250);
            }
            break;

        case 2: // Vừa nước (Smiling! :))
            for (int i = 0; i < 2; i++)
            {
                digitalWrite(BUZZER_PIN, HIGH); 
                delay(300);
                digitalWrite(BUZZER_PIN, LOW);
                delay(350);
            }
            break;

        case 3: // Thiếu nước (Sad... 🙁)
            for (int i = 0; i < 2; i++)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                delay(500);
                digitalWrite(BUZZER_PIN, LOW);
                delay(600);
            }
            break;

        case 4: // Khô (Crying!!! 😭)
            for (int i = 0; i < 5; i++)
            {
                digitalWrite(BUZZER_PIN, HIGH);
                delay(100);
                digitalWrite(BUZZER_PIN, LOW);
                delay(150);
            }
            break;
        }
    }
}
