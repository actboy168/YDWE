# /*
#  *  BJ函数优化 -- Group
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_GROUP_H
#define INCLUDE_BJ_OPTIMIZATION_GROUP_H
#                                                        
#  define GroupAddUnitSimple(unit, group)                        GroupAddUnit(group, unit)
#  define GroupRemoveUnitSimple(unit, group)                     GroupRemoveUnit(group, unit)
#  define ForceAddPlayerSimple(player, force)                    ForceAddPlayer(force, player)
#  define ForceRemovePlayerSimple(player, force)                 ForceRemovePlayer(force, player)
#  define GetUnitsInRangeOfLocAll(radius, location)              GetUnitsInRangeOfLocMatching(radius, location, null)
#  define GetUnitsOfPlayerAll(player)                            GetUnitsOfPlayerMatching(player, null)
#
# /*
#  *  有返回值的函数, 在某些场合下会出错，比如
#  *    call GetLastCreatedUnit()
#  *  但不建议这样写，某种意义下来说也算是一种错误检测吧。
#  */
#
#  define GetPlayersAll()                                        bj_FORCE_ALL_PLAYERS
#
#endif
