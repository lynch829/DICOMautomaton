//Structs.h.

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <list>
#include <set>
#include <map>
#include <tuple>
#include <array>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <initializer_list>

#include <experimental/optional>

#include "YgorMath.h"
#include "YgorImages.h"
#include "YgorPlot.h"
#include "YgorFilesDirs.h"

//This is a wrapper around the YgorMath.h class "contour_of_points." It holds an instance of a contour_of_points, but also provides some meta information
// which helps identify the origin, quality, and purpose of the data.
//
//This should be turned into an enum, I think. Or at least reordered numerically.
namespace Segmentations {
    //General orientations.
    const uint32_t left              = (uint32_t)(1) <<  0; //1;
    const uint32_t right             = (uint32_t)(1) <<  1; //2;
    const uint32_t front             = (uint32_t)(1) <<  2; //4;
    const uint32_t back              = (uint32_t)(1) <<  3; //8;
    const uint32_t top               = (uint32_t)(1) <<  4; //16; //aka "up"
    const uint32_t bottom            = (uint32_t)(1) <<  5; //32; //aka "down"
    const uint32_t other_orientation = (uint32_t)(1) <<  6; //64;
    const uint32_t inner             = (uint32_t)(1) << 20; //1048576;
    const uint32_t outer             = (uint32_t)(1) << 21; //2097152;

    //Placeholder orientations - probably not useful/meaningful except for temporary uses.
    const uint32_t negative          = (uint32_t)(1) <<  7; //128; //This contour was below (on the positive side of) the plane we split on.
    const uint32_t positive          = (uint32_t)(1) <<  8; //256; //This contour was above (on the negative side of) the plane we split on.

    //Splitting type.
    const uint32_t volume            = (uint32_t)(1) <<  9; //512;   //Splitting on an entire volume of contours.
    const uint32_t height            = (uint32_t)(1) << 10; //1024;  //Splitting on all the contours of a given type at the same height.
    const uint32_t contour           = (uint32_t)(1) << 11; //2048;  //Splitting on individual contours.
    const uint32_t other_split       = (uint32_t)(1) << 12; //4096;

    //Splitting direction.
    const uint32_t coronal           = (uint32_t)(1) << 13; //8192;
    const uint32_t transverse        = (uint32_t)(1) << 23; //8388608;

    const uint32_t medial            = (uint32_t)(1) << 14; //16384;
    const uint32_t sagittal          = (uint32_t)(1) << 15; //32768;
    const uint32_t lateral           = (uint32_t)(1) << 16; //65536;
    const uint32_t ant_post          = (uint32_t)(1) << 17; //131072;

    //Other things.
    const uint32_t ray_cast          = (uint32_t)(1) << 18; //262144;  //Default is planar split.
    const uint32_t misc_marker       = (uint32_t)(1) << 19; //524288;
    const uint32_t core_peel         = (uint32_t)(1) << 22; //4194304;
    //Note: std::numeric_limits<uint32_t>::max() == 4294967295.
};

std::string Segmentations_to_Words(const std::vector<uint32_t> &in);




class contours_with_meta : public contour_collection<double> {
//class contour_with_meta : public contour_of_points<double> {
    public:
        //This is the ROI number as it is defined in the DICOM file. There is no importance to this number. It just 
        // allows us to conveniently differentiate contours with a number.
        long int      ROI_number;

        //This is the minimum distance between adjacent contours. One should be ready to handle 0.0 or negatives! 
        double        Minimum_Separation; 

        //These are the raw names read out of the DICOM file. There is no danger having them as std::string, because
        // we can cast to a unicode-friendly format as needed (treating them as binary data).
        std::string   Raw_ROI_name;

        //Used to keep track of the various ways the data in this contour has been segmented. (Might tell us it is 
        // the medial, posterior portion of the original contour.)
        std::vector<uint32_t> Segmentation_History;

        //Constructors.
        contours_with_meta();
        contours_with_meta(const contours_with_meta &);
        contours_with_meta(const contour_collection<double> &);

        contours_with_meta & operator=(const contours_with_meta &rhs);

};

