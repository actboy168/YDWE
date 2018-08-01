#define YDWE_PRELOADSL_HEAD_MASK                         'YDWE'    
#define YDWE_PRELOADSL_CODE_INDEX_BEG(p)         (501*GetPlayerId(p))
#define YDWE_PRELOADSL_GET_RND_MASK()                GetRandomInt('0000', 'OOOO')    
#define YDWE_PRELOADSL_LIST_FILE(dir)                "save\\"+dir+"\\list.sav" 
#define YDWE_PRELOADSL_DATA_FILE(dir, file)    "save\\"+dir+"\\pre"+file+".sav"    

#define YDWE_PRELOADSL_GET_KEY(p)                        StringHash(GetPlayerName(p))

     
#include "YDWEStringHash.j"
#include "Util/YDWESync.j"

library YDWEPreloadSL initializer Init requires YDWEStringHash, YDWESync

globals
    ///
    /// 每个玩家占用的空间[500*id, 500*(id-1))
    /// 下标越界没有进行检查，似乎没这个必要，谁要是用超过500...就去死吧- -
    /// 
    private integer array Code
    private integer array CodeHI
    private integer array CodeLO
    boolean bj_lastLoadPreloadSLResult
endglobals        
 
#include "PreloadSL/YDWE_PreloadSL_Pre.j" 
#include "PreloadSL/YDWE_PreloadSL_Manager.j" 
#include "PreloadSL/YDWE_PreloadSL_Code.j" 

#if 0 
    ///    
    ///    一个简单的多存档管理接口 
    ///        打开dir目录下的list.sav文件，获取其中的字符串。    
    ///        可以用于记录这个目录下的存档文件名，当然要用作其他用途也是可以的。 
    /// 
    function YDWE_PreloadSL_SetFile takes player p, string dir, string value, integer n returns nothing
    function YDWE_PreloadSL_GetFile takes player p, string dir, integer n returns string
    ///    
    ///    读取本地文件到code数组内
    ///    同一时间内不会对一个以上的玩家读取数据，Load函数内有lock来保证这点。所以Load不会马上返回。 
    ///    返回false，表示读取失败。失败时，code数组内的值将变得不可信。失败的原因可能是    
    ///        1.错误的目录名、文件名                                        
    ///        2. n的值和保存时不一致                                    
    ///        3.玩家名和保存时不一致                                     
    ///        4.存档被修改过了                                    
    ///
    function YDWE_PreloadSL_Load takes player p, string dir, string file, integer max_n returns boolean
    ///    
    ///    保存code数组到本地文件    
    ///
    function YDWE_PreloadSL_Save takes player p, string dir, string file, integer max_n returns nothing 
    ///    
    ///    读写code数组里的值 
    /// 
    function YDWE_PreloadSL_Set takes player p, string s, integer n, integer value returns nothing
    function YDWE_PreloadSL_Get takes player p, string s, integer n returns integer     
#endif
                                
private function Init takes nothing returns nothing
    local integer index = 0 
    loop
        exitwhen    index >= 16
        set udg_YDWE_PreloadSL_List[index] = "<null>"
        set index = index + 1
    endloop
endfunction 

endlibrary 
                 
#undef YDWE_PRELOADSL_HEAD_MASK
#undef YDWE_PRELOADSL_GET_RND_MASK
#undef YDWE_PRELOADSL_CODE_INDEX_BEG                    
#undef YDWE_PRELOADSL_LIST_FILE                        
#undef YDWE_PRELOADSL_LIST_FILE 

