import urllib.request
import json
import sqlite3
import time
dbConn = sqlite3.connect('AppDB.db')
db = dbConn.cursor()
def storesub(subData):
    print(subData['display_name_prefixed'])
    colorstr = subData['key_color']
    colorint = 0;
    if(colorstr != ''):
        colorint = int(colorstr[1:], 16)
    else:
        colorint = None
    print(subData['subscribers'])
    nsfwbool = 3
    if(subData['over18'] == True):
        nsfwbool = 1
    elif(subData['over18'] ==False):
        nsfwbool = 0
    submis = 5
    if(subData['submission_type'] == 'any'):
        submis = 2
    elif(subData['submission_type'] == 'self'):
        submis = 1
    elif(subData['submission_type'] == "link"):
        submis = 0
    db.execute("INSERT OR REPLACE INTO SubredditInfoCache (subreddit,sidebar_html, key_color, NSFW, submissionType, subscribers, desc, lastModified) VALUES (?,?, ?, ?, ?, ?, ?, datetime('now'))", (subData['display_name'],subData['description_html'], colorint, nsfwbool,  submis, subData['subscribers'], subData['public_description'] ))
def processOnce(after):
    url = 'https://www.reddit.com/subreddits/.json?include_over_18=on&raw_json=1&count=1000'
    if(after != None):
        url += '&after=' + after
    print(url)
    js = json.loads(urllib.request.urlopen(urllib.request.Request(url, headers= {'User-Agent' : 'super happy flair bot by /u/spladug'})).read())
    for sub in js['data']['children']:
        storesub(sub['data'])
    return js['data']['after']
af = processOnce(None)
count = 0
while count < 100:
    af = processOnce(af)
    time.sleep(1.1)
    count+=1
dbConn.commit()
dbConn.close()