//This class is used to hold a collection of contour_with_meta instances. It basically is used to replace using a std::vector<contour_with_meta>. This is done
// because using std::vector's was leaking the storage format throughout the code. Everytime I wanted to iterate over the contours, I was required to break into
// a for loop and iterate over the contours until I found the particular ROI numbers (etc..) I was looking for. This class basically just tries to contain the 
// excessive amounts of duplicated code that were floating around prior to it existing.
class Contour_Data {
    public:
        std::list<contours_with_meta> ccs; //Contour collections.

        //Constructors.
        Contour_Data();
        Contour_Data(const Contour_Data &in);

        //Member functions.
        void operator = (const Contour_Data &rhs);

        void Plot(void) const;     //Spits out a default plot of the (entirety) of the data. Use the contour_of_points::Plot() method for individual contours.
    
        //Unique duplication (aka 'copy factory').
        std::unique_ptr<Contour_Data> Duplicate() const; 

        //--- Geometeric splitting. ---
        //For each distinct ROI number, we specify a single plane to split on.
        std::unique_ptr<Contour_Data> Split_Per_Volume_Along_Given_Plane_Unit_Normal(const vec3<double> &N) const; 
        std::unique_ptr<Contour_Data> Split_Per_Volume_Along_Coronal_Plane(void) const; 
        std::unique_ptr<Contour_Data> Split_Per_Volume_Along_Sagittal_Plane(void) const;

std::unique_ptr<Contour_Data> Split_Per_Volume_Along_Transverse_Plane(void) const;

        //For each distinct ROI number, distinct height, we specify a plane to split on.
//        std::unique_ptr<Contour_Data> Split_Per_Height_Along_Given_Plane_Unit_Normal(const vec3<double> &N) const;
//        std::unique_ptr<Contour_Data> Split_Per_Height_Along_Coronal_Plane(void) const; 
//        std::unique_ptr<Contour_Data> Split_Per_Height_Along_Sagittal_Plane(void) const;

        //For each distinct contour, we specify a plane to split on.
        std::unique_ptr<Contour_Data> Split_Per_Contour_Along_Given_Plane_Unit_Normal(const vec3<double> &N) const;
        std::unique_ptr<Contour_Data> Split_Per_Contour_Along_Coronal_Plane(void) const; 
        std::unique_ptr<Contour_Data> Split_Per_Contour_Along_Sagittal_Plane(void) const;
 
        //--- Ray-casting splitting. ---
        std::unique_ptr<Contour_Data> Raycast_Split_Per_Contour_Against_Given_Direction(const vec3<double> &U) const;
        std::unique_ptr<Contour_Data> Raycast_Split_Per_Contour_Into_ANT_POST(void) const;
        std::unique_ptr<Contour_Data> Raycast_Split_Per_Contour_Into_Lateral(void) const;

        //--- Core-Peel splitting. ---
        std::unique_ptr<Contour_Data> Split_Core_and_Peel(double frac_dist) const;

        //--- Geometric ordering. ---
        std::unique_ptr<Contour_Data> Reorder_LR_to_ML(void) const;

        //Contour selector factories.
        std::unique_ptr<Contour_Data> Get_Contours_Number(long int N) const; //Extracts a single contours_with_meta at list position N.
        std::unique_ptr<Contour_Data> Get_Single_Contour_Number(long int N, long int M) const; //Extracts contour M from contours_with_meta at list position N.

        std::unique_ptr<Contour_Data> Get_Contours_With_Number(long int in) const;
        std::unique_ptr<Contour_Data> Get_Contours_With_Numbers(const std::set<long int> &in) const;
        std::unique_ptr<Contour_Data> Get_Contours_With_Last_Segmentation(const uint32_t &in) const;
        std::unique_ptr<Contour_Data> Get_Contours_With_Segmentation(const std::set<uint32_t> &in) const;
//        std::unique_ptr<Contour_Data> Get_Contours_With_Name(std::string in) const;
//        std::unique_ptr<Contour_Data> Get_Contours_With_Names(set of std::string &in) const;
//        std::unique_ptr<Contour_Data> Get_Contours_On_Side_..... ?

};


