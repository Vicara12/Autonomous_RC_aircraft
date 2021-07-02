#include "filter.h"


struct ListItem {

    double value;

    struct ListItem *next_size, *next_recent;
    struct ListItem *prev_size, *prev_recent;
};


Filter::Filter (unsigned median_factor, unsigned mean_factor) :
    lenght(median_factor < 2 ? 0 : median_factor),
    current_lenght(0),
    mean_qty(mean_factor),
    first_size_list(nullptr),
    first_recent_list(nullptr),
    last_size_list(nullptr),
    last_recent_list(nullptr),
    median_ptr(nullptr),
    mean_value(0)
{
    //
}


Filter::~Filter ()
{
    struct ListItem *list_ptr, *buffer_ptr;

    list_ptr = (ListItem*)first_size_list;

    while (list_ptr != nullptr)
    {
        buffer_ptr = list_ptr;
        list_ptr = list_ptr->next_size;
        delete buffer_ptr;
    }
}


double Filter::filter (double value)
{
    // if no median filter is desired return value
    if (lenght == 0 and mean_qty == 0)
    {
        return value;
    }

    // if only mean filter is desired
    if (lenght == 0)
    {
        mean_value = (mean_value*(mean_qty-1) + value)/mean_qty;
        return mean_value;
    }

    // median filter

    // first recent list holds the oldest element and first size list the greatest

    // add new element
    struct ListItem *new_item = new struct ListItem;
    new_item->value = value;
    new_item->next_recent = nullptr;

    if (current_lenght == 0)
    {
        first_size_list = new_item;
        first_recent_list = new_item;
        last_size_list = new_item;
        last_recent_list = new_item;
        median_ptr = new_item;
        median_pos = 0;

        new_item->prev_recent = nullptr;
        new_item->next_size = nullptr;
        new_item->prev_size = nullptr;
    }
    else
    {
        last_recent_list->next_recent = new_item;
        new_item->prev_recent = last_recent_list;
        last_recent_list = new_item;

        struct ListItem *ptr;

        if (value > median_ptr->value)
        {
            ptr = first_size_list;

            median_pos++;

            while (ptr->value > value)
                ptr = ptr->next_size;
                        
            new_item->prev_size = ptr->prev_size;
            new_item->next_size = ptr;

            if (first_size_list == ptr)
                first_size_list = new_item;
        }
        else
        {
            ptr = last_size_list;

            while (ptr->value < value)
                ptr = ptr->prev_size;
            
            new_item->prev_size = ptr;
            new_item->next_size = ptr->next_size;

            if (last_size_list == ptr)
                last_size_list = new_item;
        }

        if (new_item->next_size != nullptr)
            new_item->next_size->prev_size = new_item;
        if (new_item->prev_size != nullptr)
            new_item->prev_size->next_size = new_item;
    }

    current_lenght++;

    // remove oldest element if needed
    if (current_lenght > lenght)
    {
        if (first_recent_list->value > median_ptr->value)
            median_pos--;
        
        // hiper mega edge case: if there are several numbers with the same value
        // as the one considered as the median, deletion can cause bugs as median
        // pointer displacement from the median position
        if (first_recent_list->value == median_ptr->value and first_recent_list != median_ptr)
        {
            struct ListItem *ptr = median_ptr;
            while (ptr->value == median_ptr->value)
            {
                if (ptr == first_recent_list)
                {
                    median_pos--;
                    break;
                }
                ptr = ptr->prev_size;
            }
        }
        
        if (first_recent_list == median_ptr)
        {
            median_ptr = first_recent_list->next_size;
        }

        if (first_recent_list == first_size_list)
        {
            first_size_list = first_size_list->next_size;
        }

        if (first_recent_list == last_size_list)
        {
            last_size_list = last_size_list->prev_size;
        }

        first_recent_list->next_recent->prev_recent = nullptr;
        if (first_recent_list->prev_size != nullptr)
            first_recent_list->prev_size->next_size = first_recent_list->next_size;
        if (first_recent_list->next_size != nullptr)
            first_recent_list->next_size->prev_size = first_recent_list->prev_size;
    
        struct ListItem *buff_ptr = first_recent_list;
        first_recent_list = first_recent_list->next_recent;
        delete buff_ptr;

        current_lenght--;
    }

    // move median pointer
    if (median_pos < (current_lenght-1)/2)
    {
        median_ptr = median_ptr->next_size;
        median_pos++;
    }
    else if (median_pos > (current_lenght-1)/2)
    {
        median_ptr = median_ptr->prev_size;
        median_pos--;
    }

    // apply mean filter if needed
    if (mean_qty != 0)
        mean_value = (mean_value*(mean_qty-1) + median_ptr->value)/mean_qty;
    else
        mean_value = median_ptr->value;
    
    return mean_value;
}