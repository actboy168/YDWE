local function wtg2txt(self, file_name_in, file_name_out)
    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    local index    = 1
    local len    = #content

    local chunk    = {}
    local funcs    = {}
    local categories, category, vars, var, triggers, trigger, ecas, eca, args, arg

    --文件头
    function funcs.readHead()
        chunk.file_id,            --文件ID
        chunk.file_ver,            --文件版本
        index    = ('c4l'):unpack(content, index)
    end

    --触发器类别(文件夹)
    function funcs.readCategories()
        --触发器类别数量
        chunk.category_count, index    = ('l'):unpack(content, index)

        --初始化
        categories    = {}
        chunk.categories    = categories

        for i = 1, chunk.category_count do
            funcs.readCategory()
        end
    end

    function funcs.readCategory()
        category    = {}
        category.id, category.name, category.comment, index    = ('lzl'):unpack(content, index)

        table.insert(categories, category)
    end

    --全局变量
    function funcs.readVars()
        --全局变量数量
        chunk.int_unknow_1, chunk.var_count, index    = ('ll'):unpack(content, index)
        
        --初始化
        vars    = {}
        chunk.vars    = vars

        for i = 1, chunk.var_count do
            funcs.readVar()
        end
    end

    function funcs.readVar()
        var    = {}
        var.name,        --变量名
        var.type,        --变量类型
        var.int_unknow_1,    --(永远是1,忽略)
        var.is_array,    --是否是数组(0不是, 1是)
        var.array_size,    --数组大小(非数组是1)
        var.is_default,    --是否是默认值(0是, 1不是)
        var.value,        --初始数值
        index = ('zzllllz'):unpack(content, index)

        table.insert(vars, var)
        vars[var.name]    = var
    end

    --触发器
    function funcs.readTriggers()
        --触发器数量
        chunk.trigger_count, index    = ('l'):unpack(content, index)

        --初始化
        triggers    = {}
        chunk.triggers    = triggers

        for i = 1, chunk.trigger_count do
            funcs.readTrigger()
        end
    end

    function funcs.readTrigger()
        trigger    = {}
        trigger.name,        --触发器名字
        trigger.des,        --触发器描述
        trigger.type,        --类型(0普通, 1注释)
        trigger.enable,        --是否允许(0禁用, 1允许)
        trigger.wct,        --是否是自定义代码(0不是, 1是)
        trigger.init,        --是否初始化(0是, 1不是)
        trigger.run_init,    --地图初始化时运行
        trigger.category,    --在哪个文件夹下
        index    = ('zzllllll'):unpack(content, index)

        table.insert(triggers, trigger)
        --message('trigger:' .. trigger.name)
        --读取子结构
        funcs.readEcas()

    end

    --子结构
    function funcs.readEcas()
        --子结构数量
        trigger.eca_count, index    = ('l'):unpack(content, index)

        --初始化
        ecas    = {}
        trigger.ecas    = ecas

        for i = 1, trigger.eca_count do
            funcs.readEca()
        end
    end

    function funcs.readEca(is_child, is_arg)
        eca    = {}
        local eca    = eca
        
        eca.type,    --类型(0事件, 1条件, 2动作, 3函数调用)
        index    = ('l'):unpack(content, index)

        --是否是复合结构
        if is_child then
            eca.child_id, index    = ('l'):unpack(content, index)
        end

        --是否是参数中的子函数
        if is_arg then
            is_arg.eca    = eca
        else
            table.insert(ecas, eca)
        end
        
        eca.name,    --名字
        eca.enable,    --是否允许(0不允许, 1允许)
        index    = ('zl'):unpack(content, index)

        --message('eca:' .. eca.name)
        --读取参数
        funcs.readArgs(eca)

        --if,loop等复合结构
        eca.child_eca_count, index    = ('l'):unpack(content, index)
        for i = 1, eca.child_eca_count do
            funcs.readEca(true)
        end
    end

    --参数
    function funcs.readArgs(eca)
        --初始化
        args    = {}
        local args    = args
        eca.args    = args

        --message(eca.type, eca.name)
        local state_args    = self.function_state[eca.type][eca.name].args
        local arg_count    = #state_args

        --message('args:' .. arg_count)

        for i = 1, arg_count do
            funcs.readArg(args)
        end

    end

    function funcs.readArg(args)
        arg    = {}

        arg.type,             --类型(0预设, 1变量, 2函数, 3代码)
        arg.value,            --值
        arg.insert_call,    --是否需要插入调用
        index    = ('lzl'):unpack(content, index)
        --message('var:' .. arg.value)

        --是否是索引
        table.insert(args, arg)

        --插入调用
        if arg.insert_call == 1 then
            funcs.readEca(false, arg)
            arg.int_unknow_1, index    = ('l'):unpack(content, index) --永远是0
            --message(arg.int_unknow_1)
            return
        end

        arg.insert_index,    --是否需要插入数组索引
        index    = ('l'):unpack(content, index)

        --插入数组索引
        if arg.insert_index == 1 then
            funcs.readArg(args)
        end
    end

    --开始解析
    do
        funcs.readHead()
        funcs.readCategories()
        funcs.readVars()
        funcs.readTriggers()
    end

    --开始转化文本
    local lines    = string.create_lines(1)
    
    do
        
        --版本
        lines '[\'%s\']=%d,' ('VERSION', chunk.file_ver)
        lines '[\'%s\']=%d,' ('未知1', chunk.int_unknow_1)

        --全局变量
        local function f()
            local lines = string.create_lines(2)
            for i, var in ipairs(chunk.vars) do
                if var.is_array == 1 then
                    if var.value ~= '' then
                        lines '{%q, %q, %d, %q}' (var.type, var.name, var.array_size, var.value)
                    else
                        lines '{%q, %q, %d}' (var.type, var.name, var.array_size)
                    end
                else
                    if var.value ~= '' then
                        lines '{%q, %q, %d, %q}':format(var.type, var.name, 0, var.value)
                    else
                        lines '{%q, %q}' (var.type, var.name)
                    end
                end
            end
            return table.concat(lines, ',\r\n')
        end
        
        lines '[\'%s\']={\r\n%s' ('全局变量', f())
        lines '},'

        --触发器类别(文件夹)
        local function f()
            local lines = string.create_lines(2)

            for _, category in ipairs(chunk.categories) do
                lines '{%q, %d, %d}' (
                    category.name,
                    category.id,
                    category.comment
                )
            end

            return table.concat(lines, ',\r\n')
        end
        
        lines '[\'%s\']={\r\n%s' ('触发器类别', f())
        lines '},'
        

        --ECA结构
        

        --触发器
        local function f()
            local lines = string.create_lines(2)

            for _, trigger in ipairs(chunk.triggers) do
                local function f()
                    local lines = string.create_lines(3)
                    
                    lines '[\'%s\']=%q' ('名称', trigger.name)
                    lines '[\'%s\']=%q' ('描述', trigger.des)
                    lines '[\'%s\']=%d' ('类型', trigger.type)
                    lines '[\'%s\']=%d' ('允许', trigger.enable)
                    lines '[\'%s\']=%d' ('自定义代码', trigger.wct)
                    lines '[\'%s\']=%d' ('初始打开', trigger.init)
                    lines '[\'%s\']=%d' ('初始化运行', trigger.run_init)
                    lines '[\'%s\']=%d' ('类别', trigger.category)

                    --触发器ECA
                    local max        = #trigger.ecas
                    if max > 0 then
                        
                        local function f()
                            local lines = string.create_lines(4)
                            local lines_event = string.create_lines(5)
                            local lines_condition = string.create_lines(5)
                            local lines_action = string.create_lines(5)
                        
                            local tab    = 1
                            local ecas, index = trigger.ecas, 1

                            local function push_eca(eca, lines_arg)
                                if not eca then
                                    eca    = ecas[index]
                                    index    = index + 1
                                    if not eca then
                                        return false
                                    end
                                end
                                
                                local lines
                                if lines_arg then
                                    lines = lines_arg
                                else
                                    if eca.type == 0 then
                                        lines = lines_event
                                    elseif eca.type == 1 then
                                        lines = lines_condition
                                    elseif eca.type == 2 then
                                        lines = lines_action
                                    else
                                        message('eca类型错误', eca.type)
                                    end
                                end

                                local function f(tab)
                                    local lines = string.create_lines()

                                    lines '%q' (eca.name)
                                    if eca.enable == 0 then
                                        lines 'false'
                                    end

                                    return table.concat(lines, ', ')
                                end

                                if #eca.args == 0 then
                                    lines '{%s}' (f(lines.tab))
                                else
                                    --参数
                                    local function f2(tab)
                                        local lines = string.create_lines()
                                        
                                        local function f(tab)
                                            local lines = string.create_lines(tab + 1)
                                            local index = 1

                                            local function push_arg(arg, lines_arg)
                                                if not arg then
                                                    arg = eca.args[index]
                                                    index = index + 1
                                                    if not arg then
                                                        return
                                                    end
                                                end

                                                local lines = lines_arg or lines
                                                
                                                if arg.insert_call == 1 then
                                                    push_eca(arg.eca, lines)
                                                else
                                                    --索引
                                                    if arg.insert_index == 1 then
                                                        local function f2(tab)
                                                            local lines = string.create_lines()
                                                            
                                                            local function f(tab)
                                                                local lines = string.create_lines(tab + 1)
                                                                
                                                                push_arg(nil, lines)

                                                                return table.concat(lines, ',\r\n')
                                                            end

                                                            lines '[\'%s\']={\r\n%s' ('索引', f(tab))
                                                            --lines '}'
                                                            
                                                            return table.concat(lines, '\r\n')
                                                        end
                                                        
                                                        lines '{%q, %d, %s' (arg.value, arg.type, f2(lines.tab))
                                                        lines '}}'
                                                    else
                                                        lines '{%q, %d}' (arg.value, arg.type)
                                                        
                                                    end
                                                    
                                                    --[[
                                                    table.insert(lines, ('%s[%d]%s:%s'):format(
                                                        ('\t'):rep(tab),
                                                        arg.type,
                                                        (arg.insert_index == 1 or arg.insert_call == 1) and '*' or '',
                                                        arg.value
                                                    ))
                                                    --]]
                                                end
                                                return arg
                                            end

                                            while push_arg() do
                                            end

                                            return table.concat(lines, ',\r\n')
                                        end

                                        lines '[\'%s\']={\r\n%s' ('参数', f(tab))
                                        --lines '}'

                                        return table.concat(lines, '\r\n')
                                    end

                                    lines '{%s, %s' (f(lines.tab), f2(lines.tab))
                                    lines '}}'
                                end
                                --[[
                                table.insert(lines, ('%s%s[%d]%s%s:%s'):format(
                                    ('\t'):rep(tab),
                                    eca.child_id and ('(%d)'):format(eca.child_id) or '',
                                    eca.type,
                                    eca.child_eca_count == 0 and '' or ('<%d>'):format(eca.child_eca_count),
                                    (eca.enable == 0 and '*') or (is_arg and '#') or '',
                                    eca.name
                                ))
                                --]]

                                --[[
                                if eca.child_eca_count ~= 0 then
                                    --message(eca.name, eca.child_eca_count)
                                    tab    = tab + 1
                                    for i = 1, eca.child_eca_count do
                                        local eca    = ecas[index]
                                        index    = index + 1
                                        push_eca(eca)
                                    end
                                    tab    = tab - 1
                                end
                                --]]
                                return true
                            end
                            --ECA结构
                            while push_eca() do
                            end

                            lines '[\'%s\']={\r\n%s' ('事件', table.concat(lines_event, ',\r\n'))
                            lines '},'

                            lines '[\'%s\']={\r\n%s' ('条件', table.concat(lines_condition, ',\r\n'))
                            lines '},'

                            lines '[\'%s\']={\r\n%s' ('动作', table.concat(lines_action, ',\r\n'))
                            lines '},'

                            return table.concat(lines, '\r\n')
                        end
                        
                        lines '[\'%s\']={\r\n%s' ('触发', f())
                        lines '}'
                        
                    end
                    return table.concat(lines, ',\r\n')
                end
                lines '{\r\n%s' (f(trigger))
                lines '},'
                
            end

            return table.concat(lines, '\r\n')
        end
        
        lines '[\'%s\']={\r\n%s' ('触发器', f())
        lines '},'
        
        
    end

    io.save(file_name_out, table.concat(lines, '\r\n'):convert_wts(true))

    --io.save(file_name_out, table.concat(lines, '\r\n'))    --貌似wtg文件写入文本会出错
end

return wtg2txt
