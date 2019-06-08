#ifndef MES_H
#define MES_H

#include <vector>
#include <unordered_map>
#include <fstream>

class MES {
  std::vector<std::wstring> messages;
  
  class init_mes {
    public:
      init_mes();
  };
  
  static init_mes init;
  
public:
  static std::unordered_map<std::wstring, size_t> displaymap;
  static std::unordered_map<std::wstring, size_t> peoplemap;
  static std::unordered_map<std::wstring, size_t> structuremap;
  
  static std::unordered_map<std::wstring, size_t> opmap;
  
  static std::vector<std::wstring> displaylist;
  static std::vector<std::wstring> peoplelist;
  static std::vector<std::wstring> structurelist;
  
  static std::vector<std::wstring> oplist;
  
  static void loadMES(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  static void loadPeople();
  static void loadStructures();
  
  static std::wstring getPeopleName(size_t index);
  static std::wstring getStructureName(size_t index);
  static size_t getPeopleIndex(std::wstring const& name);
  static size_t getStructureIndex(std::wstring const& name);
  
  static bool load_people_failed;
  static bool load_structures_failed;
  
  static bool use_people_names;
  static bool use_structure_names;
  
  
  // write text to a file
  void writeText(std::wofstream& outtext) const;
  // pack text data into an archive
  void writeMES(std::ofstream& outmes) const;
  
  // read text from a file
  bool readText(std::wifstream& intext);
  // unpack text data from MES
  bool readMES(std::ifstream& inmes);
};

#endif //MES_H