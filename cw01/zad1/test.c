#include <stdio.h>
#include "library.h"

int main() {

    char **ar = createArrayDynamic(10, 5);
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 5; j++) {
            ar[i][j] = 'a';
        }
        ar[i][4] = '\0';
    }

    ar = addBlockDynamic(3, 10, 5, "Hel", ar);
//addBlockDynamic(3,11,5,"Hellloooo",ar);
    ar = addBlockDynamic(8, 11, 5, "AA", ar);
    ar = addBlockDynamic(8, 11, 5, "AA", ar);
    ar = addBlockDynamic(1, 12, 5, "AA", ar);
    ar = deleteBlockDynamic(1, 13, ar);
    printf("%s\n", ar[10]);
    insertBlockDynamic(4, 12, 5, "Mama", ar);
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%d %d %c \n", i, j, ar[i][j]);
        }
    }
    nearestDynamicSum(8, 12, ar);
    deleteArrayDynamic(ar, 12);

    createArrayStatic();
    for (int i = 0; i < NoBlocks; i++) {
        for (int j = 0; j < SoBlocks; j++) {
            StaticArray[i][j] = 'b';
            //printf("%d %d %c \n",i, j, StaticArray[i][j]);
        }
        StaticArray[i][9] = '\0';
    }

    insertBlockStatic(90, "Hello...");
    insertBlockStatic(91, "World!");
    printf("%s\n%s", StaticArray[90], StaticArray[91]);
    insertBlockStatic(0, "Halo");
    insertBlockStatic(1, "Halo");
    insertBlockStatic(999, "Halo");
    printf("%d\n", nearestStaticSum(999));
    deleteBlockStatic(90);
    deleteBlockStatic(91);
    printf("%s\n%s", StaticArray[90], StaticArray[91]);

    return 0;
}