//This class holds one set of DICOM dose planes. This lets us split up a large 3D dose array into layers which are easier to 
// reason about and integrate with planar imaging data.
//
//If multiple 3D dose arrays are required, then other functions should take a std::list of this class. The reasoning for this
// is that it is not always clear in what way to combine them. (Or the application should handle the logic of properly ordering 
// and dealing with them in that order. This is what I have done so far, using PostgreSQL as a PACs for ordering.)
//
////////////////////////////////////////////
//NOTE: Resist the temptation to inherit from planar_image collection. This will simplify some minor things (like image viewing) but will
// actually make the programming interface worse. Instead, use a sort of 'poor-mans' inheritance: make each class have similar
// member names and functions (except the specialized members required for each). This way, although the compiler cannot treat
// both classes as children of a common parent, the programmer should still be able to swap types and have everything work.
//
// FIXME:    ALTERNATIVE: Inherit from a common, custom-made, purely-virtual base class. This gains the best of both worlds!   :FIXME
////////////////////////////////////////////

/*
class Base_Array {
    public: 
        planar_image_collection<float,double> imagecoll;

        std::string filename; //The filename from which the data originated, if applicable.

        unsigned int bits;     //(0028,0101) "Bits Stored". 8 bit, 16 bit, 24 bit, 32 bit, etc..
                               // Note: (0028,0100) "Bits Allocated" will usually be identical. It defines the storage atom size.

        virtual ~Base_Array(){ } //Needed for safe deallocation of derived classes, and also enables use of dynamic_cast.
};
*/

class Dose_Array { //: public Base_Array {
    public:

        planar_image_collection<float,double> imagecoll;
        //std::unique_ptr<unsigned int []> max;  //The maximal pixel value in one channel. Only useful for display purposes.

        std::string filename; //The filename from which the data originated, if applicable.

        unsigned int bits;     //(0028,0101) "Bits Stored". 8 bit, 16 bit, 24 bit, 32 bit, etc..
                               // Note: (0028,0100) "Bits Allocated" will usually be identical. It defines the storage atom size.


        double grid_scale;     //(3004,000e) "Dose Grid Scaling". The scaling factor for converting (integer) channel data to 
                               // doses (in units of Gy, from what I can tell). 
                               // Should be used like:   Dose = (channel integer)*(grid_scale);

        //Constructor/Destructors.
        Dose_Array();
        Dose_Array(const Dose_Array &rhs); //Performs a deep copy (unless copying self).
//        ~Dose_Array(){}

        //Member functions.
        Dose_Array & operator=(const Dose_Array &rhs); //Performs a deep copy (unless copying self).
};

class Image_Array { //: public Base_Array {
    public:

        planar_image_collection<float,double> imagecoll;

        std::string filename; //The filename from which the data originated, if applicable.

        unsigned int bits;    //(0028,0101) "Bits Stored". 8 bit, 16 bit, 24 bit, 32 bit, etc..
                              // Note: (0028,0100) "Bits Allocated" will usually be identical. It defines the storage atom size.

        //Constructor/Destructors.
        Image_Array();
        Image_Array(const Image_Array &rhs); //Performs a deep copy (unless copying self).
        Image_Array(const Dose_Array &rhs); //Performs a deep copy.
//        ~Image_Array(){}

        //Member functions.
        Image_Array & operator=(const Image_Array &rhs); //Performs a deep copy (unless copying self).
};


//---------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------ Drover -------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------
//This class collects pixel data from CTs and dose files and mixes them with contours. Formally, this class only tracks pointers to these objects.
// This class exists to help facilitate the mixing of data of various types so that the algorithms are not cluttered with awkward references to 
// all the different objects separately. This class does not partition files into logical groups -- this needs to be done manually or using the
// file_wrangler class.
//
//Etymology: The term "drover" is defined as: 'a person whose occupation is the driving of sheep or cattle, esp to and from market'
// (from http://www.collinsdictionary.com/dictionary/english/drover .) This class is used to hold/wrangle/deal with the data we grab from DICOM files.
//
//The choice of shared_ptr here is so that we can easily copy and swap parts in and out. For instance, some operations on Contour_Data will return 
// additional instances of Contour_Data. If we want to compute something within the Drover using this new Contour_Data we either have to temporarily
// grab the unique_ptrs and swap in/out the new data, perform the computation, and swap them all back (possibly transferring the pointers back out.)
// Shared_ptrs allow us to simply copy Drover instances (at the cost of duplicating a few pointers) and performing computations with convenient 
// member functions (and no dealing with pointer swapping, etc..)
//
//If you are still muttering the various dogmas about 'shared_ptrs are bad because they told me not to use them,' consider that this class is 
// not designed to be passed around. That is, the ownership of instances of this class are intended to remain firmly within the grasp of the creator.
// If anything, they will be 'borrowed,' temporarily, for single-level computations. The underlying image data buffer are actually (at the time of
// writing) unique_ptrs, so the shared_ptrs are a 'butter layer' over the actual data. This class is merely a way to have both "single producers"/
// "single data stewards" and "multiple consumers" with the major benefits of each.
//

