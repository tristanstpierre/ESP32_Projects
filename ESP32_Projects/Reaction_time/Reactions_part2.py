import matplotlib.pyplot as plt
import numpy as np

list1 = []
list2 = []

with open('reactions.csv', 'r') as dfile:
    lines = dfile.readlines()
    for i in lines:
        r = i.split(',')
        l1 = r[0]
        l2 = r[1]
        list1.append(l1)
        list2.append(l2)
print(list1)

list2_1 = []
for sub in list2:
    list2_1.append(sub.replace("\n", "").replace(' ', ''))
print(list2_1)

reaction_time = [] 
for i in range (len (list1)):  
    reaction_time.append( int(list2_1[i]) - int(list1[i])) 
print(reaction_time)

trial_number = np.arange(len(reaction_time))

def average_of_positive_numbers(numbers):
    pos_nums = [num for num in numbers if num > 0]
    return sum(pos_nums) / len(pos_nums) if pos_nums else 0

avg = average_of_positive_numbers(reaction_time)

def plot_bar_chart(d):
    fig, ax = plt.subplots()
    y_pos = [0 for _ in range(len(d))]

    for i, val in enumerate(d):
        if val < 0:
            color = 'blue'
        else:
            color = 'red'

        ax.bar(i, abs(val), color=color, bottom=min(y_pos[i], 0))

    plt.title(f'Average Time for Valid Trials = {avg} ms')
    plt.xlabel('Trial')
    plt.ylabel('Time (ms)')
    plt.xticks(trial_number)

    plt.show()

plot_bar_chart(reaction_time)
