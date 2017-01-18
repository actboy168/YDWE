local table_insert = table.insert
local table_remove = table.remove
local ipairs = ipairs

local function read_triggerdata(self, file_name_in)
    local function_state
    if self.function_state then
        function_state = self.function_state
    else
        function_state = {}
        self.function_state    = function_state
    end

    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    local funcs 
    funcs = {
        --检查关键字,判断函数域
        function (line)
            local trigger_type    = line:match '^%[(.+)%]$'
            if not trigger_type then
                return
            end

            if trigger_type    == 'TriggerEvents' then
                trigger_type    = 0
            elseif trigger_type    == 'TriggerConditions' then
                trigger_type    = 1
            elseif trigger_type    == 'TriggerActions' then
                trigger_type    = 2
            elseif trigger_type    == 'TriggerCalls' then
                trigger_type    = 3
            else
                funcs.trigger_type    = nil
                return
            end

            funcs.states    = {}
            funcs.trigger_type    = trigger_type
            function_state[trigger_type]    = funcs.states

        end,

        --检查函数
        function (line)
            if not funcs.trigger_type then
                return
            end

            local name, args    = line:match '^([^_].-)%=(.-)$'
            if not name then
                return
            end

            local state    = {}
            state.name    = name
            state.args    = {}

            for arg in args:gmatch '[^%,]+' do
                --排除部分参数
                if not tonumber(arg) and arg ~= 'nothing' then
                    table_insert(state.args, arg)
                end
            end
            --类型为调用时,去掉第一个返回值
            if funcs.trigger_type == 3 then
                table_remove(state.args, 1)
            end

            table_insert(funcs.states, state)
            funcs.states[state.name]    = state
        end,
    }

    --解析文本
    for line in content:gmatch '[^\n\r]+' do
        for _, func in ipairs(funcs) do
            if func(line) then
                break
            end
        end
    end

end

return read_triggerdata
