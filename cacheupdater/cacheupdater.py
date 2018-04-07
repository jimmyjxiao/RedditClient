import urllib.request
import json

def processOnce():
    f = urllib.request.urlopen('https://www.reddit.com/subreddits/.json?include_over_18=on&raw_json=1&count=1000')
    print(f.read())

processOnce()
    
