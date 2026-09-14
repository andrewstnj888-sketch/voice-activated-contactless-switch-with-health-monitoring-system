import serial
import time
import speech_recognition as sr

# ==========================================
# ESP32 SETTINGS
# ==========================================

COM_PORT = "COM5"       # CHANGE THIS
BAUD_RATE = 9600


# ==========================================
# CONNECT TO ESP32
# ==========================================

try:
    esp32 = serial.Serial(
        COM_PORT,
        BAUD_RATE,
        timeout=1
    )

    time.sleep(2)

    print("ESP32 CONNECTED")
    print("COM PORT:", COM_PORT)

except Exception as e:

    print("ESP32 CONNECTION ERROR")
    print(e)
    exit()


# ==========================================
# SEND HELLO
# ==========================================

esp32.write(b"HELLO\n")

print("HELLO SENT TO ESP32")

time.sleep(1)


# ==========================================
# VOICE RECOGNITION
# ==========================================

recognizer = sr.Recognizer()


# ==========================================
# MAIN PROGRAM
# ==========================================

print("\n====================================")
print("VOICE CONTACTLESS SWITCH")
print("HEALTH MONITORING SYSTEM")
print("====================================")

print("\nCommands:")
print("LED ON")
print("LED OFF")
print("MOTOR ON")
print("MOTOR OFF")
print("TEMPERATURE")
print("STATUS")
print("EXIT")

print("====================================")


while True:

    try:

        # ==================================
        # READ ESP32 HEALTH DATA
        # ==================================

        while esp32.in_waiting > 0:

            data = esp32.readline().decode(
                "utf-8",
                errors="ignore"
            ).strip()

            if data:

                print("[ESP32]", data)


        # ==================================
        # LISTEN
        # ==================================

        with sr.Microphone() as source:

            print("\nListening...")

            recognizer.adjust_for_ambient_noise(
                source,
                duration=0.5
            )

            audio = recognizer.listen(
                source,
                timeout=5,
                phrase_time_limit=5
            )


        # ==================================
        # RECOGNIZE VOICE
        # ==================================

        print("Recognizing...")

        command = recognizer.recognize_google(
            audio,
            language="en-IN"
        )

        command = command.lower()

        print("YOU SAID:", command)


        # ==================================
        # LED ON
        # ==================================

        if "led on" in command:

            esp32.write(b"LED_ON\n")

            print("LED ON")


        # ==================================
        # LED OFF
        # ==================================

        elif "led off" in command:

            esp32.write(b"LED_OFF\n")

            print("LED OFF")


        # ==================================
        # MOTOR ON
        # ==================================

        elif "motor on" in command:

            esp32.write(b"MOTOR_ON\n")

            print("MOTOR ON")


        # ==================================
        # MOTOR OFF
        # ==================================

        elif "motor off" in command:

            esp32.write(b"MOTOR_OFF\n")

            print("MOTOR OFF")


        # ==================================
        # TEMPERATURE
        # ==================================

        elif "temperature" in command:

            esp32.write(b"TEMPERATURE\n")

            print("Temperature requested")


        # ==================================
        # HEALTH STATUS
        # ==================================

        elif (
            "status" in command
            or "health" in command
            or "health status" in command
        ):

            esp32.write(b"STATUS\n")

            print("Health data requested")


        # ==================================
        # EXIT
        # ==================================

        elif (
            "exit" in command
            or "quit" in command
            or "stop system" in command
        ):

            esp32.write(b"SYSTEM_OFF\n")

            print("SYSTEM OFF")

            break


        # ==================================
        # UNKNOWN COMMAND
        # ==================================

        else:

            print("Command not recognized.")


    except sr.WaitTimeoutError:

        print("No voice detected.")


    except sr.UnknownValueError:

        print("Could not understand your voice.")


    except sr.RequestError as e:

        print("Speech recognition error:")
        print(e)


    except KeyboardInterrupt:

        print("\nProgram stopped.")

        try:
            esp32.write(b"SYSTEM_OFF\n")
        except:
            pass

        break


    except Exception as e:

        print("ERROR:", e)


# ==========================================
# CLOSE ESP32
# ==========================================

if esp32.is_open:

    esp32.close()

print("ESP32 disconnected.")