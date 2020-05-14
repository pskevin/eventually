#include <stdio.h>
#include <stdlib.h>

#include <iostream>

// #include "ps/ps.h"
#include <mxnet/kvstore.h>

#include <cassandra.h>

// using namespace ps;
using namespace mxnet;
using namespace std;

NDArray* cass_select(int key)
{
    CassError rc;
    NDArray *res = nullptr;

    /* There are two bind variables in the query string */
    CassStatement *statement = cass_statement_new("SELECT * FROM mxnet_simple.kvstore WHERE key = ? GROUP BY key ORDER BY vclock DESC", 1);
    // CassStatement *statement = cass_statement_new("SELECT * FROM mxnet_simple.kvstore WHERE key = ? ", 1);


    /* Bind the values using the indices of the bind variables */
    cout << "\nPrinting key in select " << key << endl;
    rc = cass_statement_bind_int32(statement, 0, key);
    cerr << "\nBind: " << cass_error_desc(rc);

    CassFuture *query_future = cass_session_execute(cass_session, statement);

    /* This will also block until the query returns */
    const CassResult *result = cass_future_get_result(query_future);

    cass_statement_free(statement);

    /* If there was an error then the result won't be available */
    if (result == NULL)
    {

        /* Handle error */
        /* This will block until the query has finished */
        rc = cass_future_error_code(query_future);

        cass_future_free(query_future);
        cass_statement_free(statement);

        cerr << "\nFailed select - " << cass_error_desc(rc);
        return res;
    }

    /* The future can be freed immediately after getting the result object */
    cass_future_free(query_future);

    /* This can be used to retrieve the first row of the result */
    const CassRow *row = cass_result_first_row(result);

    /* Now we can retrieve the column values from the row */
    const cass_byte_t *blob;
    size_t blob_length;
    /* Get the column value of "key" by name */
    if (cass_value_get_bytes(cass_row_get_column_by_name(row, "value"), &blob, &blob_length) == CASS_OK)
    {
        cout << "\n Blob - " << *blob << "\t" << atof(reinterpret_cast<const char *>(blob)) << "\n" << endl;

        // size_t ds[] = {blob_length / sizeof(float)};
        // mxnet::TShape dshape(ds, ds + 1);
        // const TBlob recv_blob;
        // recv_blob = TBlob(reinterpret_cast<const unsigned char *>(blob), dshape, cpu::kDevMask);
        // NDArray recved = NDArray(recv_blob, 0);
        // if (outSize == sizeof(YourStruct))
        // {
        //     const YourStruct *yourStruct = reinterpret_cast<const YourStruct *>(outPtr);
        // }

        // size_t size = v.shape().Size();
        // const int dtype = v.dtype();
        // const int num_bytes = mshadow::mshadow_sizeof(dtype);
        // const auto* data = static_cast<unsigned char*>(v.data().dptr_);
        /* Statement objects can be freed immediately after being executed */
    }

    // cass_int32_t value;
    // /* Get the column value of "value" by name */
    // cass_value_get_int32(cass_row_get_column_by_name(row, "value"), &value);

    /* This will free the result as well as the string pointed to by 'key' */
    cass_result_free(result);
    cerr << "\nAt the end of select: " << cass_error_desc(rc);
    return res;
}
