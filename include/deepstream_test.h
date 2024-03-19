#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>
#include "gstnvdsmeta.h"
#include <vector>
#include <opencv2/opencv.hpp>



class DeepStream
{
    public:
        // DeepStream();
        // ~DeepStream();
        
        static int deepstream_func();
        
        static std::vector<guint64> g_Transverse_cracks_ids;
        static std::vector<guint64> g_Linear_cracks_ids;
        static std::vector<guint64> g_Pit_slot_ids;
        static std::vector<guint64> g_Crack_ids;
        static std::vector<guint64> g_Mark_ids;
        static std::vector<guint64> g_Tyreskidmark_ids;
        static std::vector<guint64> g_patched_ids;
        static std::vector<guint64> g_manhole_ids;
        static std::vector<guint64> g_joint_ids;
        static std::vector<guint64> g_trash_ids;
        static std::vector<guint64> g_puddle_ids;
        static std::vector<guint64> g_repaired_crack_ids;
        static std::vector<guint64> g_animal_ids;
        static std::vector<guint64> g_shoes_ids;
        static std::vector<guint64> g_bumps_ids;
        static std::vector<guint64> g_shadow_ids;
        static std::vector<guint64> g_incomplete_mark_ids;
        static std::vector<guint64> g_crushing_plate_ids;
        static std::vector<guint64> g_faulting_ids;

        std::map <uint32_t,int> id_to_number_map_Transverse_cracks;
        std::map <uint32_t,int> id_to_number_map_Linear_cracks;
        std::map <uint32_t,int> id_to_number_map_Pit_slot;
        std::map <uint32_t,int> id_to_number_map_Crack;
        std::map <uint32_t,int> id_to_number_map_Mark;
        std::map <uint32_t,int> id_to_number_map_Tyreskidmark;
        std::map <uint32_t,int> id_to_number_map_patched;
        std::map <uint32_t,int> id_to_number_map_manhole;
        std::map <uint32_t,int> id_to_number_map_joint;
        std::map <uint32_t,int> id_to_number_map_trash;
        std::map <uint32_t,int> id_to_number_map_puddle;
        std::map <uint32_t,int> id_to_number_map_repaired_crack;
        std::map <uint32_t,int> id_to_number_map_animal;
        std::map <uint32_t,int> id_to_number_map_shoes;
        std::map <uint32_t,int> id_to_number_map_bumps;
        std::map <uint32_t,int> id_to_number_map_shadow;
        std::map <uint32_t,int> id_to_number_map_incomplete_mark;
        std::map <uint32_t,int> id_to_number_map_crushing_plate;
        std::map <uint32_t,int> id_to_number_map_faulting;

        static void data(std::string data1,std::string data2);

        static void updateDataPeriodically(int interval);

    private:
        

};


