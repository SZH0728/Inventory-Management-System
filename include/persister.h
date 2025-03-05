
#ifndef PERSISTER_H
#define PERSISTER_H

#include "datatype.h"
#include "storage.h"

#include <list>

class Persist: public WriteLogic, public ReadLogic{
private:
    DataFile data_file;
    OperationFile operation_file;
    int max_log_row;
    bool has_closed = false;

    bool write_operation(const Item& item, const std::string& keyword);
public:
    Persist(const std::string &data_file_path, const std::string &operation_file_path, int max_log_row);
    ~Persist();

    std::list<Item> select();
    bool insert(const Item& item);
    bool update(const Item& item);
    bool del(int index);

    int flush();
    bool close();
};

#endif //PERSISTER_H
