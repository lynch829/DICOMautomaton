//Imebra_Shim.h.

#ifndef _IMEBRA_SHIM_H_DICOMAUTOMATON
#define _IMEBRA_SHIM_H_DICOMAUTOMATON

#include <vector>
#include <string>
#include <memory>
#include <list>
#include <map>

#include "Structs.h"
#include "YgorContainers.h"  //Needed for bimap class.

//------------------ General ----------------------
//One-offs.
std::string get_tag_as_string(const std::string &filename, size_t U, size_t L);
std::string get_modality(const std::string &filename);
std::string get_patient_ID(const std::string &filename);

//Mass top-level tag enumeration, for ingress into database.
//
//NOTE: May not be complete. Add additional tags as needed!
std::map<std::string,std::string> get_metadata_top_level_tags(const std::string &filename);


//------------------ Contours ---------------------
bimap<std::string,long int> get_ROI_tags_and_numbers(const std::string &filename);

std::unique_ptr<Contour_Data>  get_Contour_Data(const std::string &filename);


/*
//-----------------------------------------------
//       These are all marked for removal!
std::unique_ptr<Pixel_Data>  get_pixel_data_and_numbers(std::string FilenameIn);
std::unique_ptr<Pixel_Data>  get_pixel_data_and_numbers(std::vector<std::string> FilenameIn);

std::list<std::shared_ptr<Pixel_Data>> get_dose_pixel_data_and_numbers(std::vector<std::string> FilenameIn);

std::unique_ptr<Pixel_Data>  estimate_pixel_memory(std::string FilenameIn);
//-----------------------------------------------
*/

//-------------------- Images ----------------------
//This routine will often result in an array with only a single image. So collate output as needed.
std::unique_ptr<Image_Array> Load_Image_Array(const std::string &filename);

//These pointers will actually be unique. This just aims to convert from unique_ptr to shared_ptr for you.
std::list<std::shared_ptr<Image_Array>>  Load_Image_Arrays(const std::list<std::string> &filenames);

//Since many images must be loaded individually from a file, we will often have to collate them together.
std::unique_ptr<Image_Array> Collate_Image_Arrays(std::list<std::shared_ptr<Image_Array>> &in);

//--------------------- Dose -----------------------
std::unique_ptr<Dose_Array> Load_Dose_Array(const std::string &filename);

//These pointers will actually be unique. This just aims to convert from unique_ptr to shared_ptr for you.
std::list<std::shared_ptr<Dose_Array>>  Load_Dose_Arrays(const std::list<std::string> &filenames);


#endif
