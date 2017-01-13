# /*
#  *  局部变量、自定义值
#  *  
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_YDTRIGGER_HASH_H
#define INCLUDE_YDTRIGGER_HASH_H
#
#define YDUserDataClearTable(table_type, table) YDHashClearTable(YDHASH_HANDLE, YDHashAny2I(table_type, table))
#define YDUserDataClear(table_type, table, attribute, value_type) YDHashClear(YDHASH_HANDLE, value_type, YDHashAny2I(table_type, table), <?=StringHash(attribute)?>)
#define YDUserDataSet(table_type, table, attribute, value_type, value) YDHashSet(YDHASH_HANDLE, value_type, YDHashAny2I(table_type, table), <?=StringHash(attribute)?>, value)
#define YDUserDataGet(table_type, table, attribute, value_type) YDHashGet(YDHASH_HANDLE, value_type, YDHashAny2I(table_type, table), <?=StringHash(attribute)?>)
#define YDUserDataHas(table_type, table, attribute, value_type) YDHashHas(YDHASH_HANDLE, value_type, YDHashAny2I(table_type, table), <?=StringHash(attribute)?>)
#
#
#  // GlobalsTriggerRunSteps & TriggerRunSteps
#define YDLocalInitialize() \
    local integer ydl_localvar_step = YDHashGet(YDHASH_HANDLE, integer, YDHashH2I(GetTriggeringTrigger()), 0xCFDE6C76) YDNL\
    set ydl_localvar_step = ydl_localvar_step + 3                                                                      YDNL\
    call YDHashSet(YDHASH_HANDLE, integer, YDHashH2I(GetTriggeringTrigger()), 0xCFDE6C76, ydl_localvar_step)           YDNL\
    call YDHashSet(YDHASH_HANDLE, integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7, ydl_localvar_step)
#define YDLocalExecuteTrigger(trg)                 set ydl_triggerstep = YDHashH2I(trg)*(YDHashGet(integer, YDHashH2I(trg), 0xCFDE6C76) + 3)
#define YDLocalRelease()                           YDHashClearTable(YDHASH_HANDLE, YDHashH2I(GetTriggeringTrigger())*ydl_localvar_step)
#define YDLocalReset()                             YDHashSet(YDHASH_HANDLE, integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7, ydl_localvar_step)
# // 1.
#define YDLOCAL_1                                  YDHashH2I(GetTriggeringTrigger())*ydl_localvar_step
#define YDLocal1Set(type, name, value)             YDHashSet(YDHASH_HANDLE, type, YDLOCAL_1, <?=StringHash(name)?>, value)
#define YDLocal1ArraySet(type, name, index, value) YDHashSet(YDHASH_HANDLE, type, YDLOCAL_1, <?=StringHash(name)?> + (index), value)
#define YDLocal1Get(type, name)                    YDHashGet(YDHASH_HANDLE, type, YDLOCAL_1, <?=StringHash(name)?>)
#define YDLocal1ArrayGet(type, name, index)        YDHashGet(YDHASH_HANDLE, type, YDLOCAL_1, <?=StringHash(name)?> + (index))
# // 2.
#define YDLOCAL_2                                  YDHashH2I(GetTriggeringTrigger())*YDHashGet(integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7)
#define YDLocal2Set(type, name, value)             YDHashSet(YDHASH_HANDLE, type, YDLOCAL_2, <?=StringHash(name)?>, value)
#define YDLocal2ArraySet(type, name, index, value) YDHashSet(YDHASH_HANDLE, type, YDLOCAL_2, <?=StringHash(name)?> + (index), value)
#define YDLocal2Get(type, name)                    YDHashGet(YDHASH_HANDLE, type, YDLOCAL_2, <?=StringHash(name)?>)
#define YDLocal2ArrayGet(type, name, index)        YDHashGet(YDHASH_HANDLE, type, YDLOCAL_2, <?=StringHash(name)?> + (index))
# // 3.
#define YDLOCAL_3                                  GetExpiredTimer()
#define YDLocal3Set(type, name, value)             YDHashSet(YDHASH_HANDLE, type, YDLOCAL_3, <?=StringHash(name)?>, value)
#define YDLocal3ArraySet(type, name, index, value) YDHashSet(YDHASH_HANDLE, type, YDLOCAL_3, <?=StringHash(name)?> + (index), value)
#define YDLocal3Get(type, name)                    YDHashGet(YDHASH_HANDLE, type, YDLOCAL_3, <?=StringHash(name)?>)
#define YDLocal3ArrayGet(type, name, index)        YDHashGet(YDHASH_HANDLE, type, YDLOCAL_3, <?=StringHash(name)?> + (index))
# // 4.
#define YDLOCAL_4                                  GetTriggeringTrigger()
#define YDLocal4Set(type, name, value)             YDHashSet(YDHASH_HANDLE, type, YDLOCAL_4, <?=StringHash(name)?>, value)
#define YDLocal4ArraySet(type, name, index, value) YDHashSet(YDHASH_HANDLE, type, YDLOCAL_4, <?=StringHash(name)?> + (index), value)
#define YDLocal4Get(type, name)                    YDHashGet(YDHASH_HANDLE, type, YDLOCAL_4, <?=StringHash(name)?>)
#define YDLocal4ArrayGet(type, name, index)        YDHashGet(YDHASH_HANDLE, type, YDLOCAL_4, <?=StringHash(name)?> + (index))
# // 5.
#define YDLOCAL_5                                  ydl_triggerstep
#define YDLocal5Set(type, name, value)             YDHashSet(YDHASH_HANDLE, type, YDLOCAL_5, <?=StringHash(name)?>, value)
#define YDLocal5ArraySet(type, name, index, value) YDHashSet(YDHASH_HANDLE, type, YDLOCAL_5, <?=StringHash(name)?> + (index), value)
#define YDLocal5Get(type, name)                    YDHashGet(YDHASH_HANDLE, type, YDLOCAL_5, <?=StringHash(name)?>)
#define YDLocal5ArrayGet(type, name, index)        YDHashGet(YDHASH_HANDLE, type, YDLOCAL_5, <?=StringHash(name)?> + (index))
#
#endif
