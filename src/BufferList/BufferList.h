#pragma once
#include <string.h>
#include <stdlib.h>
#include <iostream>
class BufferList
{
public:    

    BufferList();

    ~BufferList();

    bool Init(unsigned int bufSize);

    int pop_front(unsigned char *outData, unsigned int sizeOfData = 0);

    int push_back(unsigned char *Data, unsigned int LenOfData);

    int size();

    bool empty();

    
private:

    unsigned char *m_buffer;

    unsigned int m_bufferSize;

    unsigned int m_beginIndex;

    unsigned int m_endIndex;

    unsigned int m_size;

    unsigned int m_lenSize;

    unsigned int m_leftSpace;
    

};
