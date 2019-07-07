#include "MES.h"

#include <sstream>
#include <iostream>
#include <locale>
#include <algorithm>
#include <utility>
#include <codecvt>
#include <cwctype>

#ifdef DUPE2
const bool NSQ_DEDUPE = true;
#else
const bool NSQ_DEDUPE = false;  
#ifdef DUPE
const bool SEQ_DEDUPE = true;
#else
const bool SEQ_DEDUPE = false;
#endif
#endif

std::unordered_map<std::wstring, MES::Decoder> MES::decoder_map;

// base param types
size_t const PARAM_BYTE = 0,
             PARAM_SHORT = 1,
             PARAM_COLORID = 2,
             PARAM_RGBA = 3,
             PARAM_INT = 4,
             PARAM_SYMBOL = 5;
size_t const PARAM_INDEX = PARAM_SYMBOL + 1;

// add hex
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

std::wstring widen(std::string const& str) { // naive unicode conversions (7 bit ascii)
  return std::wstring(str.begin(), str.end());
}

std::string narrow(std::wstring const& wstr) {
  return std::string(wstr.begin(), wstr.end());
}

std::string toUpper(std::string const& str) {
  std::string copy(str);
  std::transform(std::begin(str), std::end(str), std::begin(copy), [](char c) { return std::toupper(c); });
  return copy;
}

