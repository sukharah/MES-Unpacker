#include "MES.h"

#include <sstream>
#include <iostream>
#include <locale>
#include <algorithm>
#include <utility>

#ifdef DUPE
const bool SEQ_DEDUPE = true;
#else
const bool SEQ_DEDUPE = false;
#endif

std::unordered_map<std::wstring, size_t> MES::displaymap;
std::unordered_map<std::wstring, size_t> MES::peoplemap;
std::unordered_map<std::wstring, size_t> MES::structuremap;
std::unordered_map<std::wstring, size_t> MES::itemmap;
std::unordered_map<std::wstring, size_t> MES::cropmap;
std::unordered_map<std::wstring, size_t> MES::colormap;
std::unordered_map<std::wstring, size_t> MES::sfxmap;

std::unordered_map<std::wstring, size_t> MES::opmap;

std::vector<std::wstring> MES::displaylist;
std::vector<std::wstring> MES::peoplelist;
std::vector<std::wstring> MES::structurelist;
std::vector<std::wstring> MES::itemlist;
std::vector<std::wstring> MES::croplist;
std::unordered_map<size_t, std::wstring> MES::sfxlist;

std::vector<std::wstring> MES::oplist;

bool MES::load_people_failed = false;
bool MES::load_structures_failed = false;
bool MES::load_items_failed = false;
bool MES::load_crops_failed = false;
bool MES::load_sfxs_failed = false;
/*
bool MES::use_people_names = true;
bool MES::use_structure_names = true;
bool MES::use_item_names = true;
bool MES::use_crop_names = true;
bool MES::use_color_names = true;
bool MES::use_sfx_names = true;
*/

bool MES::use_people_names = true;
bool MES::use_structure_names = true;
bool MES::use_item_names = true;
bool MES::use_crop_names = true;
bool MES::use_color_names = true;
bool MES::use_sfx_names = true;

char const bytelen[] = {
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 4u, 1u, 5u, 1u, 2u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 2u, 1u, 1u, 1u, 3u, 3u, 3u, 3u, 2u, 3u, 2u, 3u, 1u, 1u, 1u,
  1u, 1u, 3u, 3u, 3u, 2u, 2u, 5u, 2u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 3u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  4u, 1u, 5u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u, 2u,
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
  1u, 1u, 1u, 1u, 3u, 1u, 1u, 1u, 1u, 1u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 1u, 0u, 0u, 0u, 1u, 1u, 2u, 1u, 1u, 2u, 1u, 2u, 0u, 0u, 0u,
  0u, 0u, 1u, 1u, 1u, 1u, 1u, 4u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 2u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  3u, 0u, 1u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u,
  0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u
};

size_t const PARAM_BYTE = 0,
             PARAM_SHORT = 1,
             PARAM_COLORID = 2,
             PARAM_PEOPLEID = 3, // byte
             PARAM_ITEMID = 4, // short
             PARAM_STRUCTUREID = 5, // byte
             PARAM_SFXID = 6, // short
             PARAM_RGBA = 7, // int
             PARAM_CROPID = 8, // short
             PARAM_INT = 9,
             PARAM_SYMBOL = 10;

