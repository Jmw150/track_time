#!/usr/bin/python3

time_per_day = 8 #hrs

# topics for the summer
summer = [
  'programming',
  'compilers  ', 
#  'AI         ', 
#  'EM         ',
         ]

import time
import glob

def get_file(filename) :
    File = ""
    with open(filename) as f:
        for l in f.readlines() :
            File += l

    return File


def clean_data() :
    
    from os.path import expanduser
    home = expanduser("~")

    input_data = []
    for filename in glob.glob(home+'/backup/code/data/*.py'):
        input_data.append(eval(get_file(filename)))
    
    data = []
    for i in range(len(input_data)) :
        # tokenize data from filename
        temp = input_data[i]['filename'].split(':')
        table = {}
        table['task'] = temp[0] 
        # start time = s_
        table['s_weekday'] = temp[1] 
        table['s_year'] = temp[2] 
        table['s_day'] = temp[3] 
        table['s_hour'] = temp[4] 
        table['s_minute'] = temp[5] 
        table['s_second'] = temp[6] 
        table['hours'] = input_data[i]['hours']
        table['minutes'] = input_data[i]['minutes']
        table['seconds'] = input_data[i]['seconds']
        
        data.append(table)

    return data

def hours(seconds) :
    return seconds//(60*60)

def minutes(seconds) : 
    return (seconds//60) % 60

def sec(sec) :
    return round(sec % 60)

def clock_sec(seconds:int) :
    temp = str(hours(seconds)) + ':'
    if minutes(seconds) < 10 :
        temp += '0'
    temp += str(minutes(seconds)) + ':' 
    if sec(seconds) < 10 :
        temp += '0'
    temp += str(sec(seconds))
    return temp

def clock_hr(hours) :
    num = round(hours*60*60)
    return clock_sec(num)

def summer_schedule(data,summer) :

    summer_ends = 235 #'august 23' # (classes start)
    is_today = time.gmtime().tm_yday
    days_left = summer_ends - is_today



    today = []
    # filter for today
    for i in range(len(data)) :
        if data[i]['s_day'] == str(is_today) :
            today.append(data[i])

    #today = data 
    # calculate based on type
    work_done = []
    temp = 0
    for j in range(len(summer)) :
     for i in range(len(today)) :
      if summer[j].replace(' ','') == today[i]['task'] :
        temp += ((today[i]['hours'])*60*60 + (today[i]['minutes'])*60 + (today[i]['seconds']))
     work_done.append( (str(summer[j]) + ' ' 
                + clock_sec(temp)
                + '/' + clock_hr(time_per_day/len(summer))))
     temp = 0

    chart = ' ' + str(days_left) +'/100 days left\n'
    chart += str(days_left * time_per_day) + '/' + str(100 * time_per_day) + ' hours left\n'
    chart += '='*len(work_done[0])+'\n'
    for w in work_done :
        chart += str(w) + '\n'
    chart += '='*len(work_done[0])
    return chart

fall = ['compilers', 'complexity', 'algorithms']
spring = ['ML', 'probability', 'stat patterns']
#print(clean_data())
print(summer_schedule(clean_data(),summer))
