import csv
import matplotlib.pyplot as plt
import numpy as np

time_min= 8640*5
time_day=time_min/1440

def data_print1(filename,t_input,color, name, fig_name):  # Funtion to plot the results, one graph per season 
    data1=[1 for i in range(0,8640)] 
    data2=[1 for i in range(0,8640)]
    data3=[1 for i in range(0,8640)]
    data4=[1 for i in range(0,8640)]

    file = open(f"Internal/{filename}", newline = '') # We open and read the file containing the results
    csvReader = csv.reader(file, delimiter = ',')
    index=0

    for row in csvReader:
        data1[index]=float(row[0])
        data2[index]=float(row[1])
        data3[index]=float(row[2])
        data4[index]=float(row[3])
        index+=1

    data=[data1,data2,data3,data4]
    t=np.linspace(0,time_day,8640)

    fig, axes = plt.subplots(4, 1, figsize=(10, 12), sharex=True) # We create the four graph with different color for each seasons
    
    for i in range (0,4):                  
        axes[i].plot(t, data[i], color=color[i], label=name[i])
        axes[i].set_xlabel('Time (Day)')
        axes[i].set_ylabel('DO (mg/L)')
        axes[i].set_ylim((0, 15))
        axes[i].axhline(2, color='red', linestyle='--', label='Fish mortality threshold')
        axes[i].axhline(5, color='grey', linestyle='--', label='Fish stress threshold')
        axes[i].legend(loc='upper left', bbox_to_anchor=(1, 1))
        if t_input>0:
            axes[i].annotate(
            "Input of wastes",   
            xy=(6, data[i][t_input]),
            xytext=(6 + 2, data[i][t_input]+ 2), 
            arrowprops=dict(facecolor='black', shrink=0.05),
            fontsize=10,
            color='black'
            )

    fig.suptitle('Dissolved Oxygen (DO) as a function of time for each season', fontsize=12, weight='bold', y=0.98)
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.savefig(f'Results/{fig_name}', format='png', dpi=300)
    plt.show()

def data_print2(filename, t_input, fig_name): # Funtion to plot the results, all seasons in one graph 
    data1=[1 for i in range(0,8640)]
    data2=[1 for i in range(0,8640)]
    data3=[1 for i in range(0,8640)]
    data4=[1 for i in range(0,8640)]

    file = open(f"Internal/{filename}", newline = '') # We open and read the file containing the results
    csvReader = csv.reader(file, delimiter = ',')
    index=0

    for row in csvReader:
        data1[index]=float(row[0])
        data2[index]=float(row[1])
        data3[index]=float(row[2])
        data4[index]=float(row[3])
        index+=1

    t=np.linspace(0,time_day,8640)  # We create the graph with different color for each seasons

    plt.figure(figsize=(10, 6))
    plt.plot(t, data1, color='#4682B4', label='Winter')
    plt.plot(t, data2, color='#006400', label='Spring')
    plt.plot(t, data3, color='#E75480', label='Summer')
    plt.plot(t, data4, color='#9B59B6', label='Autumn')
    plt.xlabel('Time (Day)')
    plt.ylabel('DO (mg/L)')
    plt.ylim((0, 15))
    plt.axhline(2, color='red', linestyle='--', label='Fish mortality threshold')
    plt.axhline(5, color='grey', linestyle='--', label='Fish stress threshold')
    plt.legend(loc='upper left', bbox_to_anchor=(1.02, 1), borderaxespad=0)
    plt.title('Dissolved Oxygen (DO) as a function of time for each season', fontsize=12, weight='bold', y=1.02)
    if t_input>0:
        plt.annotate(
        "Input of wastes",   
        xy=(6, data1[t_input]),
        xytext=(6+ 2, data1[t_input]+ 2), 
        arrowprops=dict(facecolor='black', shrink=0.05),
        fontsize=10,
        color='black'
    )
    plt.tight_layout(rect=[0, 0, 1, 0.96])
    plt.savefig(f'Results/{fig_name}', format='png', dpi=300)
    plt.show

color=['#4682B4','#006400','#E75480','#9B59B6'] 
seasons=['Winter',' Spring','Summer','Autumn']

#Scenario 1   
    
data_print1("DO_value1.csv",1727,color, seasons,'scenario1.jpg')

#Scenario 2

data_print2("DO_value2.csv", 0,'scenario2.jpg')

#Scenario 3

data_print1("DO_value3.csv", 1727, color, seasons, 'scenario3.jpg')

#Scenario 4

data_print2("DO_value4.csv", 1727,'scenario4.jpg')

#Scenario 5

data_print1("DO_value5.csv", 1727, color, seasons, 'scenario5.jpg')
