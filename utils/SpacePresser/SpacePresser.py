import pyautogui
import time

count = int(input("How many times to press space?\n> "))
i = 0
time.sleep(3)

while(i < count):
	pyautogui.press("space")
	time.sleep(1/30)
	i-=-1#=/