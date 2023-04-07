#include "proc_stat_database.h"

static struct
{
    unsigned cores_no;

} usefulData = {0};

void DB_SetCoreNo(unsigned core_no)
{
    if(core_no < 1)
        return;
    
    usefulData.cores_no = core_no;
}

unsigned DB_GetCoreNo()
{
    return usefulData.cores_no;
}