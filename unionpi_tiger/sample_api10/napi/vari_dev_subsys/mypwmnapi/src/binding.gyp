
{
    "targets": [
        {
          "target_name": "mypwm",
          "sources": [
              "./mypwm.cpp",
              "./mypwm_middle.cpp",
              "./tool_utility.cpp"],
          "include_dirs": ["."],
          "cflags_cc": [ "-frtti","-std=c++17" ]
        }
    ]
}
