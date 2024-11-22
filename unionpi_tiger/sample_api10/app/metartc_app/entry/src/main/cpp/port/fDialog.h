//
// Created on 2023/11/30.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MyApplication_fDialog_H
#define MyApplication_fDialog_H

class fDialog{
public:
    fDialog(void *parent = nullptr);
    void hide();
    void show();
};
#endif //MyApplication_fDialog_H
