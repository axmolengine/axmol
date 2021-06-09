# fastlib
* **fast_csv.hpp**: Only one API ```fast_csv_parse_line``` to help parse csv easy with FSM, no any memory alloc duraing parsing
  ```cpp
  std::string buffer = read_file("xxx.csv");
  const char* cursor = buffer.c_str();
  while(*cursor) {
     cursor = fastl::fast_csv_parse_line(cursor, [&](const char* v_start, const char* v_end) {
         // do somethings, store or parse the column value
         // std::string value(v_start, v_end);
     });
  ```
  or
  ```cpp
  std::string buffer = read_file("xxx.csv");
  if(!buffer.empty()) {
    char* cursor = &buffer.front();
    while(*cursor) {
       cursor = fastl::fast_csv_parse_line(cursor, [&](char* v_start, char* v_end) {
           // if this column is int or double, is more Memory Efficient parse it directly
           char endch = *v_end; // store the last char in the streaming
           *v_end = '\0';
           int key = atoi(v_start); // parse int value directly
           *v_end = endch; // restore rthe last char in the stream
       });
   }
  ```
* **fast_split.hpp**: Yeah, idea same with ```fast_csv.hpp```
  ```cpp
  std::string buffer = read_file("xxx.csv");
  fastl::fast_split_of(buffer.c_str(), buffer.size(), "\r\n", [&](const char* v_start, const char* v_end, int /*delimChar*/) {
    // do somethings, store or parse the column value
    // std::string value(v_start, v_end);
  });
  ```
  or
  ```cpp
  std::string buffer = read_file("xxx.csv");
  if(!buffer.empty()) {
    fastl::fast_split_of(&buffer.front(), buffer.size(), "\r\n", [&](char* v_start, char* v_end, int /*delimChar*/) {
      // if item is int or double, is more Memory Efficient parse it directly
      char endch = *v_end; // store the last char in the streaming
      *v_end = '\0';
      int key = atoi(v_start); // parse int value directly
      *v_end = endch; // restore rthe last char in the stream
    });
  ```
