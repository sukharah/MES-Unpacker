#include "MES.h"

#include <sstream>
#include <iostream>
#include <locale>
#include <algorithm>
#include <utility>

std::unordered_map<std::wstring, size_t> MES::displaymap;
std::unordered_map<std::wstring, size_t> MES::peoplemap;
std::unordered_map<std::wstring, size_t> MES::structuremap;

std::unordered_map<std::wstring, size_t> MES::opmap;

std::vector<std::wstring> MES::displaylist;
std::vector<std::wstring> MES::peoplelist;
std::vector<std::wstring> MES::structurelist;

std::vector<std::wstring> MES::oplist;

bool MES::load_people_failed = false;
bool MES::load_structures_failed = false;

bool MES::use_people_names = true;
bool MES::use_structure_names = true;

MES::init_mes MES::init; // static init

MES::init_mes::init_mes() {
  std::wstring const display[] = {
    L"0", L"1", L"2", L"3", L"4", L"5", L"6", L"7", L"8", L"9", L"-", L"A", L"B", L"C", L"D", L"E",
    L"F", L"G", L"H", L"I", L"J", L"K", L"L", L"M", L"N", L"O", L"P", L"Q", L"R", L"S", L"T", L"U",
    L"V", L"W", L"X", L"Y", L"Z", L"あ", L"い", L"う", L"え", L"お", L"か", L"き", L"く", L"け", L"こ", L"さ",
    L"し", L"す", L"せ", L"そ", L"た", L"ち", L"つ", L"て", L"と", L"な", L"に", L"ぬ", L"ね", L"の", L"は", L"ひ",
    L"ふ", L"へ", L"ほ", L"ま", L"み", L"む", L"め", L"も", L"や", L"ゆ", L"よ", L"ら", L"り", L"る", L"れ", L"ろ",
    L"わ", L"を", L"ん", L"ぁ", L"ぃ", L"ぅ", L"ぇ", L"ぉ", L"ゃ", L"ゅ", L"ょ", L"っ", L"が", L"ぎ", L"ぐ", L"げ",
    L"ご", L"ざ", L"じ", L"ず", L"ぜ", L"ぞ", L"だ", L"ぢ", L"づ", L"で", L"ど", L"ば", L"び", L"ぶ", L"べ", L"ぼ",
    L"ぱ", L"ぴ", L"ぷ", L"ぺ", L"ぽ", L"ア", L"イ", L"ウ", L"エ", L"オ", L"カ", L"キ", L"ク", L"ケ", L"コ", L"サ",
    L"シ", L"ス", L"セ", L"ソ", L"タ", L"チ", L"ツ", L"テ", L"ト", L"ナ", L"ニ", L"ヌ", L"ネ", L"ノ", L"ハ", L"ヒ",
    L"フ", L"ヘ", L"ホ", L"マ", L"ミ", L"ム", L"メ", L"モ", L"ヤ", L"ユ", L"ヨ", L"ラ", L"リ", L"ル", L"レ", L"ロ",
    L"ワ", L"ヲ", L"ン", L"ア", L"イ", L"ウ", L"エ", L"オ", L"ャ", L"ュ", L"ョ", L"ッ", L"ガ", L"ギ", L"グ", L"ゲ",
    L"ゴ", L"ザ", L"ジ", L"ズ", L"ゼ", L"ゾ", L"ダ", L"ヂ", L"ヅ", L"デ", L"ド", L"バ", L"ビ", L"ブ", L"ベ", L"ボ",
    L"ヴ", L"パ", L"ピ", L"プ", L"ペ", L"ポ", L"+", L"×", L".", L"○", L"?", L"!", L"●", L"♂", L"♀", L"·",
    L"—", L"&", L"/", L"♪", L"☆", L"★", L"♥", L"%", L"a", L"b", L"c", L"d", L"e", L"f", L"g", L"h",
    L"i", L"j", L"k", L"l", L"m", L"n", L"o", L"p", L"q", L"r", L"s", L"t", L"u", L"v", L"w", L"x",
    L"y", L"z", L"'", L"<", L">", L"(", L")", L"｢", L"｣", L"~", L"*", L"{SPACE2}", L"{SPACE3}", L"ä", L"ö", L"ü",

    L"Ä", L"Ö", L"Ü", L"β", L"\"", L",", L":"
  };
  MES::displaylist.resize(sizeof(display)/sizeof(display[0]));
  for (size_t i = 0; i < sizeof(display)/sizeof(display[0]); ++i) {
    MES::displaylist[i] = display[i];
    MES::displaymap.insert(std::pair<std::wstring, size_t>(display[i], i));
  }
  for (size_t i = sizeof(display)/sizeof(display[0]); i < 512; ++i) {
    std::wstringstream ss(L"{SYMBOL-");
    ss << i << L"}";
    MES::displaylist.push_back(ss.str());
    MES::displaymap.insert(std::pair<std::wstring, size_t>(ss.str(), i));
  }

  std::wstring const ops[] = {
    L"ENDPAGE", L"COLOR", L"LOCATION", L"PEOPLE", L"PREVINPUT", L"ITEM",
    L"ORDER", L"VAR", L"GOLD", L"STRUCT", L"PAUSE", L"SOUND", L"SOUND2",
    L"SOUND3", L"CHOICEYN", L"CHOICE", L"FACE"
  };

  size_t const opnum[] = {
    0x03, 0x10, 0x14, 0x20, 0x21, 0x25, 0x27, 0x29, 0x2a, 0x2b,
    0x30, 0x32, 0x34, 0x35, 0x40, 0x41, 0x50
  };
  MES::oplist.resize(256);
  for (size_t i = 0; i < sizeof(ops)/sizeof(ops[0]); ++i) {
    MES::oplist[opnum[i]] = ops[i];
    MES::opmap.insert(std::pair<std::wstring, size_t>(ops[i], opnum[i]));
  }

  for (size_t i = 0x11; i <= 0x17; ++i)
    if (i != 0x14)
      MES::opmap.insert(std::pair<std::wstring, size_t>(L"COLOR", i));
}

