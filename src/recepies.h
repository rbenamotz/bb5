#ifndef _RECEPIES_H_
#define _RECEPIES_H_

#define SOURCE_ID_VOKDA 0
#define SOURCE_ID_GINGER_BEER 1
#define SOURCE_ID_LIME_JUICE 2
#define SOURCE_ID_BLOODY_MARRY_MIX  3
#define SOURCE_ID_ORANGE_JUICE  4
#define SOURCE_ID_CRANBERRY_JUICE  5
#define INGREDIENTS {"Vodka" , "Ginger beer" , "Lime juice" , "Bloody Marry Mix", "Orange juice", "Cranberry juice" }


#define TOTAL_RECEPIES 2
#define RECEPIE_ID_MOSCOW_MULE 0
#define RECEPIE_ID_BLOODY_MARRY 1


struct preparationStep {
    int ingredient;
    int milliliters;
};


typedef struct { 
    int id;
    char name[50];
    preparationStep steps[5];
    int totalSteps;    
} recepie;


void setupRecepies();

recepie getRecepieById(int id);

const char* getIngredientName(int id);

#endif