size_t const paramtypes[] = {
  // 0x00 EOM
  // 0x01 newline
  // 0x02 space
  // 0x03 {ENDPAGE}
  // 0x04-0x0f undefined
  // 0x10 {COLOR-0}
  PARAM_COLORID,
  // 0x11 {COLOR-1} // blue?
  PARAM_COLORID,
  // 0x12 {COLOR-RED} // 2
  PARAM_COLORID,
  // 0x13 {COLOR-GREEN} // 3
  PARAM_COLORID,
  // 0x14 LOCATION
  PARAM_BYTE, PARAM_BYTE, PARAM_BYTE,
  // 0x15 undefined
  PARAM_COLORID, // ?
  // 0x16 {COLORRGBA-0x00000000}
  PARAM_RGBA,
  // 0x17 {COLOR-BLACK} // 6
  PARAM_COLORID,
  // 0x18 OP18
  PARAM_BYTE,
  // 0x19 OP19
  PARAM_BYTE,
  // 0x1a-0x1f undefined
  // 0x20 {PEOPLE
  PARAM_PEOPLEID,
  // 0x21 {PREVINPUT
  PARAM_BYTE,
  // 0x22 undefined
  // 0x23 OP23
  // 0x24 OP24
  // 0x25 ITEM
  PARAM_ITEMID,
  // 0x26 CROP
  PARAM_CROPID,
  // 0x27 ORDER
  PARAM_BYTE, PARAM_BYTE,
  // 0x28 OP28
  PARAM_SHORT,
  // 0x29 VAR
  PARAM_BYTE,
  // 0x2a GOLD
  PARAM_BYTE, PARAM_BYTE,
  // 0x2b STRUCTURE
  PARAM_STRUCTUREID,
  // 0x2c OP2C
  PARAM_BYTE, PARAM_BYTE,
  // 0x2d OP2D
  // 0x2e-0x2f undefined
  // 0x30 PAUSE
  // 0x31 OP31
  // 0x32 WAIT
  PARAM_SHORT,
  // 0x33 OP33
  PARAM_SHORT,
  // 0x34 PLAYSFX
  PARAM_SFXID,
  // 0x35 TEXTSOUND
  PARAM_BYTE,
  // 0x36 OP36
  PARAM_BYTE,
  // 0x37 OP37
  PARAM_BYTE, PARAM_BYTE, PARAM_BYTE, PARAM_BYTE,
  // 0x38 OP38
  PARAM_BYTE,
  // 0x39 OP39
  // 0x3a-0x3f undefined
  // 0x40 CHOICEYN
  PARAM_BYTE,
  // 0x41 CHOICE
  PARAM_BYTE, PARAM_BYTE,
  // 0x42-0x4f undefined
  // 0x50 FACE
  PARAM_BYTE, PARAM_BYTE, PARAM_BYTE,
  // 0x51 undefined
  // 0x52 OP52
  PARAM_INT,
  // 0x53-0x7f undefined
  // 0x80 SYMBOL
  PARAM_SYMBOL,
  // 0x81 SYMBOL
  PARAM_SYMBOL,
  // 0x82 SYMBOL
  PARAM_SYMBOL,
  // 0x83 SYMBOL
  PARAM_SYMBOL,
  // 0x84 SYMBOL
  PARAM_SYMBOL,
  // 0x85 SYMBOL
  PARAM_SYMBOL,
  // 0x86 SYMBOL
  PARAM_SYMBOL,
  // 0x87 SYMBOL
  PARAM_SYMBOL,
  // 0x88 SYMBOL
  PARAM_SYMBOL,
  // 0x89 SYMBOL
  PARAM_SYMBOL,
  // 0x8a SYMBOL
  PARAM_SYMBOL,
  // 0x8b SYMBOL
  PARAM_SYMBOL,
  // 0x8c SYMBOL
  PARAM_SYMBOL,
  // 0x8d SYMBOL
  PARAM_SYMBOL,
  // 0x8e SYMBOL
  PARAM_SYMBOL,
  // 0x8f SYMBOL
  PARAM_SYMBOL
  // 0x90-0xff undefined
};

