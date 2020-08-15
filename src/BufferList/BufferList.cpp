#include "BufferList.h"

BufferList::BufferList()
{
    m_lenSize = sizeof(unsigned int);
    m_bufferSize = 0;
    m_beginIndex = 0;
    m_endIndex = 0;
    m_size = 0;
    m_buffer = NULL;
}

BufferList::~BufferList()
{
    m_bufferSize = 0;
    m_beginIndex = 0;
    m_endIndex = 0;
    m_size = 0;

    if (m_buffer != NULL)
    {
        ::free(m_buffer);
        m_buffer = NULL;
    }
}

bool BufferList::Init(unsigned int bufSize)
{
    if (bufSize == 0)
    {
        return false;
    }
    m_leftSpace = bufSize;
    m_bufferSize = bufSize + 2;
    if (m_buffer != NULL)
    {
        ::free(m_buffer);
        m_buffer = NULL;
    }
    m_buffer = (unsigned char *)::malloc(m_bufferSize);
    m_beginIndex = 0;
    m_endIndex = 1;
    m_size  = 0;
}

int BufferList::pop_front(unsigned char *outData, unsigned int sizeOfData)
{
    unsigned int len;
    unsigned int next = 0;
    unsigned int leftLen= 0;
    unsigned int totalLen = 0;
    
    /*empty check*/
    if (m_size == 0)
    {
        return 0;
    }

    /*next end check*/ 
    next = (m_beginIndex + 1) % m_bufferSize;
    if (next == m_endIndex)
    {
        /*empty*/
        m_leftSpace = m_bufferSize - 2;
        m_size = 0;
        m_beginIndex = 0;
        m_endIndex = 1;
        return 0;
    }

    /*used space check*/ 
    if (m_bufferSize - 2 - m_leftSpace < m_lenSize)
    {
        m_leftSpace = m_bufferSize - 2;
        m_size = 0;
        m_beginIndex = 0;
        m_endIndex = 1;
    }
    else
    {
        m_leftSpace += m_lenSize;
    }

    /*read len*/
    leftLen = m_bufferSize - next;
    if (leftLen >= m_lenSize)
    {
        memcpy(&len, m_buffer + next, m_lenSize);

    }
    else
    {
        memcpy(&len, m_buffer + next, leftLen);
        memcpy((unsigned char *)&len + leftLen, m_buffer, m_lenSize - leftLen);
    }
    next = (next + m_lenSize) % m_bufferSize;


    /*data size check*/
    if (sizeOfData != 0 && sizeOfData < len)
    {
        return -1;
    }

    if(len == 0)
    {
        /*data len is 0*/
        m_leftSpace = m_bufferSize - 2;
        m_size = 0;
        m_beginIndex = 0;
        m_endIndex = 1;
        return 0;
    }

    /*is data enough*/
    if (m_bufferSize - 2 - m_leftSpace < len)
    {
        m_leftSpace = m_bufferSize - 2;
        m_size = 0;
        m_beginIndex = 0;
        m_endIndex = 1;
    }
    else
    {
        m_leftSpace += len;
    }

    leftLen = m_bufferSize - next;
    
    if (leftLen >= len)
    {
        memcpy(outData, m_buffer + next, len);
    }
    else
    {
        memcpy(outData, m_buffer + next, leftLen);
        memcpy(outData + leftLen, m_buffer, len - leftLen);
    }

    m_size--;
    m_beginIndex = (m_beginIndex + m_lenSize + len) % m_bufferSize;
    return len;
}

int BufferList::push_back(unsigned char *Data, unsigned int LenOfData)
{
    unsigned int next = 0;
    unsigned int endIndex = 0;
    unsigned int leftLen = 0;
    
    /*Data Len check*/
    if (LenOfData == 0 || LenOfData > m_bufferSize - 2 - m_lenSize)
    { 
        return 0;
    }

    /*left space check*/
    if (m_leftSpace <= LenOfData + m_lenSize)
    {
        return 0;
    }
    else
    {
        m_leftSpace = m_leftSpace - LenOfData - m_lenSize;
    }

    endIndex = (m_endIndex + LenOfData + m_lenSize) % m_bufferSize;

    /*add Len*/
    leftLen = m_bufferSize - m_endIndex;
    if (leftLen >= m_lenSize)
    {
        memcpy(m_buffer + m_endIndex, &LenOfData, m_lenSize);
    }
    else
    {
        memcpy(m_buffer + m_endIndex, (unsigned char *)&LenOfData, leftLen);
        memcpy(m_buffer, (unsigned char *)&LenOfData + leftLen, m_lenSize - leftLen);
    }

    next = (m_endIndex + m_lenSize) %  m_bufferSize;

    /*add data*/
    leftLen = m_bufferSize - next;
    
    if (leftLen >= LenOfData)
    {
        memcpy(m_buffer + next, Data, LenOfData);
    }
    else
    {
        memcpy(m_buffer + next, Data, leftLen);
        memcpy(m_buffer, Data + leftLen, LenOfData - leftLen);
    }

    m_endIndex = endIndex;
    m_size++;
    return LenOfData;
} 

int BufferList::size()
{
    return m_size;
}

bool BufferList::empty()
{
    if (m_size == 0 && ((m_beginIndex + 1) % m_bufferSize) == m_endIndex)
    {
        return true;
    }
    else
    {
        return false;
    }
}

