//Edit By TigerCN
library xebasic
//**************************************************************************
//
// xebasic 0.4
// =======
// XE_DUMMY_UNITID : Rawcode of the dummy unit in your map. It should
//                   use the dummy.mdx model, so remember to import it as
//                   well, just use copy&paste to copy the dummy from the
//                   xe map to yours, then change the rawcode.
//
// XE_HEIGHT_ENABLER: Medivh's raven form ability, you may need to change
//                    this rawcode to another spell that morphs into a flier
//                    in case you modified medivh's spell in your map.
//
// XE_TREE_RECOGNITION: The ancients' Eat tree ability, same as with medivh
//                      raven form, you might have to change it.
//
// XE_ANIMATION_PERIOD: The global period of animation used by whatever
//                      timer that depends on it, if you put a low value
//                      the movement will look good but it may hurt your
//                      performance, if instead you use a high value it
//                      will not lag but will be fast.
//
// XE_MAX_COLLISION_SIZE: The maximum unit collision size in your map, if
//                        you got a unit bigger than 197.0 it would be
//                        a good idea to update this constant, since some
//                        enums will not find it. Likewise, if none of
//                        your units can go bellow X and X is much smaller
//                        than 197.0, it would be a good idea to update
//                        as well, since it will improve the performance
//                        those enums.
//
// Notice you probably don't have to update this library, unless I specify
// there are new constants which would be unlikely. 
//
//**************************************************************************

//===========================================================================
    globals
        constant integer XE_DUMMY_UNITID       = 'e000'
        constant integer XE_HEIGHT_ENABLER     = 'Amrf'
        constant integer XE_TREE_RECOGNITION   = 'Aeat'
        constant real    XE_ANIMATION_PERIOD   =  0.025
        constant real    XE_MAX_COLLISION_SIZE =  197.0
    endglobals

endlibrary