SETUP

1. fill in user.txt (create it if it does not exist)
username = YOUR USERNAME
password = 

(yes, leave the password blank)

2.  on linux: symlink libawesomium-1-7.so and libtwitcurl.so into /usr/lib
    on windows: if you get an error about msvcr120.dll install http://www.microsoft.com/en-us/download/details.aspx?id=40784
3. open config.txt, find "browser command = " and replace the path with the path to the .exe for your browser of choice

4. launch zatcap (on linux pass -noredirect the first time so you can see console output)

5.  on windows: right click on "Load Old Tweets" (bottom left) to open the terminal
6. there should be a URL in the terminal, copy this to your browser, and authenticate on twitter
7. type the code twitter gives you back into the terminal, and press enter
8. on windows, right click "Load Old Tweets" again to hide the terminal
9. tada!

COLUMNS

columns are created from the columns/ folder
they are sorted left to right based on the numbers before the _
YOU MUST HAVE A NUMBER AND A _

The format:
+/- COMMAND optional stuff

COMMANDs: (so far)  (ask me if you need one)
all (no opt) -  matches all tweets, events, etc
str A STRING - matches all text after "str" and before the end of the line
rt USERNAME - matches a retweet of USERNAME
type NUMBER - matches a certain tweet type (0=tweet,1=retweet,2=(un)favorite,3=follow notification,4=DM)
by USERNAME - matches a tweet by USERNAME or a retweet by USERNAME
fav - matches any tweet you have favorited

if a tweet matches any command with a - in front of it, it won't be added to the column
if it matches any command with a + in front of it (and none with a -) it will be added to the column
(no logical operations, sorry)

