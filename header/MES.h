#ifndef MES_H
#define MES_H

#include <vector>
#include <unordered_map>
#include <fstream>

class MES {
public:
  struct Error {
    std::wstring message;
  };
  
  struct Decoder {
    bool isMSB = true;
  
    std::unordered_map<std::wstring, size_t> displaymap;
  
    std::unordered_map<std::wstring, size_t> parammap;
    std::vector<size_t> paramsize;
    
    std::unordered_map<std::wstring, size_t> idxparammap;
    std::vector<std::unordered_map<std::wstring, size_t>> idxmap;
    std::vector<std::vector<std::wstring>> idxlist;
    std::vector<char> idxenablenames;
    std::vector<char> idxloadfailed;
    std::vector<unsigned char> opsingle;
    
    std::vector<unsigned char> opsize;
    std::vector<unsigned char> opparamnum;
    std::vector<unsigned char> opparamofs;
    std::vector<unsigned char> opparamtypes;
    
    std::unordered_map<std::wstring, size_t> opmap;
    std::unordered_map<std::wstring, size_t> singlemap;
    std::vector<std::wstring> displaylist;
    
    std::vector<std::wstring> oplist;
    std::vector<std::wstring> singlelist;
    
    std::unordered_map<size_t, std::wstring> op2color;
    std::unordered_map<std::wstring, size_t> color2op;
    
    std::vector<MES::Error> errors;
  };

private:
  std::vector<std::wstring> messages;
  
  std::vector<MES::Error> errors;
  
  static std::unordered_map<std::wstring, MES::Decoder> decoder_map;

  static void loadMES(std::wstring const& decoder_path, std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  static void loadList(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag);
  
  static size_t getIndex(std::wstring const& name);
  
  static void loadSymbols(std::wifstream& infile, MES::Decoder& decoder);
  static void loadSingleByte(std::wifstream& infile, MES::Decoder& decoder);
  
  //static void init(std::wifstream& infile);
  static MES::Decoder* getDecoder(std::wstring const& decoder_path);

public:
  // write text to a file
  void writeText(std::wofstream& outtext, std::wstring const& decoder_path) const;
  // pack text data into an archive
  void writeMES(std::ofstream& outmes, std::wstring const& decoder_path) const;
  
  // read text from a file
  bool readText(std::wifstream& intext, std::wstring const& decoder_path);
  // unpack text data from MES
  // nospecial is used for loading param idx values
  bool readMES(std::ifstream& inmes, std::wstring const& decoder_path, bool nospecial = false);
  
};

#endif //MES_H