size_t paramtypeofs[256];

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
    L"ワ", L"ヲ", L"ン", L"ｧ", L"ｨ", L"ｩ", L"ｪ", L"ｫ", L"ャ", L"ュ", L"ョ", L"ッ", L"ガ", L"ギ", L"グ", L"ゲ",
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
  for (size_t i = sizeof(display)/sizeof(display[0]); i < 0x1000; ++i) {
    std::wstringstream ss;
    ss << L"{SYMBOL-" << i << L"}";
    MES::displaylist.push_back(ss.str());
    MES::displaymap.insert(std::pair<std::wstring, size_t>(ss.str(), i));
  }

  std::wstring const ops[] = {
    L"ENDPAGE", L"COLOR", L"LOCATION", L"COLORRGBA", L"OP18", L"OP19",
    L"PEOPLE", L"PREVINPUT", L"OP23", L"OP24", L"ITEM", L"CROP",
    L"ORDER", L"OP28", L"VAR", L"GOLD", L"STRUCTURE", L"OP2C",
    L"OP2D", L"PAUSE", L"OP31", L"WAIT", L"OP33", L"PLAYSFX",
    L"TEXTSOUND", L"OP36", L"OP37", L"OP38", L"OP39", L"CHOICEYN",
    L"CHOICE", L"FACE", L"OP52", L"SYMBOL"
  };

  size_t const opnum[] = {
    0x03, 0x10, 0x14, 0x16, 0x18, 0x19,
    0x20, 0x21, 0x23, 0x24, 0x25, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c,
    0x2d, 0x30, 0x31, 0x32, 0x33, 0x34,
    0x35, 0x36, 0x37, 0x38, 0x39, 0x40,
    0x41, 0x50, 0x52, 0x80
    
  };
    
  MES::oplist.resize(256);
  for (size_t i = 0; i < sizeof(ops)/sizeof(ops[0]); ++i) {
    MES::oplist[opnum[i]] = ops[i];
    MES::opmap.insert(std::pair<std::wstring, size_t>(ops[i], opnum[i]));
  }

  for (size_t i = 0x11; i <= 0x17; ++i)
    if (i != 0x14 && i != 0x16)
      MES::opmap.insert(std::pair<std::wstring, size_t>(L"COLOR", i));
    
  for (size_t o = 0, ofs = 0; o < 256; ++o) {
    paramtypeofs[o] = ofs;
    ofs += numparams[o];
  }
}

bool isNumber(std::wstring const& str, size_t ofs, size_t len) {
  return std::all_of(std::begin(str) + ofs, std::begin(str) + (ofs + len), [](wchar_t c){ return std::isdigit(c); });
}

bool isNumber(std::wstring const& str) {
  return isNumber(str, 0, str.size());
}

size_t toNumber(std::wstring const& str) {
  if (str.size() > 2 && (str[1] & -33) == L'X' && str[0] == L'0') {
    return static_cast<size_t>(std::stoll(str, 0, 16));
  } else {
    return std::stoll(str);
  }
}

size_t toNumber(std::string const& str) {
  if (str.size() > 2 && (str[1] & -33) == L'X' && str[0] == L'0') {
    return static_cast<size_t>(std::stoll(str, 0, 16));
  } else {
    return static_cast<size_t>(std::stoll(str));
  }
}

std::wstring widen(std::string const& str) {
  return std::wstring(str.begin(), str.end());
}

std::string toUpper(std::string const& str) {
  std::string copy(str);
  std::transform(std::begin(str), std::end(str), std::begin(copy), [](char c) { return std::toupper(c); });
  return copy;
}

std::wstring trim(std::wstring const& str) {
  std::wstring::const_iterator st = std::find_if_not(str.cbegin(), str.cend(), [](wchar_t c) {return std::isspace(c);}),
                               ed = std::find_if_not(str.crbegin(), str.crend(), [](wchar_t c) {return std::isspace(c);}).base();
  if (std::distance(str.cbegin(), st) > std::distance(str.cbegin(), ed))
    std::swap(st, ed);
  return std::wstring(st, ed);
}

std::string trim(std::string const& str) {
  std::string::const_iterator st = std::find_if_not(str.cbegin(), str.cend(), [](char c) {return std::isspace(c);}),
                              ed = std::find_if_not(str.crbegin(), str.crend(), [](char c) {return std::isspace(c);}).base();
  if (std::distance(str.cbegin(), st) > std::distance(str.cbegin(), ed))
    std::swap(st, ed);
  return std::string(st, ed);
}

wchar_t const hexchars[] = L"0123456789abcdef";

