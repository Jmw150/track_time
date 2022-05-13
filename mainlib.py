#!/usr/bin/python3

import time
import math

time_per_day = 10  # in hours
semester = ["Work"]  # categories for the semester
semester_ends = 160 #150 #120  # last year-day of spring classes
is_today = time.localtime().tm_yday
days_left = semester_ends - is_today

# helper functions
# {{{


def get_file(filename):
    File = ""
    with open(filename) as f:
        for l in f.readlines():
            File += l

    return File


def pad_strings(strings):

    for i in range(len(strings)):
        for j in range(len(strings)):
            if len(strings[i]) > len(strings[j]):
                strings[j] += " " * (len(strings[i]) - len(strings[j]))

    return strings


def clean_data():
    # {{{
    "put data in a more streamlined format"
    from os.path import expanduser
    import glob

    home = expanduser("~")

    input_data = []
    for filename in glob.glob(home + "/backup/code/data/*.py"):
        input_data.append(eval(get_file(filename)))

    data = []
    for i in range(len(input_data)):
        # tokenize data from filename
        temp = input_data[i]["filename"].split(":")
        table = {}
        table["task"] = temp[0]
        # start time = s_
        table["s_weekday"] = temp[1]
        table["s_year"] = temp[2]
        table["s_day"] = temp[3]
        table["s_hour"] = temp[4]
        table["s_minute"] = temp[5]
        table["s_second"] = temp[6]
        table["hours"] = input_data[i]["hours"]
        table["minutes"] = input_data[i]["minutes"]
        table["seconds"] = input_data[i]["seconds"]

        data.append(table)
    # }}}
    return data


def hours(seconds):
    return seconds // (60 * 60)


def minutes(seconds):
    return (seconds // 60) % 60


def sec(sec):
    return round(sec % 60)


def clock_sec(seconds: int):
    temp = str(hours(seconds)) + ":"
    if minutes(seconds) < 10:
        temp += "0"
    temp += str(minutes(seconds)) + ":"
    if sec(seconds) < 10:
        temp += "0"
    temp += str(sec(seconds))
    return temp


def clock_hour(hours):
    num = round(hours * 60 * 60)
    return clock_sec(num)


# }}}

def print_total_schedule(
    work_done_total,
    start_of_semester = 129,
    end = 365):

    # present data (set size of data too)
    bar_len = len("000:00:00 of expected 000 hours") + 1  # box line end

    presentation  = bar_len * "=" + "\n"
    presentation += "Time worked this year\n"
    presentation += bar_len * "=" + "\n"

    for day in range(0,len(work_done_total)):
        if work_done_total[day] > 0:               # skip no work days
            presentation += daycal(day)
            if work_done_total[day] < 10 * 60 * 60: # x < 10 hours -> space
                presentation += " "
            presentation += (
                "   "
                + str(clock_sec(work_done_total[day]))
                + "    "
                + dots(work_done_total[day])
                + "|\n"
            )

    presentation += bar_len * "=" + "\n"
    presentation += (
        str(clock_sec(sum(work_done_total)))
        + " of expected "
        + str(((is_today - start_of_semester) * time_per_day))
        + " hours\n"
    )
    presentation += bar_len * "="

    return presentation

def total_schedule(semester=semester):
    "create a chart for the semester"
    # {{{

    data = clean_data() # input from file
    semester = pad_strings(semester)

    # filter for selected days from total data
    work_done_total = []
    for j in range(0, semester_ends): 

        # get data
        day = []
        for i in range(len(data)):
            if int(data[i]["s_day"]) == j:
                day.append(data[i])

        # compute data
        day_hour_total = 0
        # pulling from jobs on the list and adding them to the total
        for ind_semester in range(len(semester)):
            for ind_day in range(len(day)):
                if semester[ind_semester] == day[ind_day]["task"]:
                    day_hour_total += (
                        (day[ind_day]["hours"]) * 60 * 60
                        + (day[ind_day]["minutes"]) * 60
                        + (day[ind_day]["seconds"])
                    )

        # save data
        work_done_total.append(day_hour_total)

    return print_total_schedule(work_done_total)


# }}}


def day_schedule(semester=semester):
    """create a chart based on topics"""
    # {{{
    data = clean_data()
    temp = []
    for s in semester:
        temp.append(s.replace(" ", ""))
    semester = temp[:]

    # initial clean from c code
    semester = pad_strings(semester)

    # filter for 24hr interval
    today = []
    for i in range(len(data)):

        # from yesterday same time to 0:00
        if (
            int(data[i]["s_day"]) == is_today - 1
            and int(data[i]["s_hour"]) >= time.localtime().tm_hour
        ):
            today.append(data[i])

        # from 0:00 to now
        if int(data[i]["s_day"]) == is_today:
            today.append(data[i])

    # select out data based on type of work
    work_done = []
    temp = 0
    for j in range(len(semester)):
        for i in range(len(today)):
            if semester[j] == today[i]["task"]:
                temp += (
                    (today[i]["hours"]) * 60 * 60
                    + (today[i]["minutes"]) * 60
                    + (today[i]["seconds"])
                )
        work_done.append(
            (
                (
                    str(semester[j])
                    + " "
                    + clock_sec(temp)
                    + "/"
                    + clock_hour(time_per_day / len(semester))
                )
            )
        )
        temp = 0

    # construct the chart
    chart = (
        " "
        + str(days_left)
        + " days left\n"
        + str(days_left * time_per_day)
        + "/"
        + str(100 * time_per_day)
        + " hours left\n"
        + "=" * len(work_done[0])
        + "\n"
    )
    for w in work_done:
        chart += str(w) + "\n"
    chart += "=" * len(work_done[0])
    return chart


# }}}


def get_days_left():
    # {{{
    chart = day_schedule(semester)
    chart_lines = chart.split("\n")
    return chart_lines[0] + " "


# }}}


def get_main_task():
    # {{{
    chart = day_schedule(semester)
    chart_lines = chart.split("\n")
    return " " + chart_lines[3][:-6] + " "


# }}}


def daycal(year_day):
    # {{{
    day = 60 * 60 * 24
    return time.ctime(time.time() - day * is_today + day * year_day)[4:-14]


# }}}


def dots(sec, length=10):
    # {{{
    total = round(sec / (60 * 60))  # per hour
    if total > length:
        total = length
    return "*" * total + " " * (length - total)


# }}}


# tests should go here
if __name__ == "__main__":
    # print(day_schedule(semester))
    1 + 1
