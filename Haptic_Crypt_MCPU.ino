#include <WiFi.h>
#include <map>
#include <chrono>
#include <iostream>
#include <vector>
#include <string>
#include <bitset>
#include <sstream>
#include <cstddef>
#include "stdlib.h"
#include "mbedtls/md.h"
#include "SPIFFS.h"

using std::vector;
using std::string;
using std::bitset;
using std::to_string;
using std::ostringstream;
using std::stoi;

File file;

std::map<string, string> tableO = {
  {"a", "00000"},
  {"b", "00001"},
  {"c", "00010"},
  {"d", "00011"},
  {"e", "00100"},
  {"f", "00101"},
  {"g", "00110"},
  {"h", "00111"},
  {"i", "01000"},
  {"j", "01001"},
  {"k", "01010"},
  {"l", "01011"},
  {"m", "01100"},
  {"n", "01101"},
  {"o", "01110"},
  {"p", "01111"},
  {"r", "10000"},
  {"s", "10001"},
  {"t", "10010"},
  {"u", "10011"},
  {"v", "10100"},
  {"z", "10101"},
  {"1", "10110"},
  {"2", "10111"},
  {"3", "11000"},
  {"4", "11001"},
  {"5", "11010"},
  {"6", "11011"},
  {"7", "11100"},
  {"8", "11101"},
  {"9", "11111"}
};

vector<const char*> friends = {"usr0"};
vector<const char*> ips = {"localhost"};
vector<int> ports = {30000};
std::map<const char*, const char*> wifis = {{"XXX", "XXXX"}};

string hashHex(vector<uint8_t> data) {
  string res = "";
  
  for (int i = 0; i < data.size(); i++) {
    char buf[5];
    snprintf(buf, sizeof(buf), "%02X", data[i]);
    res += buf;
  }
  return res;
}

vector<uint8_t> generateSHA256(const string& text) {
  std::vector<uint8_t> hash(32);

  mbedtls_md_context_t ctx;
  mbedtls_md_type_t md_type = MBEDTLS_MD_SHA256;

  mbedtls_md_init(&ctx);
  mbedtls_md_setup(&ctx, mbedtls_md_info_from_type(md_type), 0);
  mbedtls_md_starts(&ctx);
  mbedtls_md_update(&ctx, (const unsigned char*)text.c_str(), text.length());
  mbedtls_md_finish(&ctx, hash.data());
  mbedtls_md_free(&ctx);

  return hash;
}

string stringSlice(string s, int off, int keep) {
  return string(s.begin() + off, s.begin() + std::min(keep, static_cast<int>(s.size())));
}

vector<string> divideBinary(int deviNum, string listed) {
  vector<string> lis = {};
  int length = listed.size();
  
  lis.reserve((int)listed.size()/deviNum + 1);
  
  for (int i = 0; i < length; i += deviNum) {
      string chunk = stringSlice(listed, i, i + deviNum);
      if (chunk.size() < deviNum) {
          chunk = string(deviNum - chunk.size(), '0') + chunk;
      }
  
      lis.push_back(chunk);
  }
  
  return lis;
}

string strToBinaryBase(string listed, bool base = true) {
  string res = "";
  if (base) {
      for (int i = 0; i < listed.size(); i++) {
          res += bitset<8>(listed.c_str()[i]).to_string();
      }
  }
  else {
      for (int i = 0; i < listed.size(); i += 8) {
          std::string byte = listed.substr(i, 8);
          
          char character = static_cast<char>(bitset<8>(byte).to_ulong());
          
          res += character;
      }
  }
  return res;
}

vector<string> findInList(vector<string> lst, std::map<string, string> table, bool base = true) {
  vector<string> lis = {};
  lis.reserve(lst.size());
  
  if (base) {
      std::map<string, string> reversedMap = {};
      for (const auto pair : table) {
          reversedMap[pair.second] = pair.first;
      }
  
      for (string s : lst) {
          if (reversedMap.find(s) != reversedMap.end()) {
              lis.push_back(reversedMap[s]);
          }
      }
  }
  else {
      for (string s : lst) {
          if (table.find(s) != table.end()) {
              lis.push_back(table[s]);
          }
      }
  }
  return lis;
}

