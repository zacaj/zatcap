SETUP

1. fill in user.txt (create if not exist)
username = YOUR USERNAME
password = YOUR PASSWORD

2.  on linux: symlink libawesomium-1-7.so.0.0 into /usr/lib
    on windows: if you get an error about msvcr110.dll install http://www.microsoft.com/en-us/download/details.aspx?id=30679
    
3. replace 'zacaj' with your username in every file in columns.txt

4. launch zatcap and hope

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
type NUMBER - matches a certain tweet type (0=tweet,1=retweet,2=(un)favorite,3=follow notification)
by USERNAME - matches a tweet by USERNAME or a retweet by USERNAME
fav - matches any tweet you have favorited

if a tweet matches any command with a - in front of it, it won't be added to the column
if it matches any command with a + in front of it (and none with a -) it will be added to the column
(no logical operations, sorry)
