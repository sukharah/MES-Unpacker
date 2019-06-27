#ifndef MES_H
#define MES_H

#include <vector>
#include <unordered_map>
#include <fstream>

class MES {
  std::vector<std::wstring> messages;
  /*
  class pre_mes {
    public:
      pre_mes();
  };
  
  static pre_mes pre;
  */
  // strip entries of entities
  /*
  void strip();
  */
public:
  static std::unordered_map<std::wstring, size_t> displaymap;
  
  static std::unordered_map<std::wstring, size_t> parammap;
  static std::vector<size_t> paramsize;
  
  static std::unordered_map<std::wstring, size_t> idxparammap;
  static std::vector<std::unordered_map<std::wstring, size_t>> idxmap;
  static std::vector<std::vector<std::wstring>> idxlist;
  static std::vector<char> idxenablenames;
  static std::vector<char> idxloadfailed;
  static std::vector<unsigned char> opsingle;
  /*
  static std::unordered_map<std::wstring, size_t> peoplemap;
  static std::unordered_map<std::wstring, size_t> structuremap;
  static std::unordered_map<std::wstring, size_t> itemmap;
  static std::unordered_map<std::wstring, size_t> cropmap;
  static std::unordered_map<std::wstring, size_t> colormap;
  static std::unordered_map<std::wstring, size_t> sfxmap;
  */
  
  static std::vector<unsigned char> opsize;
  static std::vector<unsigned char> opparamnum;
  static std::vector<unsigned char> opparamofs;
  static std::vector<unsigned char> opparamtypes;
  
  static std::unordered_map<std::wstring, size_t> opmap;
  static std::unordered_map<std::wstring, size_t> singlemap;
  static std::vector<std::wstring> displaylist;
  /*static std::vector<std::wstring> peoplelist;
  static std::vector<std::wstring> structurelist;
  static std::vector<std::wstring> itemlist;
  static std::vector<std::wstring> croplist;
  static std::unordered_map<size_t, std::wstring> sfxlist;*/
  
  static std::vector<std::wstring> oplist;
  static std::vector<std::wstring> singlelist;
  
  static std::unordered_map<size_t, std::wstring> op2color;
  static std::unordered_map<std::wstring, size_t> color2op;
  
  static void loadMES(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  static void loadList(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  /*
  static void loadPeople();
  static void loadStructures();
  static void loadItems();
  static void loadCrops();
  static void loadSFXs();
  
  static std::wstring getName(size_t index);
  static std::wstring getPeopleName(size_t index);
  static std::wstring getStructureName(size_t index);
  static std::wstring getItemName(size_t index);
  static std::wstring getCropName(size_t index);
  static std::wstring getColorName(size_t index);
  static std::wstring getSFXName(size_t index);
  */
  static size_t getIndex(std::wstring const& name);
  /*static size_t getPeopleIndex(std::wstring const& name);
  static size_t getStructureIndex(std::wstring const& name);
  static size_t getItemIndex(std::wstring const& name);
  static size_t getCropIndex(std::wstring const& name);
  static size_t getColorIndex(std::wstring const& name);
  static size_t getSFXIndex(std::wstring const& name);
  
  static bool load_people_failed;
  static bool load_structures_failed;
  static bool load_items_failed;
  static bool load_crops_failed;
  static bool load_sfxs_failed;
  
  static bool use_people_names;
  static bool use_structure_names;
  static bool use_item_names;
  static bool use_crop_names;
  static bool use_color_names;
  static bool use_sfx_names;
  */
  static void loadSymbols(std::wifstream& infile);
  static void loadSingleByte(std::wifstream& infile);
  
  static void init(std::wifstream& infile);
  
  // write text to a file
  void writeText(std::wofstream& outtext) const;
  // pack text data into an archive
  void writeMES(std::ofstream& outmes) const;
  
  // read text from a file
  bool readText(std::wifstream& intext);
  // unpack text data from MES
  // nospecial is used for loading param idx values
  bool readMES(std::ifstream& inmes, bool nospecial = false);
  
};

#endif //MES_H