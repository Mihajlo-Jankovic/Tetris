# Tetris in the Linux Terminal

![alt text](https://github.com/Mihajlo-Jankovic/Tetris/blob/master/tetris_screenshot.png?raw=true)

***About:***

This is a basic Tetris game in C with Ncurses. 
In Tetris, players must complete lines by moving differently shaped pieces (tetrominoes), which descend onto the playing field. 
The completed lines disappear and grant the player points, and the player can proceed to fill the vacated spaces. 
The game ends when the playing field is filled. Once the game is finished, it will record and display the top scores.

***How to run:***

1. Install the [`ncurses  package`](https://www.cyberciti.biz/faq/linux-install-ncurses-library-headers-on-debian-ubuntu-centos-fedora/).
2. In Activities tab select Terminal -> Preferences -> Unnamed and set the Cell spacing width to 1,80 and close. Enlarge (***just drag the corners with your mouse***) your terminal so all windows can display properly (***see the screenshot above***). If some window is not showing, scale your terminal more and run (***step 3***) the game again.
3. Make sure you are in the same directory where `tetris.c` file is and write the following line in your Terminal: `gcc -o exe tetris.c -lncurses && ./exe`
4. Enjoy!

For keybindings check the help window in the game.

---

***For developers:*** You can clone the whole repository and check out the code. If you have any suggestions for code improving or visuals, I will be more than glad to hear and learn from you. Thanks in advance for your time!