std::wstring toHex(int v, int l) {
  std::wstring str(l + 2, L'0');
  str[1] = L'x';
  for (size_t i = l + 1;i >= 2; --i) {
    str[i] = hexchars[v & 0x0f];
    v >>= 4;
  }
  return str;
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

const size_t MAX_INST_SIZE = 7;
const size_t MAX_PARAM_SIZE = 6;

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
  if (isNumber(colname)){
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
            case 0x80: case 0x81: case 0x82: case 0x83:
            case 0x84: case 0x85: case 0x86: case 0x87:
            case 0x88: case 0x89: case 0x8a: case 0x8b:
            case 0x8c: case 0x8d: case 0x8e: case 0x8f:{
              size_t charidx = ((buffer[buffer_ofs] & 0xff) | op << 8) & 0xfff;
              message.append(MES::displaylist[charidx]);
              break;}
            
            default:{
              std::wstring const& opname = oplist[op];
              if (opname.empty()) {
                message.append(1, L'{').append(wprintNum(op)).append(1, L'}');
              } else {
                size_t num_params = numparams[op];
                size_t param_ofs = paramtypeofs[op];
                message.append(1, L'{').append(opname);
                for (size_t paramidx = 0, buffer_rel = buffer_ofs; paramidx < num_params; ++paramidx) {
                  message.append(1, L'-');
                  switch (paramtypes[param_ofs + paramidx]) {
                    case PARAM_BYTE:
                      message.append(wprintNum(buffer[buffer_rel++] & 0xff));
                      break;
                    case PARAM_SHORT:
                      message.append(wprintNum((buffer[buffer_rel] & 0xff) << 8 | (buffer[buffer_rel + 1] & 0xff)));
                      buffer_rel += 2;
                      break;
                    case PARAM_INT:
                      message.append(wprintNum(buffer[buffer_rel] << 24 | (buffer[buffer_rel + 1] & 0xff) << 16 |
                                          (buffer[buffer_rel + 2] & 0xff) << 8 | (buffer[buffer_rel + 3] & 0xff)));
                      buffer_rel += 4;
                      break;
                    case PARAM_ITEMID:
                      message.append(getItemName((buffer[buffer_rel] & 0xff) << 8 | (buffer[buffer_rel + 1] & 0xff)));
                      buffer_rel += 2;
                      break;
                    case PARAM_COLORID:
                      message.append(getColor(opToCol[op - 0x10]));
                      break;
                    case PARAM_RGBA:
                      message.append(toHex(buffer[buffer_rel] << 24 | (buffer[buffer_rel + 1] & 0xff) << 16 |
                                          (buffer[buffer_rel + 2] & 0xff) << 8 | (buffer[buffer_rel + 3] & 0xff), 8));
                      buffer_rel += 4;
                      break;
                    case PARAM_PEOPLEID:
                      message.append(getPeopleName(buffer[buffer_rel++] & 0xff));
                      break;
                    case PARAM_STRUCTUREID:
                      message.append(getStructureName(buffer[buffer_rel++] & 0xff));
                      break;
                    case PARAM_CROPID:
                      message.append(getCropName((buffer[buffer_rel] & 0xff) << 8 | (buffer[buffer_rel + 1] & 0xff)));
                      buffer_rel += 2;
                      break;
                   case PARAM_SFXID:
                      message.append(getSFXName((buffer[buffer_rel] & 0xff) << 8 | (buffer[buffer_rel + 1] & 0xff)));
                      buffer_rel += 2;
                      break;
                  }
                }
                message.append(1, L'}');
              }
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
  const size_t BUFFER_MAX = 4096 - (4 + MAX_INST_SIZE + 1);
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
  std::wstring params[MAX_PARAM_SIZE];
  
  size_t prev_data_ofs = data_ofs;
  for (size_t i = 0; i < this->messages.size(); ++i) {
    std::wstring const& message = this->messages[i];
    bool dupe = (SEQ_DEDUPE && this->messages[i - 1] == message);
    if (dupe) {
      offsets[i] = prev_data_ofs;
    } else {
      offsets[i] = data_ofs;
      prev_data_ofs = data_ofs;
      for (size_t char_ofs = 0; char_ofs < message.size(); ++char_ofs) {
        if (buffer_len > BUFFER_MAX) {
          outfile.write(buffer, buffer_len);
          buffer_len = 0;
        }
        wchar_t first_char = message[char_ofs];
        if (first_char == L'{') {
          size_t first_bracket = char_ofs;
          if (std::isdigit(message[++char_ofs])) {
            size_t eos = message.find(L'}', char_ofs);
            std::wstring param = message.substr(char_ofs, eos - char_ofs);
            if (isNumber(param)) {
              buffer[buffer_len++] = std::stoi(param);
              ++data_ofs;
            } else {
              
            }
            char_ofs = eos;
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
              size_t expected_params = numparams[op];
              while (numops < expected_params && message[char_ofs] != '}') {
                if (message[char_ofs] == '-')
                  ++char_ofs;
                size_t param_start = char_ofs;
                while (message[char_ofs] != '}' && message[char_ofs] != '-')
                  ++char_ofs;
                std::wstring param = message.substr(param_start, char_ofs - param_start);
                params[numops++] = param;
              }

              if (numops < expected_params) {
                // error insufficient params
              } else {
                for (size_t pidx = 0, pofs = paramtypeofs[op], buffer_ofs = buffer_len + 1, pval; pidx < numops; ++pidx) {
                  switch (paramtypes[pofs++]) {
                    case PARAM_BYTE:
                      buffer[buffer_ofs++] = toNumber(params[pidx]);
                      break;
                    case PARAM_SHORT:
                      pval = toNumber(params[pidx]);
                      buffer[buffer_ofs] = pval >> 8;
                      buffer[buffer_ofs + 1] = pval;
                      buffer_ofs += 2;
                      break;
                    case PARAM_COLORID:
                      buffer[buffer_len] += colToOp[getColor(params[pidx])];
                      break;
                    case PARAM_PEOPLEID:
                      buffer[buffer_ofs++] = getPeopleIndex(params[pidx]);
                      break;
                    case PARAM_ITEMID:
                      pval = getItemIndex(params[pidx]);
                      buffer[buffer_ofs] = pval >> 8;
                      buffer[buffer_ofs + 1] = pval;
                      buffer_ofs += 2;
                      break;
                    case PARAM_STRUCTUREID:
                      buffer[buffer_ofs++] = getStructureIndex(params[pidx]);
                      break;
                    case PARAM_SFXID:
                      pval = getSFXIndex(params[pidx]);
                      buffer[buffer_ofs] = pval >> 8;
                      buffer[buffer_ofs + 1] = pval;
                      buffer_ofs += 2;
                      break;
                    case PARAM_RGBA:
                      pval = toNumber(params[pidx]);
                      buffer[buffer_ofs] = pval >> 24;
                      buffer[buffer_ofs + 1] = pval >> 16;
                      buffer[buffer_ofs + 2] = pval >> 8;
                      buffer[buffer_ofs + 3] = pval;
                      buffer_ofs += 4;
                      break;
                    case PARAM_CROPID:
                      pval = getCropIndex(params[pidx]);
                      buffer[buffer_ofs] = pval >> 8;
                      buffer[buffer_ofs + 1] = pval;
                      buffer_ofs += 2;
                      break;
                    case PARAM_SYMBOL:
                      pval = toNumber(params[pidx]);
                      buffer[buffer_len] += pval >> 8;
                      buffer[buffer_ofs++] = pval;
                      break;
                    case PARAM_INT:
                      pval = toNumber(params[pidx]);
                      buffer[buffer_ofs] = pval >> 24;
                      buffer[buffer_ofs + 1] = pval >> 16;
                      buffer[buffer_ofs + 2] = pval >> 8;
                      buffer[buffer_ofs + 3] = pval;
                      buffer_ofs += 4;
                  }
                }
              }
              buffer_len += size;
              data_ofs += size;
            } else {
              std::wstring skey;
              skey.append(1, L'{').append(opstr).append(1, L'}');
              it = MES::displaymap.find(skey);
              if (it != MES::displaymap.end()) {
                size_t index = it->second;
                buffer[buffer_len] = 0x80 | (index >> 8 & 0x0f);
                buffer[buffer_len + 1] = index;
                buffer_len += 2;
                data_ofs += 2;
              } else {
                // error unrecognized op
              }
            }
          }
          
          size_t last_bracket = message.find(L'}', first_bracket + 2);
          char_ofs = last_bracket;
        } else {
          if (first_char == L' ') {
            buffer[buffer_len++] = 0x02;
            ++data_ofs;
          } else if (first_char == L'\n') {
            buffer[buffer_len++] = 0x01;
            ++data_ofs;
          } else {
            std::wstring ws(1, first_char);
            std::unordered_map<std::wstring, size_t>::iterator it = MES::displaymap.find(ws);
            if (it != MES::displaymap.end()) {
              buffer[buffer_len] = 0x80 + (it->second >> 8 & 0x0f);
              buffer[buffer_len + 1] = it->second;
              buffer_len += 2;
              data_ofs += 2;
            } else { // invalid character
              
            }
          }
        }
      }
      buffer[buffer_len++] = 0;
      ++data_ofs;
      while (data_ofs & 3) {
        buffer[buffer_len++] = 0;
        ++data_ofs;
      }
    }
  }
  if (buffer_len)
    outfile.write(buffer, buffer_len);
  
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
    mes.strip();
    mlist.resize(mes.messages.size());
    for (size_t i = 0; i < mes.messages.size(); ++i) {
      std::wstring upper = toUpper(trim(mes.messages[i])); // crash
      if (mmap.count(upper)) {
        size_t sz = upper.size();
        upper.resize(sz += 4);
        upper[sz - 4] = L' ';
        upper[sz - 3] = L'(';
        upper[sz - 1] = L')';
        size_t nextsize = 10;
        size_t vers = 1;
        do {
          if (++vers == nextsize) {
            upper.resize(++sz);
            upper[sz - 1] = L')';
            nextsize *= 10;
          }
          size_t v2 = vers, s2 = sz - 1;
          while (v2 > 0) {
            upper[--s2] = L'0' + v2 % 10;
            v2 /= 10;
          }
        } while (mmap.count(upper));
      }
      mlist[i] = upper;
      mmap.insert(std::pair<std::wstring, size_t>(upper, i));
    }
  } else {
    fail_flag = true;
  }
}


