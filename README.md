Enrico Gunawan

CS 120B

Final Project

August 31, 2017

## Simon

My final project for this class is Simon, a game that asks a player to re-enter sequence displayed on LED. When it is turned on, the LCD will output a welcome message. You need to press any button to start the game. It displays a random sequence and you need to re-enter the sequence displayed correctly in order to win. The game uses 5 LEDs to produce random sequences and 5 buttons (A0-A4) for you to repeat the sequence. It has a feature to produce unique sound for each button pressed. Button A0, for example, produces a frequency of 523.25 Hz, which is a C5 note. The game has 7 levels in total. 

You start with Level 1, which has a sequence of 3 LEDs. If you guess correctly, you can continue to the next level, which is Level 2 in this case. The last level, Level 7, has a sequence of 9 LEDs. You win after you successfully beat Level 7. After you have won, the game ends and returns to the beginning. You will be given three lives to complete the game. If you do not input the right sequence, you lose one life, and you retry that level. After three mistakes, you lose and you need to start from the beginning. The LCD shows you the current level, your remaining lives, and your score. You get one point for every correct button pressed. This means you get 3 points when you finish Level 1, 4 points for Level 2, and so on. The game saves your highest score. The highest score possible for this game is 42 (3+4+5+6+7+8+9). Enjoy the game!

## Notes
- Button A0 = C5 note = 523.25 Hz 
- Button A1 = D5 note = 587.33 Hz 
- Button A2 = E5 note = 659.26 Hz 
- Button A3 = F5 note = 698.46 Hz 
- Button A4 = G5 note = 783.99 Hz 

## Technologies and components used:
1. AVR Studio 6 software
2. 5 LEDs
3. 5 buttons
4. 5 330? resistors
5. A potentiometer
6. 5V power supply
7. ATMega1284 microcontroller
8. LCD screen
9. A speaker