std::wstring trim(std::wstring const& str) {
  std::wstring::const_iterator st = std::find_if_not(str.cbegin(), str.cend(), [](wchar_t c) {return std::iswspace(c);}),
                               ed = std::find_if_not(str.crbegin(), str.crend(), [](wchar_t c) {return std::iswspace(c);}).base();
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
    ofs = 0;
    
    while (result && len < req && len < buffer_len) {
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
  std::ctype<wchar_t> const& facet = std::use_facet<std::ctype<wchar_t>>(std::locale());
  facet.toupper(&copy[0], &copy[0] + copy.size());
  return copy;
}

std::wstring toLower(std::wstring const& str) {
  std::wstring copy(str);
  std::ctype<wchar_t> const& facet = std::use_facet<std::ctype<wchar_t>>(std::locale());
  facet.tolower(&copy[0], &copy[0] + copy.size());
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

std::wstring getColor(size_t colnum, MES::Decoder& decoder) {
  std::unordered_map<size_t, std::wstring>::iterator it = decoder.op2color.find(colnum);
  std::wstring colname;
  if (it != decoder.op2color.end()) {
    colname = it->second;
  } else {
    colname = wprintNum(colnum);
  }
  return colname;
}

size_t getColor(std::wstring const& colname, MES::Decoder& decoder) {
  size_t col = 0;
  if (isNumber(colname)){
    col = std::stoi(colname);
  } else {
    std::unordered_map<std::wstring, size_t>::iterator it = decoder.color2op.find(colname);
    if (it != decoder.color2op.end()) {
      col = it->second;
    }
  }
  return col;
}

std::wstring& makesafe(std::wstring& ws) {
  for (size_t i = 0; i < ws.size(); ++i) {
    if (ws[i] == L'-')
      ws[i] = L'_';
  }
  return ws;
}

size_t readInt(char* array, bool msb) {
  return msb ? array[0] << 24 | (array[1] & 0xff) << 16 | (array[2] & 0xff) << 8 | (array[3] & 0xff)
         : (array[0] & 0xff) | (array[1] & 0xff) << 8 | (array[2] & 0xff) << 16 | array[3] << 24;
}

bool MES::readMES(std::ifstream& infile, std::wstring const& decoder_path, bool nospecial) {
  MES::Decoder * decoder = MES::getDecoder(decoder_path);
  
  char buffer[4096];
  infile.read(buffer, sizeof(buffer));
  size_t buffer_len = infile.gcount();
  size_t buffer_ofs = 8;
  size_t signature = readInt(buffer, decoder->isMSB);
  bool validsig = (signature == 0xcdc3b0b0); // MC00
  
  bool result = validsig;
  
  if (result) {
    size_t num_messages = readInt(&buffer[4], decoder->isMSB);
    this->messages.resize(num_messages);
    size_t* offsets = new size_t[num_messages];
    result = true;
    for (size_t m = 0; result && m < num_messages; ++m) {
      if (ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, 4)) {
        offsets[m] = readInt(&buffer[buffer_ofs], decoder->isMSB);
        buffer_ofs += 4;
      } else
        result = false;
    }
    size_t data_ofs = num_messages * 4 + 8;
    if (result) {
      for (size_t m = 0; result && m < num_messages; ++m) { // read the messages
        if (data_ofs != offsets[m]) {
          if (offsets[m] < data_ofs && (data_ofs - offsets[m]) <= buffer_ofs) {
            buffer_ofs -= data_ofs - offsets[m];
          } else {
            if (infile.eof())
              infile.clear();
            infile.seekg(data_ofs, std::ios::beg);
            buffer_ofs = buffer_len;
          }
          data_ofs = offsets[m];
        }
        std::wstring message;
        bool eom = false;
        while (!eom) {
          if (!ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, 1)) {
            result = false;
          }
          size_t op = buffer[buffer_ofs++] & 0xff;
          size_t required_length = decoder->opsize[op] - 1;
          if (!ensureBuffer(infile, buffer, sizeof(buffer), buffer_ofs, buffer_len, required_length)) {
            result = false;
          }
          switch (op) {
            case 0x00:
              eom = true;
              break;
            default:{
              if (decoder->opsingle[op] == 1) {
                if (!nospecial || decoder->singlelist[op][0] != L'{')
                  message.append(decoder->singlelist[op]);
              } else if (decoder->opsingle[op] == 2) { // symbol
                size_t symbolidx = (op << 8 | (buffer[buffer_ofs] & 0xff)) & 0x0fff;
                if (!nospecial || (!decoder->displaylist[symbolidx].empty() && decoder->displaylist[symbolidx][0] != L'{')) {
                  message.append(decoder->displaylist[symbolidx]);
                }
              } else {
                std::wstring const& opname = decoder->oplist[op];
                if (opname.empty()) { // single character
                  if (!nospecial)
                    message.append(1, L'{').append(wprintNum(op)).append(1, L'}');
                } else {
                  size_t num_params = decoder->opparamnum[op];
                  size_t param_ofs = decoder->opparamofs[op];
                  if (nospecial) {
                    if (num_params == 1 && decoder->opparamtypes[param_ofs] >= PARAM_INDEX) {
                      size_t ptype = decoder->opparamtypes[param_ofs];
                      size_t param_size = decoder->paramsize[ptype];
                      size_t idx = 0;
                      switch (param_size) {
                        case 4:
                          idx |= (buffer[buffer_ofs + param_size - 4] & 0xff) << 24;
                          /* fall through */
                        case 3:
                          idx |= (buffer[buffer_ofs + param_size - 3] & 0xff) << 16;
                          /* fall through */
                        case 2:
                          idx |= (buffer[buffer_ofs + param_size - 2] & 0xff) << 8;
                          /* fall through */
                        case 1:
                          idx |= (buffer[buffer_ofs + param_size - 1] & 0xff);
                      }
                      if (decoder->idxenablenames[ptype - PARAM_INDEX] && idx < decoder->idxlist[ptype - PARAM_INDEX].size()) {
                        message.append(decoder->idxlist[ptype - PARAM_INDEX][idx]);
                      }
                    }
                  } else {
                    message.append(1, L'{').append(opname);
                    for (size_t paramidx = 0, buffer_rel = buffer_ofs; paramidx < num_params; ++paramidx) {
                      size_t paramtype = decoder->opparamtypes[param_ofs + paramidx];
                      size_t param_size = decoder->paramsize[paramtype];
                      message.append(1, L'-');
                      switch (paramtype) {
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
                        case PARAM_COLORID:
                          message.append(getColor(op, *decoder));
                          break;
                        case PARAM_RGBA:
                          message.append(toHex(buffer[buffer_rel] << 24 | (buffer[buffer_rel + 1] & 0xff) << 16 |
                                              (buffer[buffer_rel + 2] & 0xff) << 8 | (buffer[buffer_rel + 3] & 0xff), 8));
                          buffer_rel += 4;
                          break;
                        default:{ // index param
                          size_t idx = 0;
                          switch (param_size) {
                            case 4:
                              idx |= (buffer[buffer_rel + param_size - 4] & 0xff) << 24;
                              /* fall through */
                            case 3:
                              idx |= (buffer[buffer_rel + param_size - 3] & 0xff) << 16;
                              /* fall through */
                            case 2:
                              idx |= (buffer[buffer_rel + param_size - 2] & 0xff) << 8;
                              /* fall through */
                            case 1:
                              idx |= (buffer[buffer_rel + param_size - 1] & 0xff);
                          }
                          if (decoder->idxenablenames[paramtype - PARAM_INDEX] && idx < decoder->idxlist[paramtype - PARAM_INDEX].size() && !decoder->idxlist[paramtype - PARAM_INDEX][idx].empty())
                            message.append(decoder->idxlist[paramtype - PARAM_INDEX][idx]);
                          else
                            message.append(wprintNum(idx));
                          buffer_rel += param_size;
                          break;}
                      }
                    }
                    message.append(1, L'}');
                  }
                }
              }
            }
          }
          
          buffer_ofs += required_length;
          data_ofs += required_length + 1;
        }
        if (nospecial)
          makesafe(message);
        this->messages[m] = message;
        while (buffer_ofs < buffer_len && buffer[buffer_ofs] == 0) {
          ++buffer_ofs;
          ++data_ofs;
        }
      }
    }
    delete[] offsets;
  } else { // ERROR: invalid signature
    
  }
  
  return result;
}