bool isDigit(std::wstring const& str, size_t ofs, size_t len) {
  return std::all_of(std::begin(str) + ofs, std::begin(str) + (ofs + len), [](wchar_t c){ return std::isdigit(c); });
}

bool isDigit(std::wstring const& str) {
  return isDigit(str, 0, str.size());
}

bool ensureBuffer(std::ifstream& infile, char* buffer, size_t buffer_len, size_t& ofs, size_t& len, size_t req) {
  bool result = (ofs + req <= len);
  if (!result && infile) {
    result = true;
    for (size_t i = ofs; i < len; ++i)
      buffer[i - ofs] = buffer[i];
    len -= ofs;
    ofs = len;
    
    while (result && ofs + req > len && len < buffer_len) {
      if (!infile) {
        result = false;
      } else {
        infile.read(buffer + len, buffer_len - len);
        len += infile.gcount();
      }
    }
    result &= (ofs + req <= len);
  }
  return result;
}

std::wstring toUpper(std::wstring const& str) {
  std::wstring copy(str);
  std::transform(std::begin(str), std::end(str), std::begin(copy), [](wchar_t c) { return std::toupper(c); });
  return copy;
}

char const bytelen[] = {
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 4u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 2u, 1u, 1u, 1u, 3u, 1u, 3u, 1u, 2u, 3u, 2u, 1u, 1u, 1u, 1u,
  1u, 1u, 3u, 3u, 2u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 3u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  4u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 2u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u
};

char const numparams[] = {
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 1u, 1u, 1u, 3u, 1u, 1u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 1u, 0u, 0u, 0u, 2u, 0u, 2u, 0u, 1u, 2u, 1u, 0u, 0u, 0u, 0u,
  0u, 0u, 2u, 2u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  3u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u
};

std::wstring wprintNum(int v) {
  bool neg = (v < 0);
  std::wstring ws;
  if (neg)
    v = -v;
  if (!v)
    ws.append(1, L'0');
  else
    while (v > 0) {
      ws.append(1, L'0' + v % 10);
      v /= 10;
    }
  if (neg)
    ws.append(1, L'-');
  std::reverse(std::begin(ws), std::end(ws));
  return ws;
}

size_t const opToCol[] = {
  0, 1, 2, 3, 0, 4, 5, 6
};

size_t const colToOp[] = {
  0, 1, 2, 3, 5, 6, 7
};

std::wstring getColor(size_t colnum) {
  return std::wstring(1, L'0' + colnum);
}