void MES::loadPeople() {
  if (MES::use_people_names && MES::peoplemap.empty() && !MES::load_people_failed) {
    MES::use_people_names = false;
    MES::loadMES("./people.mes", MES::peoplelist, MES::peoplemap, MES::load_people_failed);
    if (!MES::load_people_failed)
      MES::use_people_names = true;
  }
}

void MES::loadStructures() {
  if (MES::use_structure_names && MES::structuremap.empty() && !MES::load_structures_failed) {
    MES::use_structure_names = false;
    MES::loadMES("./structure.mes", MES::structurelist, MES::structuremap, MES::load_structures_failed);
    if (!MES::load_structures_failed)
      MES::use_structure_names = true;
  }
}

void MES::loadItems() {
  if (MES::use_item_names && MES::itemmap.empty() && !MES::load_items_failed) {
    MES::use_item_names = false;
    MES::loadMES("./item.mes", MES::itemlist, MES::itemmap, MES::load_items_failed);
    if (!MES::load_items_failed)
      MES::use_item_names = true;
  }
}

void MES::loadCrops() {
  if (MES::use_crop_names && MES::cropmap.empty() && !MES::load_crops_failed) {
    MES::use_crop_names = false;
    MES::loadMES("./farmcrop.mes", MES::croplist, MES::cropmap, MES::load_crops_failed);
    if (!MES::load_crops_failed)
      MES::use_crop_names = true;
  }
}

