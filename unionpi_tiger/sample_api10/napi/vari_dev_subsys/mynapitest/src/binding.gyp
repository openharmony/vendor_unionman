
{
    "targets": [
        {
          "target_name": "my_hello_napi",
          "sources": [
              "./my_hello_napi.cpp",
              "./my_hello_napi_middle.cpp",
              "./tool_utility.cpp"],
          "include_dirs": ["."],
          "cflags_cc": [ "-frtti","-std=c++17" ]
        }
    ]
}
