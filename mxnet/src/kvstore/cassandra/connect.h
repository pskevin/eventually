#include <stdio.h>
#include <iostream>

#include <cassandra.h>

using namespace std;

CASS_RESULT cass_connect()
{
    /* Setup and connect to cluster */
    CassFuture *connect_future = NULL;
    CASS_RESULT res = { CASS_STATUS::SUCCESS,""};

    /* Provide the cluster object as configuration to connect the session */
    connect_future = cass_session_connect(cass_session, cass_cluster);

    /* This operation will block until the result is ready */
    CassError rc = cass_future_error_code(connect_future);

    if (rc != CASS_OK)
    {
        /* Display connection error message */
        const char *message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        // final_message = "\n[CASS] Connection error - " + *message;
        res = {CASS_STATUS::FAIL, "\n[CASS] Connection error - " + *message};
        
    }

    cass_future_free(connect_future);

    return res;
}

void cass_cleanup() {
    cass_cluster_free(cass_cluster);
    cass_session_free(cass_session);
}