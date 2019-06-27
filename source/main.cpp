#include "main.h"

int main(int argc, char** argv) {
  if (argc >= 4) {
    std::string command(argv[1]);
    std::string dpath = "data/awl.txt";
    if (argc >= 5) {
      dpath = std::string(argv[4]);
    }
    
    std::wifstream csfile;
    csfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
    csfile.open(dpath);
    if (csfile) {
      MES::init(csfile);
      csfile.close();
    
      if ("unpack" == command) {
        std::ifstream infile;
        infile.open(argv[2], std::ios_base::in | std::ios_base::binary);
        if (infile) {
          std::wofstream outfile;
          outfile.open(argv[3]);
          if (outfile) {
            outfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
            MES mes;
            mes.readMES(infile);
            mes.writeText(outfile);
            outfile.close();
          } else {
            std::cout << "Could not open file " << argv[3] << " for writing" << std::endl;
          }
          infile.close();
        } else {
          std::cout << "Could not open file " << argv[2] << " for reading" << std::endl;
        }
      } else if ("pack" == command) {
        std::wifstream infile;
        infile.open(argv[2]);
        if (infile) {
          infile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
          std::ofstream outfile;
          outfile.open(argv[3], std::ios_base::out | std::ios_base::binary);
          if (outfile) {
            MES mes;
            mes.readText(infile);
            mes.writeMES(outfile);
            outfile.close();
          } else {
            std::cout << "Could not open file " << argv[3] << " for writing" << std::endl;
          }
          infile.close();
        } else {
          std::cout << "Could not open file " << argv[2] << " for reading" << std::endl;
        }
      } else if ("retext" == command) {
        std::wifstream infile;
        infile.open(argv[2]);
        if (infile) {
          infile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
          std::wofstream outfile;
          outfile.open(argv[3]);
          if (outfile) {
            outfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
            MES mes;
            mes.readText(infile);
            mes.writeText(outfile);
            outfile.close();
          } else {
            std::cout << "Could not open file " << argv[3] << " for writing" << std::endl;
          }
          infile.close();
        } else {
          std::cout << "Could not open file " << argv[2] << " for reading" << std::endl;
        }
      } else if ("repack" == command) {
        std::ifstream infile;
        infile.open(argv[2], std::ios_base::in | std::ios_base::binary); // out
        if (infile) {
          std::ofstream outfile;
          outfile.open(argv[3], std::ios_base::out | std::ios_base::binary);
          if (outfile) {
            MES mes;
            mes.readMES(infile);
            mes.writeMES(outfile);
            outfile.close();
          } else {
            std::cout << "Could not open file " << argv[3] << " for writing" << std::endl;
          }
          infile.close();
        } else {
          std::cout << "Could not open file " << argv[2] << " for reading" << std::endl;
        }
      } else {
        std::cout << "Unrecognized command: " << command << std::endl;
      }
    } else {
      std::cout << "Could not open init file" << std::endl;
    }
  } else {
    std::cout << "Insufficient parameters" << std::endl;
  }
}