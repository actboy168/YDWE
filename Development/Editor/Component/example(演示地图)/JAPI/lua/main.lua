local CJ = require "jass.common"
local BJ = require "blizzard"

local trig = CJ.CreateTrigger()
BJ.TriggerRegisterAnyUnitEventBJ(trig, CJ.EVENT_PLAYER_UNIT_SPELL_EFFECT)

-- 下面大量使用了函数定义后直接使用的技巧，这样甚至不需要给函数安一个名字，所以称之为‘匿名函数’。
-- 好处是代码结构更加紧密，也省去了给函数想名字的烦恼。
-- zinc也有匿名函数，如果你使用过zinc，应该不会陌生。
CJ.TriggerAddCondition(trig, CJ.Condition(
	function ()
		-- |xxxx|不是一个标准的lua语法，而是ydwe lua的扩展语法，值和jass里'xxxx'一样
		-- 注意，在lua里'xxxx'是一个字符串，和"xxxx"一样。
		return CJ.GetSpellAbilityId() == 1095264354 --'AHhb'
	end
))
CJ.TriggerAddAction(trig,
	function () 
		local u = CJ.GetSpellTargetUnit()
		local n = 0
		-- 这里定义的两个局部变量，可以直接在下面的计时器函数直接使用。当然你依然可以使用hashtable来传递(但不推荐)。
		-- 这是和jass的最大差别。
		CJ.TimerStart(CJ.CreateTimer(), 1.00, true, 
			function () 
				if n == 8 then
					CJ.DestroyTimer(CJ.GetExpiredTimer())
				else
					n = n + 1						
					CJ.SetUnitState(u, CJ.UNIT_STATE_LIFE, 10 + CJ.GetUnitState(u, CJ.UNIT_STATE_LIFE))
					CJ.DestroyEffect(CJ.AddSpecialEffectTarget("Abilities\\Spells\\Human\\HolyBolt\\HolyBoltSpecialArt.mdl", u, "overhead"))
				end
			end
		)
	end
)

CJ.TimerStart(CJ.CreateTimer(), 0.1, true, 
	function () 
		print('ha!')
	end
)
