#ifndef YDWEHeroLearnSkillAIIncluded
#define YDWEHeroLearnSkillAIIncluded

#include "YDWEBase.j"

//===========================================================================
//AI-自动学习技能 
//===========================================================================
library YDWEHeroLearnSkillAI requires YDWEBase
 
private function AIStringFind takes string subject,string find,integer offset returns integer
    local integer len = StringLength(find)
    local integer pos = offset
    local string s
    local string str    
    if ( offset < 1 ) then
        set pos = 1
    endif 
    if ( find == "" ) then
        return -1
    endif   
    loop
        set s = SubString(subject, pos-1, pos+len-1)
        if ( s == find ) then
            return pos
        endif
        if ( SubString(subject, pos-1, pos) == "" ) then
            return -1
        endif
        set pos = pos + 1
    endloop
    return -1
endfunction                       

function YDWEAIRecordAbility takes integer uid,integer ab1,integer ab2,integer ab3,integer ab4,integer ab5 returns nothing
    call YDWESaveIntegerByString(I2S(uid),"HeroAbilities_1",ab1)
    call YDWESaveIntegerByString(I2S(uid),"HeroAbilities_2",ab2)
    call YDWESaveIntegerByString(I2S(uid),"HeroAbilities_3",ab3)
    call YDWESaveIntegerByString(I2S(uid),"HeroAbilities_4",ab4)
    call YDWESaveIntegerByString(I2S(uid),"HeroAbilities_5",ab5)
endfunction

function YDWEAIRecordLearn_sequence takes integer uid,string ls1,string ls2,string ls3,string ls4,string ls5 returns nothing
    call YDWESaveStringByString(I2S(uid),"learn_quence_1",ls1)
    call YDWESaveStringByString(I2S(uid),"learn_quence_2",ls2)
    call YDWESaveStringByString(I2S(uid),"learn_quence_3",ls3)
    call YDWESaveStringByString(I2S(uid),"learn_quence_4",ls4)
    call YDWESaveStringByString(I2S(uid),"learn_quence_5",ls5)
endfunction

function YDWEHeroLearnSkillAI takes unit hero,integer level returns integer
    local string learn_sequence
    local integer skills
    local integer i = 1
    local integer uid=GetUnitTypeId(hero)    
    loop
        exitwhen i > 5
		set skills = YDWEGetIntegerByString(I2S(uid),"HeroAbilities_"+I2S(i))	
		set learn_sequence = YDWEGetStringByString(I2S(uid),"learn_quence_"+I2S(i))    	
        if AIStringFind(learn_sequence , "^" + I2S(level) + "^", 1) != -1 then         
            if GetUnitAbilityLevel(hero,skills)==0 then      
                call UnitAddAbility(hero,skills)
              else      
                call IncUnitAbilityLevel(hero,skills)    
            endif            
            call UnitModifySkillPoints(hero, -1)               
			//call SelectHeroSkill(hero,skills)
			//call DisplayTimedTextToPlayer(GetLocalPlayer(), 0, 0, 3600, ( "学习  " + GetAbilityName(skills)+ "，" +  "目前技能等级为" +I2S(GetUnitAbilityLevel(hero,skills))))   
            return skills
        endif
        set i = i + 1
    endloop
    return 0
endfunction                                                 

function YDWEHeroMaxSkillAI takes unit hero returns nothing
    local integer level = GetHeroLevel(hero)
    local integer uid = GetUnitTypeId(hero)
    local integer i=0
    call UnitModifySkillPoints(hero, level - GetHeroSkillPoints(hero))
    loop 
        set i = i + 1 
        exitwhen i>5
        call UnitRemoveAbility(hero,YDWEGetIntegerByString(I2S(uid),"HeroAbilities_"+I2S(i)))         
    endloop   
    set i = 0
    loop  
        set i = i + 1         
        exitwhen i>level           
        call  YDWEHeroLearnSkillAI(hero,i)
    endloop    
endfunction
endlibrary 

#endif /// YDWEHeroLearnSkillAIIncluded
