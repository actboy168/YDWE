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
    local integer ydl_localvar_step = YDHashGet(YDLOC, integer, YDHashH2I(GetTriggeringTrigger()), 0xCFDE6C76) YDNL\
    set ydl_localvar_step = ydl_localvar_step + 3                                                              YDNL\
    call YDHashSet(YDLOC, integer, YDHashH2I(GetTriggeringTrigger()), 0xCFDE6C76, ydl_localvar_step)           YDNL\
    call YDHashSet(YDLOC, integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7, ydl_localvar_step)
#define YDLocalExecuteTrigger(trg)                 set ydl_triggerstep = YDHashH2I(trg)*(YDHashGet(YDLOC, integer, YDHashH2I(trg), 0xCFDE6C76) + 3)
#define YDLocalReset()                             YDHashSet(YDLOC, integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7, ydl_localvar_step)
# // 1.
#define YDLOCAL_1                                  YDHashH2I(GetTriggeringTrigger())*ydl_localvar_step
#define YDLocal1Set(type, name, value)             YDHashSet(YDLOC, type, YDLOCAL_1, <?=StringHash(name)?>, value)
#define YDLocal1ArraySet(type, name, index, value) YDHashSet(YDLOC, type, YDLOCAL_1, <?=StringHash(name)?> + (index), value)
#define YDLocal1Get(type, name)                    YDHashGet(YDLOC, type, YDLOCAL_1, <?=StringHash(name)?>)
#define YDLocal1ArrayGet(type, name, index)        YDHashGet(YDLOC, type, YDLOCAL_1, <?=StringHash(name)?> + (index))
#define YDLocal1Release()                          YDHashClearTable(YDLOC, YDLOCAL_1)
# // 2.
#define YDLOCAL_2                                  YDHashH2I(GetTriggeringTrigger())*YDHashGet(YDLOC, integer, YDHashH2I(GetTriggeringTrigger()), 0xECE825E7)
#define YDLocal2Set(type, name, value)             YDHashSet(YDLOC, type, YDLOCAL_2, <?=StringHash(name)?>, value)
#define YDLocal2ArraySet(type, name, index, value) YDHashSet(YDLOC, type, YDLOCAL_2, <?=StringHash(name)?> + (index), value)
#define YDLocal2Get(type, name)                    YDHashGet(YDLOC, type, YDLOCAL_2, <?=StringHash(name)?>)
#define YDLocal2ArrayGet(type, name, index)        YDHashGet(YDLOC, type, YDLOCAL_2, <?=StringHash(name)?> + (index))
# // 3.
#define YDLOCAL_3                                  YDHashH2I(GetExpiredTimer())
#define YDLocal3Set(type, name, value)             YDHashSet(YDLOC, type, YDLOCAL_3, <?=StringHash(name)?>, value)
#define YDLocal3ArraySet(type, name, index, value) YDHashSet(YDLOC, type, YDLOCAL_3, <?=StringHash(name)?> + (index), value)
#define YDLocal3Get(type, name)                    YDHashGet(YDLOC, type, YDLOCAL_3, <?=StringHash(name)?>)
#define YDLocal3ArrayGet(type, name, index)        YDHashGet(YDLOC, type, YDLOCAL_3, <?=StringHash(name)?> + (index))
#define YDLocal3Release()                          YDHashClearTable(YDLOC, YDLOCAL_3)
# // 4.
#define YDLOCAL_4                                  YDHashH2I(GetTriggeringTrigger())
#define YDLocal4Set(type, name, value)             YDHashSet(YDLOC, type, YDLOCAL_4, <?=StringHash(name)?>, value)
#define YDLocal4ArraySet(type, name, index, value) YDHashSet(YDLOC, type, YDLOCAL_4, <?=StringHash(name)?> + (index), value)
#define YDLocal4Get(type, name)                    YDHashGet(YDLOC, type, YDLOCAL_4, <?=StringHash(name)?>)
#define YDLocal4ArrayGet(type, name, index)        YDHashGet(YDLOC, type, YDLOCAL_4, <?=StringHash(name)?> + (index))
#define YDLocal4Release()                          YDHashClearTable(YDLOC, YDLOCAL_4)
# // 5.
#define YDLOCAL_5                                  ydl_triggerstep
#define YDLocal5Set(type, name, value)             YDHashSet(YDLOC, type, YDLOCAL_5, <?=StringHash(name)?>, value)
#define YDLocal5ArraySet(type, name, index, value) YDHashSet(YDHASH_HYDLOCANDLE, type, YDLOCAL_5, <?=StringHash(name)?> + (index), value)
#define YDLocal5Get(type, name)                    YDHashGet(YDLOC, type, YDLOCAL_5, <?=StringHash(name)?>)
#define YDLocal5ArrayGet(type, name, index)        YDHashGet(YDLOC, type, YDLOCAL_5, <?=StringHash(name)?> + (index))
#
#define YDLocalSet(page, type, name, value)             YDHashSet(YDLOC, type, YDHashH2I(page), <?=StringHash(name)?>, value)
#define YDLocalArraySet(page, type, name, index, value) YDHashSet(YDLOC, type, YDHashH2I(page), <?=StringHash(name)?> + (index), value)
#define YDLocalGet(page, type, name)                    YDHashGet(YDLOC, type, YDHashH2I(page), <?=StringHash(name)?>)
#define YDLocalArrayGet(page, type, name, index)        YDHashGet(YDLOC, type, YDHashH2I(page), <?=StringHash(name)?> + (index))
#define YDLocalRelease(page)                            YDHashClearTable(YDLOC, YDHashH2I(page))
#
#endif
