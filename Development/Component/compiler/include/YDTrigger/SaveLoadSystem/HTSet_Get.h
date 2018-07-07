# /*
#  *  YDTrigger专用存储系统
#  *  
#  *  set 和 get 的实现
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_SET_AND_GET_H
#define INCLUDE_YDTRIGGER_SET_AND_GET_H
#
# /*
#  *  
#  *  HashTable版 Clear
#  *
#  */
#
#  define YDHashClearAll(handle)                             FlushParentHashtable(handle)
#  define YDHashClearTable(handle, table)                    FlushChildHashtable(handle, table)
#  define YDHashClearTableByString(handle, table)            FlushChildHashtable(handle, StringHash(table))
#
# /*
#  *  
#  *  HashTable版 Set & Get
#  *
#  */
#                                               
#if defined(YDHASH_SUPPORT_SETNULL)                                       
#  define YDHashSet(handle, type, table, key, value)         YDTRIGGER_SET_##type##(handle, table, key, value)
#else                                   
#  define YDHashSet(handle, type, table, key, value)         YDTRIGGER_##type##(SET)(handle, table, key, value)
#endif   
#  define YDHashGet(handle, type, table, key)                YDTRIGGER_##type##(GET)(handle, table, key)
#
#  define YDHashSetEx(handle, type, table, key, value)       YDHashSet(handle, type, table, key, value)
#  define YDHashGetEx(handle, type, table, key)              YDHashGet(handle, type, table, key) 
# 
#  define YDHashSetByString(handle, type, table, key, value) YDTRIGGER_##type##(SET)(handle, StringHash(table), StringHash(key), value)
#  define YDHashGetByString(handle, type, table, key)        YDTRIGGER_##type##(GET)(handle, StringHash(table), StringHash(key))
# 
#  define YDTRIGGER_HTSET(Type)                         Save##Type
#  define YDTRIGGER_HTGET(Type)                         Load##Type
#                                                      
#  define YDTRIGGER_integer(SG)                         YDTRIGGER_HT##SG##(Integer)
#  define YDTRIGGER_real(SG)                            YDTRIGGER_HT##SG##(Real)
#  define YDTRIGGER_boolean(SG)                         YDTRIGGER_HT##SG##(Boolean)
#  define YDTRIGGER_string(SG)                          YDTRIGGER_HT##SG##(Str)
#  define YDTRIGGER_StringExt(SG)                       YDTRIGGER_HT##SG##(Str)
#  define YDTRIGGER_timer(SG)                           YDTRIGGER_HT##SG##(TimerHandle)
#  define YDTRIGGER_trigger(SG)                         YDTRIGGER_HT##SG##(TriggerHandle)
#  define YDTRIGGER_unit(SG)                            YDTRIGGER_HT##SG##(UnitHandle)
#  define YDTRIGGER_unitcode(SG)                        YDTRIGGER_HT##SG##(Integer)
#  define YDTRIGGER_abilcode(SG)                        YDTRIGGER_HT##SG##(Integer)
#  define YDTRIGGER_item(SG)                            YDTRIGGER_HT##SG##(ItemHandle)
#  define YDTRIGGER_itemcode(SG)                        YDTRIGGER_HT##SG##(Integer)
#  define YDTRIGGER_group(SG)                           YDTRIGGER_HT##SG##(GroupHandle)
#  define YDTRIGGER_player(SG)                          YDTRIGGER_HT##SG##(PlayerHandle)
#  define YDTRIGGER_location(SG)                        YDTRIGGER_HT##SG##(LocationHandle)
#  define YDTRIGGER_destructable(SG)                    YDTRIGGER_HT##SG##(DestructableHandle)
#  define YDTRIGGER_force(SG)                           YDTRIGGER_HT##SG##(ForceHandle)
#  define YDTRIGGER_rect(SG)                            YDTRIGGER_HT##SG##(RectHandle)
#  define YDTRIGGER_region(SG)                          YDTRIGGER_HT##SG##(RegionHandle)
#  define YDTRIGGER_sound(SG)                           YDTRIGGER_HT##SG##(SoundHandle)
#  define YDTRIGGER_effect(SG)                          YDTRIGGER_HT##SG##(EffectHandle)
#  define YDTRIGGER_unitpool(SG)                        YDTRIGGER_HT##SG##(UnitPoolHandle)
#  define YDTRIGGER_itempool(SG)                        YDTRIGGER_HT##SG##(ItemPoolHandle)
#  define YDTRIGGER_quest(SG)                           YDTRIGGER_HT##SG##(QuestHandle)
#  define YDTRIGGER_questitem(SG)                       YDTRIGGER_HT##SG##(QuestItemHandle)
#  define YDTRIGGER_timerdialog(SG)                     YDTRIGGER_HT##SG##(TimerDialogHandle)
#  define YDTRIGGER_leaderboard(SG)                     YDTRIGGER_HT##SG##(LeaderboardHandle)
#  define YDTRIGGER_multiboard(SG)                      YDTRIGGER_HT##SG##(MultiboardHandle)
#  define YDTRIGGER_multiboarditem(SG)                  YDTRIGGER_HT##SG##(MultiboardItemHandle)
#  define YDTRIGGER_trackable(SG)                       YDTRIGGER_HT##SG##(TrackableHandle)
#  define YDTRIGGER_dialog(SG)                          YDTRIGGER_HT##SG##(DialogHandle)
#  define YDTRIGGER_button(SG)                          YDTRIGGER_HT##SG##(ButtonHandle)
#  define YDTRIGGER_texttag(SG)                         YDTRIGGER_HT##SG##(TextTagHandle)
#  define YDTRIGGER_lightning(SG)                       YDTRIGGER_HT##SG##(LightningHandle)
#  define YDTRIGGER_image(SG)                           YDTRIGGER_HT##SG##(ImageHandle)
#  define YDTRIGGER_fogstate(SG)                        YDTRIGGER_HT##SG##(FogStateHandle)
#  define YDTRIGGER_fogmodifier(SG)                     YDTRIGGER_HT##SG##(FogModifierHandle)
#  define YDTRIGGER_radian(SG)                          YDTRIGGER_HT##SG##(Real)
#  define YDTRIGGER_degree(SG)                          YDTRIGGER_HT##SG##(Real)
#  define YDTRIGGER_imagefile(SG)                       YDTRIGGER_HT##SG##(Str)
#  define YDTRIGGER_modelfile(SG)                       YDTRIGGER_HT##SG##(Str)
#                                                  
#endif
