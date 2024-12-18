# Computational Methods and Tools - Project : River pollution simulation

## Project description

This program simulates different scenario of a release of waste into the Rhone river, to show the impact of waste on dissolved oxygen and thus on aquatic life (in this case fish).

The program will : 

1. Read the flow of the river obtained from the Swiss Federal bureau of the Environment, Hydrology Division ("*Flow_river.csv*" located in "*Data*") and create a list to store those values.
2. Compute the evolution of the dissolved oxygen in the river as a function of time for each season ("*Code/projet.c*").
3. Fill in the csv files already created for each scenario (CSVs located in "*Internal*")
4. Plot the graph for each different scenario ("*Code/projet.py*") and save them in the folder "*Results*".

## Project structure

- "*Data*" contains input data
- "*Code*" contains program code in C and python.
- "*Internal*" contains csv files that are complete by C to then be passed to python. They are automatically edited by the program and should not be manually modified.
- "*Results*" contains saved .png files of graphs and a texte file with the parameters for each scenario

### Inputs and Outputs

Inputs:
- "*Data/Flow_river.csv*" is a line-delimited file.

Internal:
- "*Internal/DO_valuei.csv*" for i = 1,2,3,4,5 is a comma-delimted file

Outputs:
- "*Results/scenarioi.jpg*" for i = 1,2,3,4,5 is an image file
-"*Results/parameters.txt*" a text file

### Implementation details

**Overview:**
-The simulation is handled by C. It reads data of a CSV and writes the results of computation directly into CSV files.
-C also writes changed parameters for each scenario into a text file 
-Python reads into the completed CSV files and handles the visualization of results

**Structure:** In the directory "*Code/*":
-"*projet.c*":
    -Reads data in "*Data/Flow_river.csv*" and creates a list to store those value
    -Writes the defined parameters of each scenario in the text file "*Results/parameters.txt*"
    -Calculates missing parameters (L0, D0, kr, kd, etc.) for each scenario
    -Exports results into the CSVs "*Internal/DO_valuei.csv*" for i = 1,2,3,4,5

-"*project.py*":
    -Reads the data calculated by C into the CSV files "*Internal/DO_valuei.csv*" for i = 1,2,3,4,5
    -Plots the results of each season for each scenario in matplotlib with the addition of fish survival threshold
    -Saves the plot in "*Results*"

## Instructions


To reproduce results in the report, two steps should be followed:
1. Navigate to the makefile to ensure the Python interpreter selected is yours.
2. Run the following line in the terminal from the project root directory (location of this README.md file):
    ```
    make
    ```
The program will run automatically and open windows with the plotted graph and generates the "*scenarioi.jpg*", for i = 1,2,3,4,5, image files.
## Requirements

Versions of Python and C used are as follows.
```
$ python --version
Python 2.7.18

$ gcc --version
gcc (Ubuntu 9.4.0-1ubuntu1~20.04.2) 9.4.0

The Python libraries used were the following:

numpy 1.23.2

csv 1.0
```
## Data
The data file "*Flow_river.csv*" comes from [Swiss Federal Bureau for the Environment](https://www.bafu.admin.ch/bafu/fr/home/themes/eaux/etat/donnees/obtenir-des-donnees-mesurees-sur-le-theme-de-l-eau/commander-des-donnees-hydrologiques-historiques-et-validees.html)

## Formulae

All the function in "*project.c*" that allow us to calculate dissolved oxygen are taken from Pearson's *[Introduction to Environmental Engineering and Science](https://www.pearson.com/en-gb/subject-catalog/p/introduction-to-environmental-engineering-and-science-pearson-new-international-edition/P200000004949/9781292025759)*.