//Helper functions - unordered_map with iterator or pointer key types. Sorting/hashing is performed using the address of the object 
// being pointed to. This is extremely convenient, but quite unsafe. Do not use if you do not understand how to avoid segfaults!
//typedef std::list<contours_with_meta>::const_iterator  bnded_dose_map_key_t; //Required be an iterator or pointer.
typedef std::list<contours_with_meta>::iterator  bnded_dose_map_key_t; //Required be an iterator or pointer.

typedef std::function<bool (const bnded_dose_map_key_t &, const bnded_dose_map_key_t &)>  bnded_dose_map_cmp_func_t;

/*
typedef std::function<size_t (const bnded_dose_map_key_t &)>  bnded_dose_map_cmp_func_t;
const auto bnded_dose_map_cmp_lambda = [](const bnded_dose_map_key_t &n) -> size_t {
    //This lambda gets the address of the object pointed to by the key_t (an iterator or pointer type).
    // The address is explicitly casted to size_t. Maybe this should be changed to a fixed-width type?
    //
    //NOTE: This is quite dangerous and WILL lead to segfaults if improperly used! Always ensure the given 
    // key_t is valid!
    return reinterpret_cast<size_t>(&(*n));
};

typedef std::unordered_map<bnded_dose_map_key_t,double,                          bnded_dose_map_cmp_func_t>  drover_bnded_dose_mean_dose_map_t;
typedef std::unordered_map<bnded_dose_map_key_t,std::pair<int64_t,int64_t>,      bnded_dose_map_cmp_func_t>  drover_bnded_dose_accm_dose_map_t;
typedef std::unordered_map<bnded_dose_map_key_t,std::pair<double,double>,        bnded_dose_map_cmp_func_t>  drover_bnded_dose_min_max_dose_map_t;
typedef std::unordered_map<bnded_dose_map_key_t,std::tuple<double,double,double>,bnded_dose_map_cmp_func_t>  drover_bnded_dose_min_mean_max_dose_map_t;
*/

const auto bnded_dose_map_cmp_lambda = [](const bnded_dose_map_key_t &A, const bnded_dose_map_key_t &B) -> bool {
    //Given that we are dealing with iterators, we must be careful with possibly-invalid data sent to 
    // this function!

    //It is desirable to order the same on every machine so that output data can be more easily compared.
 
    //First, we check the structure name.
    if(A->Raw_ROI_name != B->Raw_ROI_name) return (A->Raw_ROI_name < B->Raw_ROI_name);
    
    //Next we check the segmentation history.
    if(A->Segmentation_History != B->Segmentation_History) return (A->Segmentation_History < B->Segmentation_History);

    //Next check the number of contours.

    // ... 

    //Next, use the built-in operator<.

    // ... 

    //Next check the address for equality. This is machine portable and consistent.
    if(reinterpret_cast<size_t>(&(*A)) == reinterpret_cast<size_t>(&(*B))) return false;

    //Finally, we resort to operator<'ing the casted address. Note that this *will* 
    // vary from machine to machine, but it may or may not ever be a problem. 
    // If we get here, issue a warning.
    // 
    FUNCWARN("Resorting to a less-than on addresses. This will produce possibly non-reproduceable sorting order! Fix me if required");
    return reinterpret_cast<size_t>(&(*A)) < reinterpret_cast<size_t>(&(*B));
};

