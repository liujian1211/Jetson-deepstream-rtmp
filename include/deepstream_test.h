#ifndef DeepStream_H
#define DeepStream_H

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include "gstnvdsmeta.h"
#include <vector>
#include <opencv2/opencv.hpp>

class DeepStream
{
    public:
        DeepStream();
        ~DeepStream();
        int deepstream_func1();
        int deepstream_func();

        std::vector<guint64> g_person_ids;
        std::vector<guint64> g_chair_ids;

        std::map <uint32_t,int> id_to_number_map_person;
        std::map <uint32_t,int> id_to_number_map_chair;

    // private:

};

#endif