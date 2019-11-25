#include <vector>
#include "JsonValue.hpp"
namespace LYW_CODE
{
    class JsonNodePool
    {
    private:
        std::vector < void * > m_resource_header;
        std::vector < PJsonValue > m_Simple_Value_Free;
        std::vector < PJsonValue > m_String_Value_Free;
        std::vector < PJsonValue > m_Json_Value_Free;
        std::vector < PJsonValue > m_Array_Value_Free;

    public:
        PJsonValue GetOne ();
        void GiveBack ( PJsonValue );
        void GiveBackAll ();
    };
}