size_t getColor(std::wstring const& colname) {
  size_t col = 0;
  if (isDigit(colname)){
    col = std::stoi(colname);
  }
  return col;
}

bool MES::readMES(std::ifstream& infile) {
  char buffer[4096];
  infile.read(buffer, sizeof(buffer));
  size_t buffer_len = infile.gcount();
  size_t buffer_ofs = 8;
  size_t signature = buffer[0] << 24 | (buffer[1] & 0xff) << 16 | (buffer[2] & 0xff) << 8 | (buffer[3] & 0xff);
  
  bool result = false;
  
  if (signature == 0xcdc3b0b0) {
    size_t num_messages = buffer[4] << 24 | (buffer[5] & 0xff) << 16 | (buffer[6] & 0xff) << 8 | (buffer[7] & 0xff);
    this->messages.resize(num_messages);
    size_t* offsets = new size_t[num_messages];
    result = true;
    for (size_t m = 0; result && m < num_messages; ++m) {
      if (ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, 4)) {
        offsets[m] = buffer[buffer_ofs] << 24 | (buffer[buffer_ofs + 1] & 0xff) << 16 | (buffer[buffer_ofs + 2] & 0xff) << 8 | (buffer[buffer_ofs + 3] & 0xff);
        buffer_ofs += 4;
      } else
        result = false;
    }
    size_t data_ofs = num_messages * 4 + 8;
    if (result) {
      for (size_t m = 0; result && m < num_messages; ++m) { // read the messages
        if (data_ofs != offsets[m]) {
          infile.seekg(data_ofs = offsets[m]);
          buffer_ofs = buffer_len;
        }
        std::wstring message;
        bool eom = false;
        while (!eom) {
          if (!ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, 1)) {
            result = false;
          }
          size_t op = buffer[buffer_ofs++] & 0xff;
          size_t required_length = bytelen[op] - 1;
          if (!ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, required_length)) {
            result = false;
          }
          switch (op) {
            case 0x00:
              eom = true;
              break;
            case 0x01:
              message.append(1, L'\n');
              break;
            case 0x02:
              message.append(1, L' ');
              break;
            case 0x10: case 0x11: case 0x12: case 0x13:
            case 0x15: case 0x16: case 0x17:
              message.append(L"{COLOR-").append(getColor(opToCol[op - 0x10])).append(1, L'}');
              break;
            case 0x20:
              message.append(L"{PEOPLE-").append(getPeopleName(buffer[buffer_ofs] & 0xff)).append(1, L'}');
              break;
            case 0x2b:
              message.append(L"{STRUCTURE-").append(getStructureName(buffer[buffer_ofs])).append(1, L'}');
              break;
            case 0x81:
            case 0x80:{
              size_t charidx = ((buffer[buffer_ofs] & 0xff) | op << 8) & 0x1ff;
              message.append(MES::displaylist[charidx]);
              break;
            }
            default:
              std::wstring const& opname = oplist[op];
              if (opname.empty()) {
                message.append(1, L'{').append(wprintNum(op)).append(1, L'}');
              } else {
                message.append(1, L'{').append(opname);
                for (size_t i = 0; i < required_length; ++i) {
                  message.append(1, L'-').append(wprintNum(buffer[buffer_ofs + i] & 0xff));
                }
                message.append(1, L'}');
              }
          }
          buffer_ofs += required_length;
          data_ofs += required_length + 1;
        }
        this->messages[m] = message;
        while (buffer_ofs < buffer_len && buffer[buffer_ofs] == 0) {
          ++buffer_ofs;
          ++data_ofs;
        }
      }
    }
    delete[] offsets;
  }
  
  return result;
}