vector<string> findInListFlash(vector<string> lst) {
  vector<string> lis = {};
  lis.reserve(lst.size());
  
  for (string s : lst) {
    file.seek(0, SeekSet);
    string line;
    while (file.available()) {
      char c = file.read();
      if (c == '\n') {
        string val(line.begin()+line.rfind(':')+2, line.end()-2);
        string key(line.begin()+1, line.begin()+line.rfind(':')-1);
        if (val == s) {
          //Serial.println("FOUND!!!!!!!!!!!!!!!!!!!!!!!!!!");
          //Serial.printf("s-%s\n", s.c_str());
          //Serial.printf("v-%s\n", val.c_str());
          //Serial.println();
          //string key(line.begin()+1, line.end()-line.rfind(':')-2);
          //Serial.println(key.c_str());
          lis.push_back(key);
          break;
        }
        line.clear();
      }
      else {
        line += c;
      }
    }
  }
  return lis;
}

string joinVector(vector<string> vec) {
  ostringstream oss;

  for (string s : vec) {
      oss << s;
  }

  return oss.str();
} 

vector<string> encode(string plain) {
  vector<string> str50T = {};
  str50T.reserve(plain.size());
  
  for (char ch : plain) {
      str50T.push_back('%' + to_string(static_cast<int>(ch)));
  }
  
  vector<string> bin50T = {};
  bin50T.reserve(str50T.size());
  
  for (string s : str50T) {
      bin50T.push_back(strToBinaryBase(s));
  }
  
  string binStr50 = joinVector(bin50T);
  
  vector<string> inputFSx = divideBinary(5, binStr50);
  
  vector<string> inputFSp = findInList(inputFSx, tableO);
  
  string sr50 = joinVector(inputFSp);
  
  string inputFOc = strToBinaryBase(sr50);
  
  vector<string> inputFNn = divideBinary(10, inputFOc);

  vector<string> inputFDc = findInListFlash(inputFNn);
  string encStr = sr50.size() >= 10 ? string(sr50.end() - 10, sr50.end()) : string(10 - sr50.size(), ' ') + sr50;
  string encHt = string(str50T.back().begin() + 1, str50T.back().end());
  
  inputFDc.push_back(encHt);
  inputFDc.push_back(encStr);

  return inputFDc;
}

WiFiClient client;

uint8_t fingers[4] = {0, 0, 0, 0};
string id = "";
string charBuffer = "";
string clientBuffer = "";
string receiver = friends[0];
bool isHeld[4] = {false, false, false, false};

uint16_t pointerTimer = 3000;
auto pointerStart = std::chrono::high_resolution_clock::now();
auto pointerStop = std::chrono::high_resolution_clock::now();
auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(pointerStop - pointerStart);
bool pointerTimerStart = false;


void fingerCheck();
void combine(uint8_t comb[4]);
uint8_t numberCheck();
void clearInputs();

std::map<const char, const char*> dict = {
  {'a', "001"}, {'b', "010"}, {'c', "100"}, {'d', "101"}, {'e', "110"},
  {'f', "111"}, {'g', "011"}, {'h', "002"}, {'i', "020"}, {'j', "200"},
  {'k', "201"}, {'l', "210"}, {'m', "211"}, {'n', "112"}, {'o', "121"},
  {'p', "022"}, {'q', "220"}, {'r', "202"}, {'s', "003"}, {'t', "030"},
  {'u', "300"}, {'v', "301"}, {'w', "310"}, {'x', "004"}, {'y', "040"},
  {'z', "400"}
};

std::map<const char*, int> nums = {
  {"z", 0}, {"o", 1}, {"th", 2}, {"f", 4}, {"fi", 5},
  {"s", 6}, {"se", 7}, {"e", 8}, {"n", 9}
};