void writeInt(char* array, size_t ofs, size_t val, bool msb) {
  if (msb) {
    array[ofs] = val >> 24;
    array[ofs + 1] = val >> 16;
    array[ofs + 2] = val >> 8;
    array[ofs + 3] = val;
  } else {
    array[ofs] = val;
    array[ofs + 1] = val >> 8;
    array[ofs + 2] = val >> 16;
    array[ofs + 3] = val >> 24;
  }
}

void MES::writeMES(std::ofstream& outfile, std::wstring const& decoder_path) const {
  MES::Decoder * decoder = MES::getDecoder(decoder_path);
  
  const size_t BUFFER_MAX = 4096 - (4 + MAX_INST_SIZE + 1);
  char buffer[4096];
  
  writeInt(buffer, 0, 0xcdc3b0b0u, decoder->isMSB);
  writeInt(buffer, 4, this->messages.size(), decoder->isMSB);

  outfile.write(buffer, 8);
  size_t data_ofs = this->messages.size() * 4 + 8;
  size_t buffer_len = 0;
  outfile.seekp(data_ofs, std::ios::beg);
  size_t* offsets = new size_t[this->messages.size()];
  std::vector<std::wstring> params;
  
  #ifdef DUPE2
  std::unordered_map<std::wstring, size_t> prev_msg_ofs;
  #endif
  
  size_t prev_data_ofs = data_ofs;
  for (size_t i = 0; i < this->messages.size(); ++i) {
    std::wstring const& message = this->messages[i];
    #ifdef DUPE2
    std::unordered_map<std::wstring, size_t>::iterator it = prev_msg_ofs.find(message);
    bool dupe = (it != prev_msg_ofs.end());
    #else
    bool dupe = (SEQ_DEDUPE && this->messages[i - 1] == message);
    #endif
    if (dupe) {
      #ifdef DUPE2
      prev_data_ofs = it->second;
      #endif
      offsets[i] = prev_data_ofs;
    } else {
      #ifdef DUPE2
      prev_msg_ofs.insert(std::pair<std::wstring, size_t>(message, data_ofs));
      #endif
      offsets[i] = data_ofs;
      #ifndef DUPE2
      prev_data_ofs = data_ofs;
      #endif
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
            params.clear();
            std::wstring opstr = message.substr(opstart, char_ofs - opstart);
            std::unordered_map<std::wstring, size_t>::iterator it = decoder->opmap.find(opstr);
            if (it != decoder->opmap.end()) {
              size_t op = it->second;
              size_t size = decoder->opsize[op];
              buffer[buffer_len] = op;
              size_t expected_params = decoder->opparamnum[op];
              while (params.size() < expected_params && message[char_ofs] != '}') {
                if (message[char_ofs] == '-')
                  ++char_ofs;
                size_t param_start = char_ofs;
                while (message[char_ofs] != '}' && message[char_ofs] != '-')
                  ++char_ofs;
                std::wstring param = message.substr(param_start, char_ofs - param_start);
                params.push_back(param);
              }

              if (params.size() < expected_params) {
                // error insufficient params
              } else {
                for (size_t pidx = 0, pofs = decoder->opparamofs[op], buffer_ofs = buffer_len + 1, pval; pidx < expected_params; ++pidx) {
                  size_t ptype = decoder->opparamtypes[pofs++];
                  switch (ptype) {
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
                      buffer[buffer_len] = getColor(params[pidx], *decoder);
                      break;
                    case PARAM_RGBA:
                      pval = toNumber(params[pidx]);
                      buffer[buffer_ofs] = pval >> 24;
                      buffer[buffer_ofs + 1] = pval >> 16;
                      buffer[buffer_ofs + 2] = pval >> 8;
                      buffer[buffer_ofs + 3] = pval;
                      buffer_ofs += 4;
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
                      break;
                    default:{ // index param
                      size_t idxtype = ptype - PARAM_INDEX;
                      size_t idx;
                      std::unordered_map<std::wstring, size_t>::iterator it2 = decoder->idxmap[idxtype].find(params[pidx]);
                      if (it2 != decoder->idxmap[idxtype].end()) {
                        idx = it2->second;
                      } else if (isNumber(params[pidx])) {
                        idx = toNumber(params[pidx]);
                      }
                      size_t psize = decoder->paramsize[ptype];
                      switch (psize) {
                        case 4:
                          buffer[buffer_ofs + psize - 4] = idx >> 24;
                          /* fall through */
                        case 3:
                          buffer[buffer_ofs + psize - 3] = idx >> 16;
                          /* fall through */
                        case 2:
                          buffer[buffer_ofs + psize - 2] = idx >> 8;
                          /* fall through */
                        case 1:
                          buffer[buffer_ofs + psize - 1] = idx;
                          /* fall through */
                      }
                      buffer_ofs += psize;
                    break;}
                  }
                }
              }
              buffer_len += size;
              data_ofs += size;
            } else {
              std::wstring skey;
              skey.append(1, L'{').append(opstr).append(1, L'}');
              it = decoder->displaymap.find(skey);
              if (it != decoder->displaymap.end()) {
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
          std::wstring ws(1, first_char);
          std::unordered_map<std::wstring, size_t>::iterator it2 = decoder->singlemap.find(ws);
          if (it2 != decoder->singlemap.end()) {
            buffer[buffer_len++] = it2->second;
            ++data_ofs;
          } else {
            std::unordered_map<std::wstring, size_t>::iterator it = decoder->displaymap.find(ws);
            if (it != decoder->displaymap.end()) {
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
  
  if (data_ofs & 31) {
    for (size_t i = 0; i < (-data_ofs & 31); ++i)
      buffer[i] = 0;
    outfile.write(buffer, -data_ofs & 31);
  }
  
  outfile.seekp(8, std::ios::beg);
  buffer_len = 0;
  for (size_t m = 0; m < this->messages.size(); ++m) {
    if (buffer_len > BUFFER_MAX) {
      outfile.write(buffer, buffer_len);
      buffer_len = 0;
    }
    writeInt(buffer, buffer_len, offsets[m], decoder->isMSB);
    buffer_len += 4;
  }
  if (buffer_len)
    outfile.write(buffer, buffer_len);
}

bool MES::readText(std::wifstream& infile, std::wstring const& decoder_path) {
  MES::Decoder * decoder = MES::getDecoder(decoder_path);
  (void)decoder;
  
  std::wstring line;
  std::wstring message;
  size_t messagenum = 0;
  bool messageline = true;
  while (std::getline(infile, line)) {
    size_t ofs;
    if (!line.empty() && line[0] == L'\ufeff')
      line.erase(0, 1);
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

void MES::writeText(std::wofstream& outfile, std::wstring const& decoder_path) const {
  MES::Decoder * decoder = MES::getDecoder(decoder_path);
  (void)decoder;
  
  for (size_t i = 0; i < this->messages.size(); ++i) {
    if (i)
      outfile << std::endl;
    outfile << "--- message " << i << " ---" << std::endl;
    outfile << this->messages[i];
  }
}

void MES::loadMES(std::wstring const& decoder_path, std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag) {
  std::ifstream mesfile;
  mesfile.open(filepath, std::ios_base::in | std::ios_base::binary);
  if (mesfile) {
    MES mes;
    mes.readMES(mesfile, decoder_path, true);
    mesfile.close();
    mlist.resize(mes.messages.size());
for (size_t i = 0; i < mes.messages.size(); ++i) {
      std::wstring upper = toUpper(trim(mes.messages[i]));
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

void MES::loadList(std::string const& filepath, std::vector<std::wstring>& mlist, std::unordered_map<std::wstring, size_t>& mmap, bool& fail_flag) {
  std::wifstream sfxfile;
  sfxfile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
  sfxfile.open(filepath);
  if (sfxfile) {
    std::wstring line;
    while (std::getline(sfxfile, line) && !line.empty()) {
      if (line[0] == L'\ufeff')
        line.erase(0, 1);
      size_t col = line.find_first_of(L':');
      size_t num = toNumber(trim(line.substr(0, col)));
      size_t com = line.find_last_of(L',');
      if (com != std::string::npos)
        com -= col + 1;
      std::wstring sfxname = trim(line.substr(col + 1, com));
      std::wstring upper = toUpper(sfxname);
      mmap.insert(std::pair<std::wstring, size_t>(upper, num));
      if (mlist.size() <= num)
        mlist.resize(num + 1);
      mlist[num] = upper;
    }
    sfxfile.close();
  } else {
    fail_flag = true;
  }
}

void MES::loadSymbols(std::wifstream& infile, MES::Decoder& decoder) {
  std::wstring line;
  while (std::getline(infile, line)) {
    if (!line.empty() && line[0] == L'\ufeff')
      line.erase(0, 1);
    for (size_t i = 0; i < line.length(); ++i) {
      wchar_t wc = line[i];
      if (wc == L'{') {
        size_t e = line.find_first_of(L'}', i + 2);
        if (e != std::string::npos) {
          std::wstring ws = line.substr(i, e + 1 - i);
          decoder.displaymap.insert(std::pair<std::wstring, size_t>(ws, decoder.displaylist.size()));
          decoder.displaylist.push_back(ws);
          i = e;
        } else { // ERROR: unterminated entity
          i = line.length();
        }
      } else if (wc == L' ') { // undefined character
        std::wstring ws = L"{SYMBOL-";
        ws.append(wprintNum(decoder.displaylist.size())).append(1, L'}');
        decoder.displaymap.insert(std::pair<std::wstring, size_t>(ws, decoder.displaylist.size()));
        decoder.displaylist.push_back(ws);
      } else {
        std::wstring ws(1, wc);
        if (decoder.displaymap.count(ws)) { // WARNING: duplicate character
          
        } else {
          decoder.displaymap.insert(std::pair<std::wstring, size_t>(ws, decoder.displaylist.size()));
        }
        decoder.displaylist.push_back(ws);
      }
    }
  }
  while (decoder.displaylist.size() < 0x1000) {
    std::wstring ws = L"{SYMBOL-";
    ws.append(wprintNum(decoder.displaylist.size())).append(1, L'}');
    decoder.displaymap.insert(std::pair<std::wstring, size_t>(ws, decoder.displaylist.size()));
    decoder.displaylist.push_back(ws);
  }
}

void MES::loadSingleByte(std::wifstream& infile, MES::Decoder& decoder) {
  std::wstring line;
  size_t ofs = 0,
        size = 0;
  while (std::getline(infile, line)) {
    if (!line.empty()) {
      if (line[0] == L'\ufeff')
        line.erase(0, 1);
      if (size) { // add characters
        bool exact = false;
        if (line[0] == L'"' && line[line.size() - 1] == L'"') {
          exact = true;
          size_t lofs = 0;
          for (size_t i = 1; i < line.size() - 1; ++i) {
            if (line[i] == L'\\') {
              switch (line[++i]) {
                case L'n':
                  line[lofs++] = L'\n';
                  break;
                case L'r':
                  line[lofs++] = L'\r';
                  break;
                case L't':
                  line[lofs++] = L'\t';
                  break;
                case L'b':
                  line[lofs++] = L'\b';
                  break;
                default:
                  line[lofs++] = line[i];
              }
            } else
              line[lofs++] = line[i];
          }
          line.resize(lofs);
        }
        
        size_t lofs = 0;
        while (lofs < line.size() && size > 0) {
          wchar_t wc = line[lofs];
          if (wc == L'{') {
            size_t lend = line.find_first_of(L'}', lofs + 1);
            if (lend != std::string::npos) {
              std::wstring entity = line.substr(lofs, lend - lofs);
              decoder.opsingle[ofs] = true;
              decoder.singlemap.insert(std::pair<std::wstring, size_t>(entity, ofs));
              decoder.singlelist[ofs] = entity;
              ++ofs;
              --size;
              lofs = lend + 1;
            } else {
              lofs = line.size();
            }
          } else if (!exact && wc == L' ') {
            ++ofs;
            --size;
          } else {
            std::wstring wcs(1, wc);
            decoder.opsingle[ofs] = 1;
            decoder.singlemap.insert(std::pair<std::wstring, size_t>(wcs, ofs));
            decoder.singlelist[ofs] = wcs;
            ++ofs;
            --size;
            ++lofs;
          }
        }
        
      } else { // get range
        size_t hofs = line.find_first_of(L'#');
        if (hofs) {
          line = line.substr(0, hofs);
        }
        hofs = line.find_first_of(L'-');
        std::wstring start, end;
        start = trim(line.substr(0, hofs));
        if (hofs != std::string::npos) {
          end = trim(line.substr(hofs + 1));
          if (isNumber(start) && isNumber(end)) {
            ofs = toNumber(start);
            size = toNumber(end) - ofs + 1;
          }
        } else {
          if (isNumber(start)) {
            ofs = toNumber(start);
            size = std::string::npos;
          }
        }
      }
    } else {
      size = 0;
    }
  }
}

MES::Decoder* MES::getDecoder(std::wstring const& decoder_path) {
  std::wstring const dpath_lower = toLower(decoder_path);
  MES::Decoder * decoder_ptr = nullptr;
  std::unordered_map<std::wstring, MES::Decoder>::iterator it = MES::decoder_map.find(dpath_lower);
  if (it != MES::decoder_map.end()) {
    decoder_ptr = &it->second;
  } else {
    std::wifstream infile;
    infile.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
    infile.open(narrow(decoder_path));
    if (!infile) {
      std::wcout << L"Can't open file " << decoder_path << std::endl;
    } else {
      std::pair<std::unordered_map<std::wstring, MES::Decoder>::iterator, bool> insit = MES::decoder_map.insert(std::pair<std::wstring, MES::Decoder>(dpath_lower, MES::Decoder()));
      MES::Decoder& decoder = insit.first->second;
      decoder_ptr = &decoder;
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_BYTE", PARAM_BYTE));
      decoder.paramsize.push_back(1);
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_SHORT", PARAM_SHORT));
      decoder.paramsize.push_back(2);
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_COLORID", PARAM_COLORID));
      decoder.paramsize.push_back(0);
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_RGBA", PARAM_RGBA));
      decoder.paramsize.push_back(4);
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_INT", PARAM_INT));
      decoder.paramsize.push_back(4);
      decoder.parammap.insert(std::pair<std::wstring, size_t>(L"PARAM_SYMBOL", PARAM_SYMBOL));
      decoder.paramsize.push_back(1);
      
      //decoder.displaymap.clear();
      //decoder.displaylist.clear();
      
      decoder.opsingle.resize(256);
      //decoder.singlemap.clear();
      decoder.singlelist.resize(256);
      
      std::wstring symbols_path;
      std::wstring chars_path;
      
      size_t state = 0;
      
      std::wstring line;
      
      decoder.oplist.resize(256);

      //decoder.opmap.clear();
      
      decoder.opsize.resize(256);
      decoder.opparamnum.resize(256);
      decoder.opparamofs.resize(256);
      //decoder.opparamtypes.clear();
      
      for (size_t i = 0; i < 256; ++i) {
        decoder.opparamofs[i] = 0;
        decoder.opsize[i] = 1;
        decoder.opparamnum[i] = 0;
        decoder.oplist[i] = L"";
      }
      
      //decoder.op2color.clear();
      //decoder.color2op.clear();
      
      std::vector<std::wstring> idxparams;
      
      std::vector<std::wstring> idxparams2;
      
      while (std::getline(infile, line) && state < 7) {
        if (!line.empty() && line[0] == L'\ufeff')
          line.erase(0, 1);
        if (!line.empty() && line[0] != L'#') {
          switch (state) {
            case 0:
              decoder.isMSB = (toUpper(line) != L"LSB");
              state = 1;
              break;
              
            case 1: 
              symbols_path = line;
              state = 2;
              break;
            
            case 2:
              chars_path = line;
              state = 3;
              break;
              
            case 3: // opcode to entity
              if (line != L"$") {
                size_t d1 = line.find_first_of(L' ');
                size_t d2 = line.find_first_not_of(L' ', d1);
                
                size_t opcode = toNumber(line.substr(0, d1));
                std::wstring opname = trim(line.substr(d2));
                if (opcode < 256) {
                  decoder.oplist[opcode] = opname;
                  decoder.opmap.insert(std::pair<std::wstring, size_t>(opname, opcode));
                  if (opname == L"SYMBOL") {
                    decoder.opsingle[opcode] = 2;
                  }
                }
              } else
                state = 4;
              break;
            
            case 4: // colors
              if (line != L"$") {
                size_t d1 = line.find_first_of(L' ');
                size_t d2 = line.find_first_not_of(L' ', d1);
                size_t colornum = toNumber(line.substr(0, d1));
                std::wstring colorname = trim(line.substr(d2));
                if (colornum < 256) {
                  decoder.op2color.insert(std::pair<size_t, std::wstring>(colornum, colorname));
                  decoder.color2op.insert(std::pair<std::wstring, size_t>(colorname, colornum));
                }
              } else
                state = 5;
              break;
            
            case 5: // index parameters
              if (line != L"$") {
                idxparams2.clear();
                size_t ofs = 0;
                while (ofs < line.size() && idxparams2.size() < 4) {
                  ofs = line.find_first_not_of(L' ', ofs);
                  if (ofs != std::string::npos) {
                    size_t eofs;
                    bool isstr = (line[ofs] == L'"');
                    if (isstr) {
                      eofs = line.find_first_of(L'"', ++ofs);
                    } else {
                      eofs = line.find_first_of(L' ', ofs);
                    }
                    if (eofs == std::string::npos) {
                      idxparams2.push_back(line.substr(ofs));
                      ofs = line.size();
                    } else {
                      idxparams2.push_back(line.substr(ofs, eofs - ofs));
                      ofs = eofs + isstr;
                    }
                  }
                }
                if (idxparams2.size() >= 3) {
                  std::wstring paramname = idxparams2[0];
                  std::wstring filepath = idxparams2[1];
                  std::wstring basetype = idxparams2[2];
                  std::wstring idxdpath = idxparams2.size() > 3 ? idxparams2[3] : decoder_path;
                  
                  idxparams.push_back(paramname);
                  idxparams.push_back(filepath);
                  idxparams.push_back(idxdpath);
                  
                  decoder.parammap.insert(std::pair<std::wstring, size_t>(paramname, decoder.parammap.size()));
                  size_t basetypeidx = PARAM_BYTE;
                  std::unordered_map<std::wstring, size_t>::iterator it = decoder.parammap.find(basetype);
                  if (it != decoder.parammap.end()) // error, unrecognized base type
                    basetypeidx = it->second;
                  decoder.paramsize.push_back(decoder.paramsize[basetypeidx]);
                }
              } else
                state = 6;
              break;
              
            case 6: // insn parameter list
              if (line != L"$") {
                size_t insnsize = 1;
                size_t sofs = line.find_first_not_of(L' '),
                       eofs = line.find_first_of(L' ', sofs + 1);
                std::wstring opname = line.substr(sofs, eofs - sofs);
                size_t opnum = 0;
                std::unordered_map<std::wstring, size_t>::iterator it2 = decoder.opmap.find(opname);
                if (it2 != decoder.opmap.end()) {
                  opnum = it2->second;
                  decoder.opparamofs[opnum] = decoder.opparamtypes.size();
                  size_t paramcount = 0;
                  while ((sofs = line.find_first_not_of(L' ', eofs + 1)) != std::string::npos) {
                    eofs = line.find_first_of(L' ', sofs + 1);
                    if (eofs == std::string::npos)
                      eofs = line.size();
                    std::wstring paramname = line.substr(sofs, eofs - sofs);
                    std::unordered_map<std::wstring, size_t>::iterator it = decoder.parammap.find(paramname);
                    if (it != decoder.parammap.end()) { // ERROR: param name not found
                      size_t paramsize = decoder.paramsize[it->second];
                      decoder.opparamtypes.push_back(it->second);
                      insnsize += paramsize;
                      ++paramcount;
                    }
                  }
                  decoder.opparamnum[opnum] = paramcount;
                  decoder.opsize[opnum] = insnsize;
                } else { // ERROR: op name not found
                  
                }
              } else
                state = 7;
              break;
          }
        }
      }
      
      for (size_t i = 0; i < 256; ++i) {
        std::wstring opname = decoder.oplist[i];
        if (!opname.empty()) {
          std::unordered_map<std::wstring, size_t>::iterator it2 = decoder.opmap.find(opname);
          if (it2 != decoder.opmap.end()) {
            size_t srcop = it2->second;
            if (srcop != i) {
              decoder.opparamnum[i] = decoder.opparamnum[srcop];
              decoder.opsize[i] = decoder.opsize[srcop];
              decoder.opparamofs[i] = decoder.opparamofs[srcop];
            }
          }
        }
      }
      std::wifstream wif;
      wif.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>()));
      if (symbols_path != L"$") {
        wif.open(narrow(symbols_path));
        if (wif) {
          MES::loadSymbols(wif, decoder);
          wif.close();
        }
      }
      if (chars_path != L"$") {
        wif.open(narrow(chars_path));
        if (wif) {
          MES::loadSingleByte(wif, decoder);
          wif.close();
        }
      }
      
      for (size_t j = 0; j < idxparams.size(); j += 3) {
        std::wstring& filepath = idxparams[j + 1];
        std::wstring& idxdpath = idxparams[j + 2];
        
        size_t oext = filepath.find_last_of(L'.');
        std::wstring ext;
        
        if (oext != std::string::npos)
          ext = toUpper(filepath.substr(oext + 1));
        
        decoder.idxmap.emplace_back();
        decoder.idxlist.emplace_back();
        bool loadfailed = false;
        std::string npath = narrow(filepath);
        if (ext == L"TXT") {
          MES::loadList(npath, decoder.idxlist.back(), decoder.idxmap.back(), loadfailed);
        } else if (ext == L"MES") {
          MES::loadMES(idxdpath, npath, decoder.idxlist.back(), decoder.idxmap.back(), loadfailed);
        } else {
          // ERROR: unrecognized file extension
          loadfailed = true;
        }
        decoder.idxloadfailed.push_back(loadfailed);
        decoder.idxenablenames.push_back(!loadfailed);
      }
    }
  }
  return decoder_ptr;
}