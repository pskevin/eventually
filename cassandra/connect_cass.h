#include <cassandra.h>
/* Use "#include <dse.h>" when connecting to DataStax Enterpise */
#include <stdio.h>

CassFuture* connect_to_cassandra(CassSession *session, CassCluster *cluster)
{
    /* Setup and connect to cluster */
    CassFuture *connect_future = NULL;

    /* Provide the cluster object as configuration to connect the session */
    connect_future = cass_session_connect(session, cluster);

    /* This operation will block until the result is ready */
    CassError rc = cass_future_error_code(connect_future);

    if (rc != CASS_OK)
    {
        /* Display connection error message */
        const char *message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        fprintf(stderr, "Connect error: '%.*s'\n", (int)message_length, message);
    }

    return connect_future;
}