void MES::writeMES(std::ofstream& outfile) const {
  const size_t BUFFER_MAX = 4096 - (4 + 5);
  char buffer[4096];
  buffer[0] = 0xcd;
  buffer[1] = 0xc3;
  buffer[2] = 0xb0;
  buffer[3] = 0xb0;
  buffer[4] = this->messages.size() >> 24;
  buffer[5] = this->messages.size() >> 16;
  buffer[6] = this->messages.size() >> 8;
  buffer[7] = this->messages.size();
  
  outfile.write(buffer, 8);
  size_t data_ofs = this->messages.size() * 4 + 8;
  size_t buffer_len = 0;
  outfile.seekp(data_ofs, std::ios::beg);
  size_t* offsets = new size_t[this->messages.size()];
  std::wstring params[4];
  
  for (size_t i = 0; i < this->messages.size(); ++i) {
    offsets[i] = data_ofs;
    std::wstring const& message = this->messages[i];
    for (size_t char_ofs = 0; char_ofs < message.size(); ++char_ofs) {
      if (buffer_len > BUFFER_MAX) {
        outfile.write(buffer, buffer_len);
        buffer_len = 0;
      }
      wchar_t first_char = message[char_ofs];
      if (first_char == L'{') {
        if (std::isdigit(message[++char_ofs])) {
          size_t eos = message.find(L'}', char_ofs);
          std::wstring param = message.substr(char_ofs, eos - char_ofs);
          if (isDigit(param)) {
            buffer[buffer_len++] = std::stoi(param);
            ++data_ofs;
          } else {
            
          }
        } else {
          size_t opstart = char_ofs;
          while (message[char_ofs] != L'}' && message[char_ofs] != L'-') {
            ++char_ofs;
          }
          std::wstring opstr = message.substr(opstart, char_ofs - opstart);
          std::unordered_map<std::wstring, size_t>::iterator it = opmap.find(opstr);
          if (it != opmap.end()) {
            size_t op = it->second;
            size_t size = bytelen[op];
            size_t numops = 0;
            buffer[buffer_len] = op;
            while (numops < size - 1 && message[char_ofs] != '}') {
              if (message[char_ofs] == '-')
                ++char_ofs;
              size_t param_start = char_ofs;
              while (message[char_ofs] != '}' && message[char_ofs] != '-')
                ++char_ofs;
              std::wstring param = message.substr(param_start, char_ofs - param_start);
              params[numops++] = param;
            }
            size_t expected_params = numparams[op];
            if (numops < expected_params) {
              // error insufficient params
            } else {
              switch (op) {
                case 0x10: case 0x11: case 0x12: case 0x13:
                case 0x15: case 0x16: case 0x17:{
                  size_t index = getColor(params[0]);
                  buffer[buffer_len++] = 0x10 + colToOp[index];
                  break;}
                case 0x20:{
                  size_t index = MES::getPeopleIndex(params[0]);
                  buffer[buffer_len++] = index;
                  break;}
                case 0x2b:{
                  size_t index = MES::getStructureIndex(params[0]);
                  buffer[buffer_len++] = index;
                  break;}
                default:
                  for (size_t i = 1; i <= numops; ++i) {
                    buffer[buffer_len + i] += std::stoi(params[i - 1]);
                  }
              }
            }
            buffer_len += size;
            data_ofs += size;
          } else {
            it = MES::displaymap.find(opstr);
            if (it != MES::displaymap.end()) {
              buffer[buffer_len++] = it->second;
            } else {
              // error unrecognized op
            }
          }
        }
      } else {
        if (first_char == L' ') {
          buffer[buffer_len++] = 0x02;
        } else if (first_char == L'\n') {
          buffer[buffer_len++] = 0x01;
        } else {
          std::wstring ws(1, first_char);
          std::unordered_map<std::wstring, size_t>::iterator it = MES::displaymap.find(ws);
          if (it != MES::displaymap.end()) {
            buffer[buffer_len] = 0x80 + (it->second >> 8);
            buffer[buffer_len + 1] = it->second;
            buffer_len += 2;
          } else { // invalid character
            
          }
        }
      }
    }
    buffer[buffer_len++] = 0;
    while (buffer_len & 3)
      buffer[buffer_len++] = 0;
  }
  if (buffer_len)
    outfile.write(buffer, buffer_len);

  buffer[3] = buffer[2] = buffer[1] = buffer[0] = 0;
  outfile.write(buffer, 4);
  
  outfile.seekp(8, std::ios::beg);
  buffer_len = 0;
  for (size_t m = 0; m < this->messages.size(); ++m) {
    if (buffer_len > BUFFER_MAX) {
      outfile.write(buffer, buffer_len);
      buffer_len = 0;
    }
    buffer[buffer_len] = offsets[m] >> 24;
    buffer[buffer_len + 1] = offsets[m] >> 16;
    buffer[buffer_len + 2] = offsets[m] >> 8;
    buffer[buffer_len + 3] = offsets[m];
    buffer_len += 4;
  }
  if (buffer_len)
    outfile.write(buffer, buffer_len);
}

