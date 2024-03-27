
{
    "targets": [
        {
          "target_name": "adcnapi",
          "sources": [
              "./adcnapi.cpp",
              "./adcnapi_middle.cpp",
              "./tool_utility.cpp"],
          "include_dirs": ["."],
          "cflags_cc": [ "-frtti","-std=c++17" ]
        }
    ]
}
