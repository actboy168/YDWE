local function create_lines(tab)
	local lines = {}
	local tabs = ('\t'):rep(tab or 0)
	lines.tab = tab or 0
	
	local function push(self, mo)
		local line = tabs .. mo
		table.insert(self, line)
		return function(...)
			self[#self] = line:format(...)
		end
	end

	return setmetatable(lines, {__call = push})
end

return function (w2l, chunk)
    --开始转化文本
    local lines    = create_lines(1)
    
    do
        
        --版本
        lines '[\'%s\']=%d,' ('VERSION', chunk.file_ver)
        lines '[\'%s\']=%d,' ('未知1', chunk.int_unknow_1)

        --全局变量
        local function f()
            local lines = create_lines(2)
            for i, var in ipairs(chunk.vars) do
                if var.is_array == 1 then
                    if var.value ~= '' then
                        lines '{%q, %q, %d, %q}' (var.type, var.name, var.array_size, var.value)
                    else
                        lines '{%q, %q, %d}' (var.type, var.name, var.array_size)
                    end
                else
                    if var.value ~= '' then
                        lines '{%q, %q, %d, %q}' (var.type, var.name, 0, var.value)
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
            local lines = create_lines(2)

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
            local lines = create_lines(2)

            for _, trigger in ipairs(chunk.triggers) do
                local function f()
                    local lines = create_lines(3)
                    
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
                            local lines = create_lines(4)
                            local lines_event = create_lines(5)
                            local lines_condition = create_lines(5)
                            local lines_action = create_lines(5)
                        
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
                                        print('eca类型错误', eca.type)
                                    end
                                end

                                local function f(tab)
                                    local lines = create_lines()

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
                                        local lines = create_lines()
                                        
                                        local function f(tab)
                                            local lines = create_lines(tab + 1)
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
                                                        local function f2(index, tab)
                                                            local lines = create_lines()
                                                            
                                                            local function f(tab)
                                                                local lines = create_lines(tab + 1)
                                                                
                                                                push_arg(index, lines)

                                                                return table.concat(lines, ',\r\n')
                                                            end

                                                            lines '[\'%s\']={\r\n%s' ('索引', f(tab))
                                                            --lines '}'
                                                            
                                                            return table.concat(lines, '\r\n')
                                                        end
                                                        
                                                        lines '{%q, %d, %s' (arg.value, arg.type, f2(arg.index, lines.tab))
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
                                    --print(eca.name, eca.child_eca_count)
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
                        lines '},'
                        
                    end
                    return table.concat(lines, '\r\n')
                end
                lines '{\r\n%s' (f(trigger))
                lines '},'
                
            end

            return table.concat(lines, '\r\n')
        end
        
        lines '[\'%s\']={\r\n%s' ('触发器', f())
        lines '},'
        
        
    end

    --io.save(file_name_out, table.concat(lines, '\r\n'):convert_wts(true))

    --io.save(file_name_out, table.concat(lines, '\r\n'))    --貌似wtg文件写入文本会出错
    return 'return\r\n{\r\n' .. table.concat(lines, '\r\n') .. '\r\n}\r\n'
end
