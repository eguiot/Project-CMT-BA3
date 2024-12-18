#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct season {      
    char* name; 
    double T; // Temperature [degre celcius]
    double  DOs; // Saturated value of dissolved oxygen [mg/L]
    double DOr; // Dissolved oxygen in the river just upstream of the wastewater discharge point [mg/L]
    double H; // Average stream depth [m]    
    double river_area; //[m2]
};


struct River {
    double L0 ;  //Ultimate biochemical oxygen-demand of the mixture of streamwater and wastewater [mg/L]
    double kd;  // Deoxygenation constant [1/time]
    double kr;  // Reaeration constant [1/time]
    double D0;  // Initial oxygen deficit of DO in water at the temperature of the river [mg/L]
    double u;  // Average stream velocity [m/s]
    double* Qr_values;  // Volumetric flow rate of the river just upstream of the discharge point [m3/s]
    double Lr;  // Ultimate biochemical oxygen demand of the river just upstream of the point of discharge [mg/L]

    struct season* season;
};

struct waste{
    double Qw;  // Volumetric flow of wastewater [m3/s]
    double Lw;  // Ultimate biochemical oxygen demand of the wastewater [mg/L]
    double DOw; // Dissolved oxygen in the wastewater [mg/L]
};

void init_river(double Lr,double Qr_values[], struct season* season, struct River* river){    // Function to initialize the parameter of te river
    river->Qr_values = Qr_values;
    river->Lr=Lr;
    river->season = season;
}

void init_waste(double Qw, double Lw, double DOw, struct waste* waste){    // Function to initialize the parameters of wastes 
    waste->Qw = Qw;
    waste->Lw = Lw;
    waste->DOw = DOw;

}