void MES::loadSFXs() {
  if (MES::use_sfx_names && MES::sfxmap.empty() && !MES::load_sfxs_failed) {
    MES::use_sfx_names = false;
    
    std::ifstream sfxfile;
    sfxfile.open("./SFXList.txt");
    if (sfxfile) {
      std::string line;
      while (std::getline(sfxfile, line) && !line.empty()) {
        size_t col = line.find_first_of(':');
        size_t num = toNumber(trim(line.substr(0, col)));
        size_t com = line.find_last_of(',');
        if (com != std::string::npos)
          com -= col + 1;
        std::string sfxname = trim(line.substr(col + 1, com));
        std::wstring upper = toUpper(widen(sfxname));
        MES::sfxmap.insert(std::pair<std::wstring, size_t>(upper, num));
        MES::sfxlist.insert(std::pair<size_t, std::wstring>(num, upper));
      }
      sfxfile.close();
    }
    
    if (!MES::load_sfxs_failed)
      MES::use_sfx_names = true;
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
  bool dig = isNumber(name);
  if (!dig)
    MES::loadPeople();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::wstring key = toUpper(trim(name));
    std::unordered_map<std::wstring, size_t>::iterator it = MES::peoplemap.find(key);
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
  bool dig = isNumber(name);
  if (!dig)
    MES::loadStructures();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::wstring key = toUpper(trim(name));
    std::unordered_map<std::wstring, size_t>::iterator it = MES::structuremap.find(key);
    if (it != MES::structuremap.end()) {
      index = it->second;
    } else {
      // error, structure not found
    }
  }
  return index;
}

