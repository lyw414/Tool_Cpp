#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <unistd.h>

#define BLOCKSIZE 1024
#defile FILEHANDLE int

namespace LYW_CODE
{


    typedef enum _cmpRes
    {
        EQUAL_BST;
        LESS_BST;
        GREATER_BST;
    } TcmpRes;
    typedef struct _File_BST_Node
    {
        union
        {
            struct 
            {
                unsigned int OffSet;
                unsigned int Index;
                
            } HeadInfo;
        } Head;

        union 
        {
            unsigned char Buffer[ BLOCKSIZE - sizeof ( Head ) ];
        } Context;
    } TFile_BST_Node, * PFile_BST_Node;

    class File_
    {
    public:
        int m_Handle;
        File_()
        {
            m_Handle = -1;
        };
        ~File_()
        {
            ::close ( m_Handle );
        };
        int open ( const std::string & FileName )
        {
            m_Handle = ::open ( FileName.c_str(),O_CREAT | O_RDWR , S_IRUSR | S_IWUSR );
            return m_Handle;
        }

        int seek ( int SeekIndex )
        {
            return ::lseek ( m_Handle, SeekIndex,SEEK_SET );
        }

        int read ( std::string & OutText, unsigned int ReadSize )
        {
            OutText.clear();
            OutText.resize ( ReadSize );
            return ::read ( m_Handle, (unsigned char *)OutText.c_str(), ReadSize );
        }
        int write ( const unsigned char * InBuffer, unsigned int iLenOfBuffer )
        {
            return ::write ( m_Handle, InBuffer, iLenOfBuffer );
        }

    };

    class File_BST
    {
    private:
        typedef TcmpRes ( * TCmpFunc )( void * , void * );
        FILEHANDLE  m_Handle;
        std::string m_FileName;
        TCmpFunc  m_cmpFunc;
        unsigned int m_OffSet;

        
    public:
        File_BST ( const  std::string & FileName, TCmpRes CmpFunc)
        {
            m_FileName = FileName;
            m_cmpFunc = CmpFunc;
        }
        
        bool Add (  
        
    };
}