typedef std::map<bnded_dose_map_key_t,double,                                   bnded_dose_map_cmp_func_t>  drover_bnded_dose_mean_dose_map_t;
typedef std::map<bnded_dose_map_key_t,vec3<double>,                             bnded_dose_map_cmp_func_t>  drover_bnded_dose_centroid_map_t;
typedef std::map<bnded_dose_map_key_t,std::list<double>,                        bnded_dose_map_cmp_func_t>  drover_bnded_dose_bulk_doses_map_t;
typedef std::map<bnded_dose_map_key_t,std::pair<int64_t,int64_t>,               bnded_dose_map_cmp_func_t>  drover_bnded_dose_accm_dose_map_t;
typedef std::map<bnded_dose_map_key_t,std::pair<double,double>,                 bnded_dose_map_cmp_func_t>  drover_bnded_dose_min_max_dose_map_t;
typedef std::map<bnded_dose_map_key_t,std::tuple<double,double,double>,         bnded_dose_map_cmp_func_t>  drover_bnded_dose_min_mean_max_dose_map_t;
typedef std::map<bnded_dose_map_key_t,std::tuple<double,double,double,double>,  bnded_dose_map_cmp_func_t>  drover_bnded_dose_min_mean_median_max_dose_map_t;

typedef std::tuple<vec3<double>,vec3<double>,vec3<double>,double,long int,long int> bnded_dose_pos_dose_tup_t;
typedef std::map<bnded_dose_map_key_t,std::list<bnded_dose_pos_dose_tup_t>,         bnded_dose_map_cmp_func_t>  drover_bnded_dose_pos_dose_map_t; 
typedef std::map<bnded_dose_map_key_t,std::map<std::array<int,3>,double>,           bnded_dose_map_cmp_func_t>  drover_bnded_dose_stat_moments_map_t;

drover_bnded_dose_mean_dose_map_t                drover_bnded_dose_mean_dose_map_factory(void);
drover_bnded_dose_centroid_map_t                 drover_bnded_dose_centroid_map_factory(void);
drover_bnded_dose_bulk_doses_map_t               drover_bnded_dose_bulk_doses_map_factory(void);
drover_bnded_dose_accm_dose_map_t                drover_bnded_dose_accm_dose_map_factory(void);
drover_bnded_dose_min_max_dose_map_t             drover_bnded_dose_min_max_dose_map_factory(void);
drover_bnded_dose_min_mean_max_dose_map_t        drover_bnded_dose_min_mean_max_dose_map_factory(void);
drover_bnded_dose_min_mean_median_max_dose_map_t drover_bnded_dose_min_mean_median_max_dose_map_factory(void);
drover_bnded_dose_pos_dose_map_t                 drover_bnded_dose_pos_dose_map_factory(void);
drover_bnded_dose_stat_moments_map_t             drover_bnded_dose_stat_moments_map_factory(void);

class Drover {
    public:
        bool Has_Been_Melded;   //This is a safety switch: we are *unable* to compute specific routines (DVH, etc..) unless we have called the 'Meld' function.
                                // A meld will become invalidated if certain data is modified, though you can be sneaky and directly access the buffers.

    public:

        std::shared_ptr<Contour_Data>           contour_data; //Should I allow for multiple contour data? I think it would be logical...
        std::list<std::shared_ptr<Dose_Array>>  dose_data;    //Some operations will accumulate all dose, some will not!
        std::list<std::shared_ptr<Image_Array>> image_data;   //In case we ever get more than one set of images (different modalities?)
    
        //Constructors.
        Drover();
        Drover(const Drover &in);
    
        //Member functions.
        void operator = (const Drover &rhs);
        void Bounded_Dose_General( std::list<double> *pixel_doses, 
                                   drover_bnded_dose_bulk_doses_map_t *bulk_doses, //NOTE: Similar to pixel_doses, but not all in a single bunch.
                                   drover_bnded_dose_mean_dose_map_t *mean_doses, 
                                   drover_bnded_dose_min_max_dose_map_t *min_max_doses,
                                   drover_bnded_dose_pos_dose_map_t *pos_doses,
                                   std::function<bool(bnded_dose_pos_dose_tup_t)> Fselection,
                                   drover_bnded_dose_stat_moments_map_t *centralized_moments ) const;
    
