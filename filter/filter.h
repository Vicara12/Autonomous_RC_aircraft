#ifndef FILTER_H_
#define FILTER_H_

struct ListItem;

class Filter
{
public:

    Filter (unsigned median_factor, unsigned mean_factor);
    ~Filter ();

    double filter (double value);

private:

    int lenght, current_lenght;
    int mean_qty;
    struct ListItem *first_size_list, *first_recent_list;
    struct ListItem *last_size_list, *last_recent_list;
    struct ListItem *median_ptr;
    int median_pos;
    double mean_value;
};


#endif