#include "recepies.h"
#include "common.h"

recepie cookBook[] = {
    {RECEPIE_ID_MOSCOW_MULE, {{SOURCE_ID_LIME_JUICE, 15}, {SOURCE_ID_VOKDA, 60},  {SOURCE_ID_GINGER_BEER,180}},3},
    {RECEPIE_ID_BLOODY_MARRY, {{SOURCE_ID_VOKDA, 60},  {SOURCE_ID_BLOODY_MARRY_MIX,180}},2}
};



void setupRecepies() {
    // cookBook[1] = moscowMule();
    // cookBook[2] = bloodyMarry();
}

recepie getRecepieById(int id) {
    write_to_log("Looking for recepie %d in cookbook",id);
    for (int i=0; i<2; i++) {
        recepie r = cookBook[i];
        if (r.id==id) {
            write_to_log("Found - returning %d", r.id);
            return r;
        }
    }
    write_to_log("NOT Found. Returning empty");
    recepie empty;
    empty.id = -1;
    empty.totalSteps = 0;
    return empty;
}