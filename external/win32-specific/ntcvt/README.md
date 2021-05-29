# ntcvt
The windows nt encoding converter helpers

## Why create this toy repo
* The natvie Win32 API is fast than c++11 std::codecvt
* More easy to convert between utf-8, utf-16, mcbs at ntoskrnl system
* Do a hack to avoid loop fill '\0' when prealloc buffer for std::wstring
