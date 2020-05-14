#include <mxnet/kvstore.h>

#include <cassandra.h>

extern CassCluster *cass_cluster;
extern CassSession *cass_session;

enum CASS_STATUS {
    SUCCESS = 0,
    FAIL = 1
};

struct CASS_RESULT {
    CASS_STATUS status;
    const char *message;
};

// CassCollection *to_cass_list(NDArray *v)
// {

//     CassCollection *list = cass_collection_new(CASS_COLLECTION_TYPE_LIST, v.size());
//     CassError rc;

//     cout << "\n SIZE - " << v.size() << "\n"
//          << endl;
//     for (int i=0; i < v.size(); i++)
//     {
//         rc = cass_collection_append_int32(list, v[i]);
//         printf("Convert: %s\n", cass_error_desc(rc));
//     }

//     return list;
// }