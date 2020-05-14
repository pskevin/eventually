#include <vector>
#include <iostream>

#include <cassandra.h>

using namespace std;

CassCollection *to_cass_list(vector<int> &v)
{

    CassCollection *list = cass_collection_new(CASS_COLLECTION_TYPE_LIST, v.size());
    CassError rc;

    cout << "\n SIZE - " << v.size() << "\n"
         << endl;
    for (int i=0; i < v.size(); i++)
    {
        rc = cass_collection_append_int32(list, v[i]);
        printf("Convert: %s\n", cass_error_desc(rc));
    }

    return list;
}