#include "recepies.h"
#include "common.h"

String ingredients[] = INGREDIENTS;

recepie cookBook[] = {
    {RECEPIE_ID_MOSCOW_MULE, "Moscow Mule",{{SOURCE_ID_LIME_JUICE, 15}, {SOURCE_ID_VOKDA, 60},  {SOURCE_ID_GINGER_BEER,180}},3},
    {RECEPIE_ID_BLOODY_MARRY, "Bloody Marry", {{SOURCE_ID_VOKDA, 60},  {SOURCE_ID_BLOODY_MARRY_MIX,180}},2},
    {RECEPIE_ID_VODKA, "Vodka", {{SOURCE_ID_VOKDA, 60}},1},
    {RECEPIE_ID_VIRGIN_MARRY,"Virgin Marry",{{SOURCE_ID_BLOODY_MARRY_MIX,180}},1},
    {RECEPIE_ID_VIRGIN_MULE,"Virgin Mule",{{SOURCE_ID_LIME_JUICE, 15},  {SOURCE_ID_GINGER_BEER,200}},2}
};



void setupRecepies() {
}


const char* getIngredientName(int id) {
    return ingredients[id].c_str();
}

recepie getRecepieById(int id) {
    for (int i=0; i<TOTAL_RECEPIES; i++) {
        recepie r = cookBook[i];
        if (r.id==id) {
            return r;
        }
    }
    write_to_log("Recepie %d not found in cookbook",id);
    recepie empty;
    empty.id = -1;
    empty.totalSteps = 0;
    return empty;
}