int qr_data_import(char* data, int MAX_VALUES, int MAX_STEPS, double Qr[]){  // Reading of the data of the volumetric flow rate of the river just upstream of the discharge point

    char filepath[1024];  // Buffer to store the full path
    snprintf(filepath, sizeof(filepath), "Data/%s", data);  // Combine subfolder and file name
    
    FILE* file = fopen(filepath, "r");  // //We open and read the file that contains the data

    if (file == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    int QrIndex = 0;                  // Index to fill Qr
    char line[1024];                  // Buffer to read a line
    int row = 0;                      // Counter for file lines

    for (int start = 12960; start < 105120; start += 25920) {  // Main loop to browse indices i 
        rewind(file);  // Reset file pointer at start
        row = 0;

        while (row < start && fgets(line, sizeof(line), file)) {   // Advance to the first value wanted
            row++;
        }

        int count = 0; // Local counter for the number of values
        while (count < MAX_VALUES && fgets(line, sizeof(line), file)) {  // Reading of the desired number of lines
            if (QrIndex < MAX_VALUES * MAX_STEPS) {
               Qr[QrIndex] = atof(line); // Value storage
                QrIndex++;   // Incrementation of the index for the next location
                count++;        
            } else {
                printf("Error: Overflow of Qr's capacity\n");
                fclose(file);
                return 1;
            }
        }
    }
    fclose(file); // Closing of file
    return 0;
}

void def_speed(struct River* river, struct season* season, int t, int s){  // Calcutation of the average stream velocity
    river->u = river->Qr_values[t+s*8640]/ season->river_area;
}

void unpolluted_river(struct River* river, struct season* season){ // Completion of L0 and D0 for an unpolluted river (Qw=0)
    river->L0 = river->Lr;
    river->D0 = season->DOs - season->DOr;
}


void defL0 (struct waste* w, struct River* river, int t, int s){     //Calculation of the ultimate biochemical oxygen-demand of the mixture of streamwater and wastewater                                  
    river->L0 = (w->Qw * w->Lw+ river->Qr_values[t+s*8640] * river->Lr)/(w->Qw+ river->Qr_values[t+s*8640]);
}

void defD0 (struct waste* w, struct season* season, struct River* river, int t, int s){    // Calculation of the initial oxygen deficit of dissolved oxygen in water 
    river->D0= season->DOs - (w->Qw * w->DOw + river->Qr_values[t+s*8640] *season->DOr)/ (w->Qw+river->Qr_values[t+s*8640]);   
}

void defkd (struct season* season, struct River* river) {   // Calculation of the constant of deoxygenation
    double kd_day = 0.3 * pow(1.047, (season->T - 20)); // Constant of deoxygenation per day
    double kd_5min= kd_day/288; // Conversion of the constant of deoxygenation per day to the constant of deoxygenation per 5 minutes
    river->kd=kd_5min;
}

void defkr (struct River* river, struct season* season){      // Calculation of the constant of reaeration
    double kr_day_20= (3.9 * pow(river->u, 0.5)) / pow(season->H, 1.5); // Constant of reaeration per day at 20°C
    double kr_day= kr_day_20*pow(1.024, season->T-20); // Constant of reaeration per day at the river's temperature
    double kr_5min= kr_day / 288; // Conversion of the constant of reaeration per day to the constant of reaeration per 5 minutes
    river->kr= kr_5min;
}

double defDO(struct River* river, struct season* season, int t){     //Calculation of the dissolved oxygen in water 
    double D = ((river->kd * river-> L0)/(river->kr - river->kd)*(exp(-(river->kd *t))- exp(-(river->kr *t)))) + river->D0 * exp(-(river->kr*t));
    double DO = season->DOs -D;
    return DO;

}

int main(){     

    char filepath[1024];  // Buffer to store the full path
    snprintf(filepath, sizeof(filepath), "Results/%s", "parameters.txt"); // Combine subfolder and file name

    FILE* parameters= fopen(filepath, "w");                //We open and complete the file that will contain the dissolve oxygen values
    if (parameters == NULL) {
        printf("Error: File not found\n");
        return 1;
    }
                     
    double Qr[8640 * 4]; // Creation of the list to store values of qr
    qr_data_import("Flow_river.csv",8640, 4, Qr); // Importation of data

    struct River Rhone1[4]; // Creation of four struc River, one per season 

 // Scenario 1: Discharge of waste without extreme value

    fprintf(parameters, "Scenario 1: Discharge of waste without extreme value \n\n");

    float DO_value1[4][8640]; // Creation of the array containing the results 
    
    struct season seasons1[4] = {                       // Completion of temperature, dissolved oxygen at saturation, initial dissolved oxygen of the river, average stream depth, and river area for each seasons
        {"Winter", 5.31 , 12.84, 12.47, 4.919, 564.2},
        {"Spring", 8.95, 11.96 , 11.5, 5.594, 642.74},
        {"Summer",9.46, 11.08, 11.0, 5.894, 677.11},
        {"Autumn", 6.73, 11.9, 11.8, 5.344, 613.84}
    };

    struct waste discharge;                            // Initialization of waste discharge
    init_waste( 20, 230 ,2.0 , &discharge);


    for (int s=0; s<4; s++){                              // Initialization of the season's depending parameters of the river
        init_river(6, Qr ,&seasons1[s], &Rhone1[s]);
        defkd(&seasons1[s], &Rhone1[s]);
        fprintf(parameters," %s:\n  Temperature: %.2f °C\n  DOs: %.2f mg/L\n  DOr: %.2f mg/L\n  H: %.3f m\n  River area: %.2f m2\n\n",seasons1[s].name,seasons1[s].T,seasons1[s].DOs,seasons1[s].DOr,seasons1[s].H,seasons1[s].river_area);

        for (int t=0; t<8640; t++){                            //We complete the parameters of the river for each five minutes interval
            def_speed(&Rhone1[s], &seasons1[s], t, s);
            defkr(&Rhone1[s], &seasons1[s]);
            if(t<1728){                                      // The river remains unpolluted during the first six days
                unpolluted_river(&Rhone1[s], &seasons1[s]);
            }
            else{
                defL0(&discharge, &Rhone1[s], t,s);                     
                defD0(&discharge, &seasons1[s], &Rhone1[s],t,s);
            }
            DO_value1[s][t]= defDO(&Rhone1[s], &seasons1[s],t);   //We add the dissolved oxygen values to the array for each five minutes interval
    } 
    }
    fprintf(parameters," Lr: %.2f mg/L\n\n",Rhone1[0].Lr);
    fprintf(parameters," Waste parameters:\n  Qw: %.2f m3/s\n  Lw: %.2f mg/L\n  DOw: %.2f mg/L\n\n",discharge.Qw,discharge.Lw,discharge.DOw);
    
    char filepath1[1024];  // Buffer to store the full path
    snprintf(filepath1, sizeof(filepath1), "Internal/%s", "DO_value1.csv"); // Combine subfolder and file name

    FILE* results1= fopen(filepath1, "w");                //We open and complete the file that will contain the dissolve oxygen values
    if (results1 == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    for(int j=0; j<8640; j++){
        fprintf(results1, "%.2f, %.2f, %.2f, %.2f", DO_value1[0][j], DO_value1[1][j], DO_value1[2][j], DO_value1[3][j]);   

        fprintf(results1, "\n");
        }
    fclose(results1);

 // Scénario 2: Unpolluted river 

    fprintf(parameters, "Scénario 2: Unpolluted river \n\n The parameter changed is: \n\n");

    float DO_value2[4][8640];
    struct River Rhone2[4];

    struct waste unpolluted;                            // Initialization of waste discharge
    init_waste( 0, 230 ,2.0 , &unpolluted);

    for (int s=0; s<4; s++){                              
        init_river(6, Qr,&seasons1[s], &Rhone2[s]);
        defkd(&seasons1[s], &Rhone2[s]);

        for (int t=0; t<8640; t++){                           
            def_speed(&Rhone2[s], &seasons1[s], t, s);
            defkr(&Rhone2[s], &seasons1[s]);
            unpolluted_river(&Rhone2[s], &seasons1[s]);
            DO_value2[s][t]= defDO(&Rhone2[s], &seasons1[s],t);   
    } 
    }
    fprintf(parameters,"  Qw: %.2f m3/s\n\n",unpolluted.Qw);

    char filepath2[1024];  // Buffer to store the full path
    snprintf(filepath2, sizeof(filepath2), "Internal/%s", "DO_value2.csv"); // Combine subfolder and file name

    FILE* results2 = fopen(filepath2, "w");              
    if (results2 == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    for(int j=0; j<8640; j++){
        fprintf(results2, "%.2f, %.2f, %.2f, %.2f", DO_value2[0][j], DO_value2[1][j], DO_value2[2][j], DO_value2[3][j]);   

        fprintf(results2, "\n");
        }
    fclose(results2);
    

 //Scénario 3: Extreme temperature

    fprintf(parameters, "Scénario 3: Extreme temperature(Addition of 20°C)\n\n The parameters changed are: \n\n");


    float DO_value3[4][8640];
    struct River Rhone3[4];

    struct season seasons2[4] = {                         // Addition of 20°C to water temperature for each seasons
        {"Winter", 25.31 , 12.84, 12.47, 4.919, 564.2},
        {"Spring", 28.95, 11.96 , 11.5, 5.594, 642.74},
        {"Summer",29.46, 11.08, 11.0, 5.894, 677.11},
        {"Autumn", 26.73, 11.9, 11.8, 5.344, 613.84}
    };

    for (int s=0; s<4; s++){                              
        init_river(6, Qr ,&seasons2[s], &Rhone3[s]);
        defkd(&seasons2[s], &Rhone3[s]);
        fprintf(parameters," %s:\n  Temperature: %.2f °C\n\n",seasons2[s].name,seasons2[s].T);

        for (int t=0; t<8640; t++){                            
            def_speed(&Rhone3[s], &seasons2[s], t, s);
            defkr(&Rhone3[s], &seasons2[s]);
            if(t<1728){
                unpolluted_river(&Rhone3[s], &seasons2[s]);
            }
            else{
                defL0(&discharge, &Rhone3[s], t,s);                     
                defD0(&discharge, &seasons2[s], &Rhone3[s],t,s);
            }
            DO_value3[s][t]= defDO(&Rhone3[s], &seasons2[s],t);   
    } 
    }

    char filepath3[1024];  // Buffer to store the full path
    snprintf(filepath3, sizeof(filepath3), "Internal/%s", "DO_value3.csv"); // Combine subfolder and file name

    FILE* results3 = fopen(filepath3, "w");              
    if (results3 == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    for(int j=0; j<8640; j++){
        fprintf(results3, "%.2f, %.2f, %.2f, %.2f", DO_value3[0][j], DO_value3[1][j], DO_value3[2][j], DO_value3[3][j]);   

        fprintf(results3, "\n");
        }
    fclose(results3);

 // Scénario 4: Constant flow of the river for each season
    fprintf(parameters, "Scénario 4: Constant flow of the river for each season \n\n The parameter changed is: \n\n");

    double Qr2[8640 * 4];        // Creation of a new list to store qr values
    for(int i=0; i<8640*4 ; i++) Qr2[i]=300; // Implementation of the qr list with a constant value of 300 m3/s

    fprintf(parameters,"  Qr: %.2f m3/s\n\n", Qr2[0]);

    float DO_value4[4][8640];
    struct River Rhone4[4];

    for (int s=0; s<4; s++){                              
        init_river(6, Qr2,&seasons1[s], &Rhone4[s]);
        defkd(&seasons1[s], &Rhone4[s]);

        struct waste discharge;                           
        init_waste( 20, 230 ,2.0 , &discharge);


        for (int t=0; t<8640; t++){                           
            def_speed(&Rhone4[s], &seasons1[s], t, s);
            defkr(&Rhone4[s], &seasons1[s]);
            if(t<1728){
                unpolluted_river(&Rhone4[s], &seasons1[s]);
            }
            else{
                defL0(&discharge, &Rhone4[s], t,s);                     
                defD0(&discharge, &seasons1[s], &Rhone3[s],t,s);
            }
            DO_value4[s][t]= defDO(&Rhone4[s], &seasons1[s],t);   
    } 
    }

    char filepath4[1024];  // Buffer to store the full path
    snprintf(filepath4, sizeof(filepath4), "Internal/%s", "DO_value4.csv"); // Combine subfolder and file name

    FILE* results4= fopen(filepath4, "w");                
    if (results4 == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    for(int j=0; j<8640; j++){
        fprintf(results4, "%.2f, %.2f, %.2f, %.2f", DO_value4[0][j], DO_value4[1][j], DO_value4[2][j], DO_value4[3][j]);   

        fprintf(results4, "\n");
        }
    fclose(results4);

 // Scénario 5: High quantity of waste's discharge
    
    fprintf(parameters, "Scénario 5: High quantity of waste's discharge \n\n The parameter changed is:\n\n");

    struct River Rhone5[4];
    float DO_value5[4][8640];

    struct waste discharge2;                           
    init_waste( 70, 230 ,2.0 , &discharge2);
    fprintf(parameters,"  Qw: %.2f m3/s\n\n", discharge2.Qw);

    for (int s=0; s<4; s++){                             
        init_river(6, Qr ,&seasons1[s], &Rhone5[s]);
        defkd(&seasons1[s], &Rhone5[s]);

        for (int t=0; t<8640; t++){                            
            def_speed(&Rhone5[s], &seasons1[s], t, s);
            defkr(&Rhone5[s], &seasons1[s]);
            if(t<1728){
                unpolluted_river(&Rhone5[s], &seasons1[s]);
            }
            else{
                defL0(&discharge2, &Rhone5[s], t,s);                     
                defD0(&discharge2, &seasons1[s], &Rhone5[s],t,s);
            }
            DO_value5[s][t]= defDO(&Rhone5[s], &seasons1[s],t);   
    } 
    }

    char filepath5[1024];  // Buffer to store the full path
    snprintf(filepath5, sizeof(filepath5), "Internal/%s", "DO_value5.csv"); // Combine subfolder and file name

    FILE* results5= fopen(filepath5, "w");               
    if (results5 == NULL) {
        printf("Error: File not found\n");
        return 1;
    }

    for(int j=0; j<8640; j++){
        fprintf(results5, "%.2f, %.2f, %.2f, %.2f", DO_value5[0][j], DO_value5[1][j], DO_value5[2][j], DO_value5[3][j]);   

        fprintf(results5, "\n");
        }
    fclose(results5);


    return 0;

}