bool MES::readText(std::wifstream& infile) {
  std::wstring line;
  std::wstring message;
  size_t messagenum = 0;
  bool messageline = true;
  while (std::getline(infile, line)) {
    size_t ofs;
    if (line.compare(0, 4, L"--- ") == 0 && (ofs = line.find(L"message ")) != std::string::npos) {
      if (messagenum) {
        if (messagenum > this->messages.size() + 1) {
          this->messages.resize(messagenum);
          this->messages[messagenum - 1] = message;
        } else {
          this->messages.push_back(message);
        }
        message.clear();
      }
      
      while (line[ofs] == L' ')
        --ofs;
      
      size_t eofs = line.find(L"---", ofs + 8);
      while (line[eofs - 1] == L' ')
        --eofs;
      
      int messageidx = std::stoi(line.substr(ofs + 8, eofs - (ofs + 8))); // throws
      
      messagenum = messageidx + 1;
      
      messageline = true;
    } else {
      if (!messageline)
        message.append(1, L'\n');
      message.append(line);
      messageline = false;
    }
  }
  if (messagenum) {
    if (messagenum > this->messages.size() + 1) {
      this->messages.resize(messagenum);
      this->messages[messagenum - 1] = message;
    } else {
      this->messages.push_back(message);
    }
  }
  return true;
}

void MES::writeText(std::wofstream& outfile) const {
  for (size_t i = 0; i < this->messages.size(); ++i) {
    if (i)
      outfile << std::endl;
    outfile << "--- message " << i << " ---" << std::endl;
    outfile << this->messages[i];
  }
}

void MES::loadMES(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag) {
  std::ifstream mesfile;
  mesfile.open(filepath, std::ios_base::in | std::ios_base::binary);
  if (mesfile) {
    MES mes;
    mes.readMES(mesfile);
    mesfile.close();
    mlist.resize(mes.messages.size());
    for (size_t i = 0; i < mes.messages.size(); ++i) {
      std::wstring upper = toUpper(mes.messages[i]);
      mlist[i] = upper;
      mmap.insert(std::pair<std::wstring, size_t>(upper, i));
    }
  } else {
    fail_flag = true;
  }
}


void MES::loadPeople() {
  if (MES::use_people_names && MES::peoplemap.empty() && !MES::load_people_failed) {
    MES::loadMES("people.mes", MES::peoplelist, MES::peoplemap, MES::load_people_failed);
    if (MES::load_people_failed)
      MES::use_people_names = false;
  }
}

void MES::loadStructures() {
  if (MES::use_structure_names && MES::structuremap.empty() && !MES::load_structures_failed) {
    MES::loadMES("structure.mes", MES::structurelist, MES::structuremap, MES::load_structures_failed);
    if (MES::load_structures_failed)
      MES::use_structure_names = false;
  }
}

std::wstring MES::getPeopleName(size_t index) {
  MES::loadPeople();
  
  std::wstringstream ss;
  
  if (!MES::use_people_names || index >= MES::peoplelist.size()) {
    ss << index;
  } else {
    ss << MES::peoplelist[index];
  }
  return ss.str();
}

size_t MES::getPeopleIndex(std::wstring const& name) {
  bool dig = isDigit(name);
  if (!dig)
    MES::loadPeople();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::unordered_map<std::wstring, size_t>::iterator it = MES::peoplemap.find(name);
    if (it != MES::peoplemap.end()) {
      index = it->second;
    } else {
      // error, people not found
    }
  }
  return index;
}

std::wstring MES::getStructureName(size_t index) {
  MES::loadStructures();
  
  std::wstringstream ss;
  if (!MES::use_structure_names || index >= MES::structurelist.size()) {
    ss << index;
  } else {
    ss << MES::structurelist[index];
  }
  return ss.str();
}

size_t MES::getStructureIndex(std::wstring const& name) {
  bool dig = isDigit(name);
  if (!dig)
    MES::loadStructures();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::unordered_map<std::wstring, size_t>::iterator it = MES::structuremap.find(name);
    if (it != MES::structuremap.end()) {
      index = it->second;
    } else {
      // error, structure not found
    }
  }
  return index;
}