std::wstring MES::getItemName(size_t index) {
  MES::loadItems();
  
  std::wstringstream ss;
  if (!MES::use_item_names || index >= MES::itemlist.size()) {
    ss << index;
  } else {
    ss << MES::itemlist[index];
  }
  return ss.str();
}

size_t MES::getItemIndex(std::wstring const& name) {
  bool dig = isNumber(name);
  if (!dig)
    MES::loadItems();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::wstring key = toUpper(trim(name));
    std::unordered_map<std::wstring, size_t>::iterator it = MES::itemmap.find(key);
    if (it != MES::itemmap.end()) {
      index = it->second;
    } else {
      // error, item not found
    }
  }
  return index;
}

std::wstring MES::getCropName(size_t index) {
  MES::loadCrops();
  
  std::wstringstream ss;
  if (!MES::use_crop_names || index >= MES::croplist.size()) {
    ss << index;
  } else {
    ss << MES::croplist[index];
  }
  return ss.str();
}

size_t MES::getCropIndex(std::wstring const& name) {
  bool dig = isNumber(name);
  if (!dig)
    MES::loadCrops();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::wstring key = toUpper(trim(key));
    std::unordered_map<std::wstring, size_t>::iterator it = MES::cropmap.find(key);
    if (it != MES::cropmap.end()) {
      index = it->second;
    } else {
      // error, crop not found
    }
  }
  return index;
}

std::wstring MES::getSFXName(size_t index) {
  MES::loadSFXs();
  
  std::wstringstream ss;
  std::unordered_map<size_t, std::wstring>::iterator it = MES::sfxlist.find(index);
  if (!MES::use_sfx_names || it == MES::sfxlist.end()) {
    ss << index;
  } else {
    ss << it->second;
  }
  return ss.str();
}

size_t MES::getSFXIndex(std::wstring const& name) {
  bool dig = isNumber(name);
  if (!dig)
    MES::loadSFXs();
  
  size_t index = 0;
  if (dig) {
    index = std::stoi(name);
  } else {
    std::wstring key = toUpper(trim(name));
    std::unordered_map<std::wstring, size_t>::iterator it = MES::sfxmap.find(key);
    if (it != MES::sfxmap.end()) {
      index = it->second;
    } else {
      // error, sfx not found
    }
  }
  return index;
}

void MES::strip() {
  for (size_t m = 0; m < this->messages.size(); ++m) {
    size_t sub = 0;
    std::wstring& str = this->messages[m];
    for (size_t o = 0; o < str.size(); ++o) {
      wchar_t c = str[o];
      if (c == L'-')
        c = L' ';
      str[o - sub] = c;
      if (c == L'{' || c == L'}')
        ++sub;
    }
    str.resize(str.size() - sub);
  }
}