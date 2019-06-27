#ifndef MES_H
#define MES_H

#include <vector>
#include <unordered_map>
#include <fstream>

class MES {
  std::vector<std::wstring> messages;
  
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
  
  static std::vector<unsigned char> opsize;
  static std::vector<unsigned char> opparamnum;
  static std::vector<unsigned char> opparamofs;
  static std::vector<unsigned char> opparamtypes;
  
  static std::unordered_map<std::wstring, size_t> opmap;
  static std::unordered_map<std::wstring, size_t> singlemap;
  static std::vector<std::wstring> displaylist;
  
  static std::vector<std::wstring> oplist;
  static std::vector<std::wstring> singlelist;
  
  static std::unordered_map<size_t, std::wstring> op2color;
  static std::unordered_map<std::wstring, size_t> color2op;
  
  static void loadMES(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  static void loadList(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  
  static size_t getIndex(std::wstring const& name);
  
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