        std::list<double> Bounded_Dose_Bulk_Values(void) const;                 //If the contours contain multiple organs, we get TOTAL bulk pixel values (Gy or cGy?)
        drover_bnded_dose_mean_dose_map_t Bounded_Dose_Means(void) const;       //Get mean dose for each contour collection. See note in source.
        drover_bnded_dose_min_max_dose_map_t Bounded_Dose_Min_Max(void) const;  //Get the min & max dose for each contour collection. See note in source.
        drover_bnded_dose_min_mean_max_dose_map_t Bounded_Dose_Min_Mean_Max(void) const;  // " " " " ...
        drover_bnded_dose_min_mean_median_max_dose_map_t Bounded_Dose_Min_Mean_Median_Max(void) const; // " " " " ...
        drover_bnded_dose_stat_moments_map_t Bounded_Dose_Centralized_Moments(void) const;
        drover_bnded_dose_stat_moments_map_t Bounded_Dose_Normalized_Cent_Moments(void) const;
    
        Drover Segment_Contours_Heuristically(std::function<bool(bnded_dose_pos_dose_tup_t)> heur) const;
    
        std::pair<double,double> Bounded_Dose_Limits(void) const;  //Returns the min and max voxel doses (in cGy or Gy?) amongst ALL contour-enclosed voxels.
        std::map<double,double>  Get_DVH(void) const;              //If the contours contain multiple organs, we get TOTAL (cumulative) DVH (in Gy or cGy?)
    
        Drover Duplicate(std::shared_ptr<Contour_Data> in) const; //Duplicates all but our the contours. Inserts those passed in instead.
        Drover Duplicate(const Contour_Data &in) const; 
        Drover Duplicate(const Drover &in) const;
    
        bool Is_Melded(void) const;
        bool Meld(bool); //Attempts to verify the various pieces of data properly fit together.
    
        bool Has_Contour_Data(void) const;
        bool Has_Dose_Data(void) const;
        bool Has_Image_Data(void) const;

        void Concatenate(std::shared_ptr<Contour_Data> in);
        void Concatenate(std::list<std::shared_ptr<Dose_Array>> in);
        void Concatenate(std::list<std::shared_ptr<Image_Array>> in);
        void Concatenate(Drover in);

        void Consume(std::shared_ptr<Contour_Data> in);
        void Consume(std::list<std::shared_ptr<Dose_Array>> in);
        void Consume(std::list<std::shared_ptr<Image_Array>> in);
        void Consume(Drover in);
    
        void Plot_Dose_And_Contours(void) const;
        void Plot_Image_Outlines(void) const;
};



typedef std::function<bool (const std::string &, const std::string)>  icase_str_lt_func_t;
typedef std::map<std::string, std::string, icase_str_lt_func_t> icase_map_t;
constexpr auto icase_str_lt_lambda = [](const std::string &A, const std::string &B) -> bool {
    return std::lexicographical_compare(std::begin(A), std::end(A), std::begin(B), std::end(B));
};


// Class for dealing with commandline argument operation options.
//
// Argument case is retained, but case insensitivity is used when pushing back new arguments.
// So you get out what you put it, but the class won't accept both 'foo' and 'FOO'.
//
class OperationArgPkg {

    private:
        std::string name; // The operation name.
        icase_map_t opts; // Arguments to pass to the operation.

    public:
        OperationArgPkg(std::string unparsed, std::string sepr = ":", std::string eqls = "="); // e.g., "SomeOperation:keyA=valueA:keyB=valueB"

        OperationArgPkg & operator=(const OperationArgPkg &rhs);

        //Accessor.
        std::string getName(void) const;

        //Checks if the provided keys (and only the provided keys) are present.
        bool containsExactly(std::initializer_list<std::string> l) const;

        std::experimental::optional<std::string> getValueStr(std::string key) const;

        bool insert(std::string key, std::string val); //Will not overwrite.

};


// Class for documenting commandline argument operation options.
struct OperationArgDoc {
    std::string name;
    std::string desc;
    std::string default_val;
    bool expected;
    std::list<std::string> examples;
};

