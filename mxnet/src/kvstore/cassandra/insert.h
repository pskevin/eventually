#include<iostream>

// #include "ps/ps.h"
#include <mxnet/kvstore.h>


#include <cassandra.h>


// using namespace ps;
using namespace mxnet;
using namespace std;

CASS_RESULT cass_insert(int key,const NDArray *value) {
    CassUuidGen *uuid_gen = cass_uuid_gen_new();
    CassUuid uuid;
    CassError rc;
    CASS_RESULT res = { CASS_STATUS::SUCCESS,""};
    NDArray v = *value;

    /* There are two bind variables in the query string */
    CassStatement *statement = cass_statement_new("INSERT INTO mxnet_simple.kvstore (key, vclock, value) VALUES (?, ?, ?)", 3);

    /* Bind the values using the indices of the bind variables */

    size_t size = v.shape().Size();
    const int dtype = v.dtype();
    const int num_bytes = mshadow::mshadow_sizeof(dtype);
    const auto* data = static_cast<unsigned char*>(v.data().dptr_);


    /* Generate a version 1 UUID */
    cass_uuid_gen_time(uuid_gen, &uuid);

    rc = cass_statement_bind_int32(statement, 0, key);
    cerr << "\nBind: "<< cass_error_desc(rc);
    rc = cass_statement_bind_uuid(statement, 1, uuid);
    cerr << "\nBind: "<< cass_error_desc(rc);
    rc = cass_statement_bind_bytes(statement, 2, data, size);

    CassFuture *query_future = cass_session_execute(cass_session, statement);

    /* This will block until the query has finished */
    rc = cass_future_error_code(query_future);
  
    /* Statement objects can be freed immediately after being executed */
    cass_uuid_gen_free(uuid_gen);
    cass_statement_free(statement);

    cerr << "\nQuery result: " << cass_error_desc(rc);

    cass_future_free(query_future);
}
