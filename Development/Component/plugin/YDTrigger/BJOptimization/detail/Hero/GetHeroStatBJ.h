# /*
#  *  GetHeroStatBJ
#  *
#  *  By actboy168
#  *
#  */
#
#ifndef INCLUDE_BJ_OPTIMIZATION_GET_HERO_STAT_BJ_H
#define INCLUDE_BJ_OPTIMIZATION_GET_HERO_STAT_BJ_H
#
#  include <YDTrigger/Common/switch.h>
#  include <YDTrigger/BJOptimization/detail/Hero/HeroStatCheck.h>
#
#  define GetHeroStatBJ(stat, hero, bonuses)            \
     YDTRIGGER_COMMON_SWITCH(HERO_STAT_CHECK_## stat, ( \
       GetHeroStr(hero, bonuses),                       \
       GetHeroAgi(hero, bonuses),                       \
       GetHeroInt(hero, bonuses),                       \
       ))
#
#endif
