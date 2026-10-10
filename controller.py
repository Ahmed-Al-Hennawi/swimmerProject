import cv2
import mediapipe as mp
import socket
import threading
import time
import numpy as np # Added for better math/smoothing
import json
from vosk import Model, KaldiRecognizer
import pyaudio

# Socket setup
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# Mediapipe setup
mp_hands = mp.solutions.hands
mp_drawing = mp.solutions.drawing_utils
mp_drawing_styles = mp.solutions.drawing_styles
hands = mp_hands.Hands(min_detection_confidence=0.7, min_tracking_confidence=0.7)

# Smoothing variables
smoothed_x = 0.5
alpha = 0.3 # Lower = smoother but slower, Higher = faster but jittery

def send_command(command):
    sock.sendto(command.encode(), (UDP_IP, UDP_PORT))
    print(f"Sent: {command}")

# Voice thread
def listen_voice():
    print("Loading Vosk...")
    model = Model("model")
    print("Model loaded")

    rec = KaldiRecognizer(model, 16000)

    p = pyaudio.PyAudio()

    stream = p.open(
        format=pyaudio.paInt16,
        channels=1,
        rate=16000,
        input=True,
        frames_per_buffer=4000
    )

    stream.start_stream()

    print("Listening...")

    while True:
        data = stream.read(4000, exception_on_overflow=False)

        if rec.AcceptWaveform(data):
            result = json.loads(rec.Result())
            text = result.get("text", "")

            print("Recognized:", text)

            if "left" in text:
                send_command("LEFT")

            elif "right" in text:
                send_command("RIGHT")

            elif "dive" in text or "down" in text:
                send_command("DIVE")
            
threading.Thread(target=listen_voice, daemon=True).start()

cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret: break
    frame = cv2.flip(frame, 1)
    h, w, _ = frame.shape
    
    # Draw UI Zones
    cv2.rectangle(frame, (0, 0), (int(w*0.35), h), (255, 0, 0), 2) # Left Zone
    cv2.rectangle(frame, (int(w*0.65), 0), (w, h), (0, 0, 255), 2) # Right Zone
    
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = hands.process(rgb_frame)

    if results.multi_hand_landmarks:
        for hand_landmarks in results.multi_hand_landmarks:
            mp_drawing.draw_landmarks(frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)
            
            # Smoothing the X coordinate
            wrist_x = hand_landmarks.landmark[mp_hands.HandLandmark.WRIST].x
            smoothed_x = (alpha * wrist_x) + ((1 - alpha) * smoothed_x)
            
            # Visualize the smoothed position
            cv2.circle(frame, (int(smoothed_x * w), 50), 10, (0, 255, 0), -1)


            # Position check with dead zone
            if smoothed_x < 0.35:
                send_command("LEFT")
                current_action = "SWIMMING LEFT"
            elif smoothed_x > 0.65:
                send_command("RIGHT")
                current_action = "SWIMMING RIGHT"
            else:
                # --- CENTER ZONE ---
                send_command("CENTER")
                current_action = "CENTERED"
            
                
           # Punch/Fist detection
            is_fist = (hand_landmarks.landmark[8].y > hand_landmarks.landmark[6].y and 
                       hand_landmarks.landmark[12].y > hand_landmarks.landmark[10].y)
            
            if is_fist:
                send_command("DIVE")
            else:
                send_command("UP") 

    cv2.imshow("Controller", frame)
    if cv2.waitKey(1) & 0xFF == 27: break

cap.release()
cv2.destroyAllWindows()