void setup() {
  Serial.begin(115200);
  delay(1000);

  id = hashHex(generateSHA256(string(String((uint32_t)ESP.getEfuseMac(), HEX).c_str())));

  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  file = SPIFFS.open("/map.txt");
  if(!file){
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Connecting to WiFi");

  for (const auto [ssid, passwd] : wifis) {
    WiFi.disconnect(true);
    delay(2000);
    WiFi.begin(ssid, passwd);

    auto startTime = std::chrono::high_resolution_clock::now();
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
      auto endTime = std::chrono::high_resolution_clock::now();
      if (std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime).count() > 5000) {
        break;  
      }
    }
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected");  
  }
  Serial.println("Connected to Wi-Fi!");


  if (client.connect(ips[0], ports[0])) {
    Serial.println("Connected to TCP server!");
  }
  else {
    Serial.println("Connection to server failed.");
  }

  Serial.println(id.c_str());
}

void loop() {
  if (client.connected()) {
    fingerCheck();

    pointerStop = pointerTimerStart ? std::chrono::high_resolution_clock::now() : pointerStart;
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(pointerStop - pointerStart);
    if (duration.count() >= pointerTimer) {
      switch (fingers[3]) {
        case 1:
          combine(fingers);
          break;
        case 2:
          charBuffer += " ";
          break;
        case 3:
          for (const string &s : encode(charBuffer))
          {
            clientBuffer += s + ", ";
          }
          Serial.printf("%s\n", charBuffer.c_str());
          Serial.printf("%s\n", clientBuffer.c_str());
          client.printf("%s", (clientBuffer+'|'+receiver+'|'+id).c_str());
          charBuffer = "";
          clientBuffer = "";
          break;
        case 4:
          receiver = friends[numberCheck()];
          break;
        case 5:
          client.stop();
          delay(1000);

          Serial.println("Connecting to server...");

          if (client.connect(ips[numberCheck()], ports[numberCheck()])) {
            Serial.println("Connected to TCP server!");
          }
          else {
            Serial.println("Connection to server failed.");
          }
          break;
        default:
          Serial.println("Not valid choice");
          break;
      }
      clearInputs();
      pointerTimerStart = false;
    }
  }
  else {
    fingerCheck();
    uint8_t serverData;

    pointerStop = pointerTimerStart ? std::chrono::high_resolution_clock::now() : pointerStart;
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(pointerStop - pointerStart);
    if (duration.count() >= pointerTimer) {
      switch(fingers[3]) {
        case 4:
          receiver = friends[numberCheck()];
          break;
        case 5:
          client.stop();
          delay(1000);

          Serial.println("Connecting to server...NON");
          
          serverData = numberCheck();
          Serial.println(serverData);
          if (client.connect(ips[serverData], ports[serverData])) {
            Serial.println("Connected to TCP server!");
          }
          else {
            Serial.println("Connection to server failed.");
          }
          break;
        default:
          Serial.println("Not valid choice");
          break;
      }
      charBuffer = "";
      clearInputs();
      pointerTimerStart = false;
    }
  }
}

void combine(uint8_t comb[4]) {
  std::string temp = "";

  for (uint8_t i = 0; i < 3; i++) {
      temp += (char)(comb[i] + '0');
  }
  for (auto const [key, val] : dict) {
    if (temp == val) {
      charBuffer += key;
      break;
    }
  }
}

void fingerCheck() {
  uint8_t touch[4] = {T9, T7, T4, T0};

  for (uint8_t i = 0; i < 4; i++) {
    if (touchRead(touch[i]) < 50 && !isHeld[i]) {
      if (i == 3) {
        if (fingers[3] == 0) {pointerStart = std::chrono::high_resolution_clock::now();}
        pointerTimerStart = true;
        Serial.println("Timer started");
      }
      fingers[i] += 1;
      isHeld[i] = true;
    }
    else if (touchRead(touch[i]) >= 50) {
      isHeld[i] = false;
    }
  }
}

uint8_t numberCheck() {
  combine(fingers);
  for (auto const [key, val] : nums) {
    if (key == charBuffer) {
      return val;
    }
  }
  return -1;
}

void clearInputs() {
  for (uint8_t& i : fingers) {
    i = 0;
  }
}
