/* 
// This file is subject to the terms and conditions defined in
// file 'LICENSE.md', which is part of this source code package.
*/

#include "staff.h"

const char* staffStatusToString(StaffStatus_e status){
    switch(status){
        case STAFF_NOM: return "Nominal"; break;
        case STAFF_ERR: return "Error - Generic"; break;
        case STAFF_ERR_NOMEM: return "Error - No Memory"; break;
        case STAFF_ERR_OUTBOUNDS: return "Error - Out of Bounds"; break;
        default:
            return "Unknown";
            break;
    }
}
