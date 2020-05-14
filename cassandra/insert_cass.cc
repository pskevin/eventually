/* Use "#include <dse.h>" when connecting to DataStax Enterpise */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>

#include <cassandra.h>

#include "connect_cass.h"
#include "cass_utils.h"

using namespace std;

void execute_demo(CassSession *session)
{
    CassFuture *close_future = NULL;

    /* Build statement and execute query */
    const char *query = "SELECT release_version FROM system.local";
    CassStatement *statement = cass_statement_new(query, 0);

    CassFuture *result_future = cass_session_execute(session, statement);

    if (cass_future_error_code(result_future) == CASS_OK)
    {
        /* Retrieve result set and get the first row */
        const CassResult *result = cass_future_get_result(result_future);
        const CassRow *row = cass_result_first_row(result);

        if (row)
        {
            const CassValue *value = cass_row_get_column_by_name(row, "release_version");

            const char *release_version;
            size_t release_version_length;
            cass_value_get_string(value, &release_version, &release_version_length);
            cout << "release_version: "
                 << release_version
                 << endl;
        }

        cass_result_free(result);
    }
    else
    {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(result_future, &message, &message_length);
        fprintf(stderr, "Unable to run query: '%.*s'\n", (int)message_length, message);
    }

    cass_statement_free(statement);
    cass_future_free(result_future);
}

void execute_paramertized_query(CassSession *session, vector<int> &key, vector<int> &value)
{
    CassUuidGen *uuid_gen = cass_uuid_gen_new();
    CassUuid uuid;
    CassError rc;

    /* There are two bind variables in the query string */
    CassStatement *statement = cass_statement_new("INSERT INTO mxnet_simple.kvstore (key, vclock, value) VALUES (?, ?, ?)", 3);

    cass_statement_set_tracing(statement, cass_true);
    /* Bind the values using the indices of the bind variables */
    CassCollection *k = to_cass_list(key);
    CassCollection *v = to_cass_list(value);

    /* Generate a version 1 UUID */
    cass_uuid_gen_time(uuid_gen, &uuid);

    rc = cass_statement_bind_collection(statement, 0, k);
    printf("Bind: %s\n", cass_error_desc(rc));

    rc = cass_statement_bind_uuid(statement, 1, uuid);
    printf("Bind: %s\n", cass_error_desc(rc));
    rc = cass_statement_bind_collection(statement, 2, v);
    printf("Bind: %s\n", cass_error_desc(rc));

    CassFuture *query_future = cass_session_execute(session, statement);

    cass_collection_free(k);
    cass_collection_free(v);

    /* This will block until the query has finished */
    rc = cass_future_error_code(query_future);
    cass_uuid_gen_free(uuid_gen);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    printf("Query result: %s\n", cass_error_desc(rc));

    cass_future_free(query_future);
}

int main(int argc, char *argv[])
{
    cass_log_set_level(CASS_LOG_INFO);
    /* Setup and connect to cluster */
    CassFuture *connect_future = NULL;
    CassCluster *cluster = cass_cluster_new();
    cass_cluster_set_protocol_version(cluster, CASS_PROTOCOL_VERSION_V4);
    CassSession *session = cass_session_new();

    char *hosts = "127.0.0.1";
    if (argc > 1)
    {
        hosts = argv[1];
    }

    /* Add contact points */
    cass_cluster_set_contact_points(cluster, hosts);

    /* Provide the cluster object as configuration to connect the session */
    connect_future = connect_to_cassandra(session, cluster);

    if (cass_future_error_code(connect_future) == CASS_OK)
    {
        // execute_demo(session);
        vector<int> k, v;
        CassCollection *cass_k, *cass_v;
        int random;

        cout << "\n Key -\t";
        for (int i = 1; i <= 5; i++)
        {
            random = rand() % 100;
            cout << random << "\t";
            k.push_back(random);
        }

        cout << "\n Value -\t";
        for (int i = 1; i <= 10; i++)
        {
            random = rand() % 100;
            cout << random << "\t";
            v.push_back(random);
        }
        cout << "\n"
             << endl;
        execute_paramertized_query(session, k, v);
    }
    else
    {
        /* Handle error */
        const char *message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        fprintf(stderr, "Unable to connect: '%.*s'\n", (int)message_length, message);
    }

    cass_future_free(connect_future);
    cass_cluster_free(cluster);
    cass_session_free(session);

    return 0;
}

void execute_query(CassSession *session)
{
    /* Create a statement with zero parameters */
    CassStatement *statement = cass_statement_new("INSERT INTO example (key, value) VALUES ('abc', 123)", 0);

    CassFuture *query_future = cass_session_execute(session, statement);

    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);

    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);

    printf("Query result: %s\n", cass_error_desc(rc));

    cass_future_free(query_future);
}

void handle_query_result(CassFuture *future)
{
    /* This will also block until the query returns */
    const CassResult *result = cass_future_get_result(future);

    /* If there was an error then the result won't be available */
    if (result == NULL)
    {

        /* Handle error */

        cass_future_free(future);
        return;
    }

    /* The future can be freed immediately after getting the result object */
    cass_future_free(future);

    /* This can be used to retrieve the first row of the result */
    const CassRow *row = cass_result_first_row(result);

    /* Now we can retrieve the column values from the row */
    const char *key;
    size_t key_length;
    /* Get the column value of "key" by name */
    cass_value_get_string(cass_row_get_column_by_name(row, "key"), &key, &key_length);

    cass_int32_t value;
    /* Get the column value of "value" by name */
    cass_value_get_int32(cass_row_get_column_by_name(row, "value"), &value);

    /* This will free the result as well as the string pointed to by 'key' */
    cass_result_free(result);
}
