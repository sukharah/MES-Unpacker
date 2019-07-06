#include "main.h"

int main(int argc, char** argv) {
  if (argc >= 4) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> utfconv;
    
    std::wstring command = utfconv.from_bytes(argv[1]);
    
    std::wstring dpathin = L"data/awl.txt", dpathout;
    if (argc >= 5) {
      dpathin = utfconv.from_bytes(argv[4]);
    }
    if (argc >= 6) {
      dpathout = utfconv.from_bytes(argv[5]);
    } else {
      dpathout = dpathin;
    }
    
    
    if (L"unpack" == command) {
      std::ifstream infile;
      infile.open(argv[2], std::ios_base::in | std::ios_base::binary);
      if (infile) {
        std::wofstream outfile;
        outfile.open(argv[3]);
        if (outfile) {
          outfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
          MES mes;
          mes.readMES(infile, dpathin);
          mes.writeText(outfile, dpathout);
          outfile.close();
        } else {
          std::wcout << L"Could not open file " << argv[3] << " for writing" << std::endl;
        }
        infile.close();
      } else {
        std::wcout << L"Could not open file " << argv[2] << " for reading" << std::endl;
      }
    } else if (L"pack" == command) {
      std::wifstream infile;
      infile.open(argv[2]);
      if (infile) {
        infile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
        std::ofstream outfile;
        outfile.open(argv[3], std::ios_base::out | std::ios_base::binary);
        if (outfile) {
          MES mes;
          mes.readText(infile, dpathin);
          mes.writeMES(outfile, dpathout);
          outfile.close();
        } else {
          std::wcout << L"Could not open file " << argv[3] << L" for writing" << std::endl;
        }
        infile.close();
      } else {
        std::wcout << L"Could not open file " << argv[2] << L" for reading" << std::endl;
      }
    } else if (L"retext" == command) {
      std::wifstream infile;
      infile.open(argv[2]);
      if (infile) {
        infile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
        std::wofstream outfile;
        outfile.open(argv[3]);
        if (outfile) {
          outfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
          MES mes;
          mes.readText(infile, dpathin);
          mes.writeText(outfile, dpathout);
          outfile.close();
        } else {
          std::cout << L"Could not open file " << argv[3] << L" for writing" << std::endl;
        }
        infile.close();
      } else {
        std::wcout << L"Could not open file " << argv[2] << L" for reading" << std::endl;
      }
    } else if (L"repack" == command) {
      std::ifstream infile;
      infile.open(argv[2], std::ios_base::in | std::ios_base::binary); // out
      if (infile) {
        std::ofstream outfile;
        outfile.open(argv[3], std::ios_base::out | std::ios_base::binary);
        if (outfile) {
          MES mes;
          mes.readMES(infile, dpathin);
          mes.writeMES(outfile, dpathout);
          outfile.close();
        } else {
          std::wcout << L"Could not open file " << argv[3] << L" for writing" << std::endl;
        }
        infile.close();
      } else {
        std::wcout << L"Could not open file " << argv[2] << L" for reading" << std::endl;
      }
    } else {
      std::wcout << L"Unrecognized command: " << command << std::endl;
    }
  } else {
    std::wcout << L"Insufficient parameters" << std::endl;
  }
}