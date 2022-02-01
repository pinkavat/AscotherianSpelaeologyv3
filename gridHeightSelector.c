
#include "gridHeightSelector.h"
// gridHeightSelector.c
// See header for details

void gridHeightSelect(struct parcel *parcel, const struct recursorGridSignature *signature){
    for(int i = 0; i < signature->width * signature->height; i++){
        parcel->children[i].transform.z = -1;   //TODO stairwork
    }
}
