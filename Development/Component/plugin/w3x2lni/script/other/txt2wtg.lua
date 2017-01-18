local function txt2wtg(self, file_name_in, file_name_out)
    local content    = io.load(file_name_in)
    if not content then
        message('文件无效:' .. file_name_in:string())
        return
    end

    local index = 0
    local line
    local function read()
        local _
        _, index, line    = content:find('(%C+)', index + 1)
        if line and line:match '^%s*$' then
            return read()
        end
        return line
    end

    local chunk = {}
    
    --解析文本
    do
        --版本号
        repeat
            read()
            chunk.file_ver    = line:match 'VERSION%=(.+)'
        until chunk.file_ver

        --块
        local chunk_type, trigger
        while read() do
            --local line    = line
            local name    = line:match '^%s*【%s*(%S-)%s*】%s*$'
            if name then
                chunk_type    = name
                if name == 'Global' then
                    chunk.vars    = {}
                elseif name == 'Category' then
                    chunk.categories    = {}
                elseif name == 'Trigger' then
                    chunk.triggers    = {}
                end
                goto continue
            end

            --全局变量
            if chunk_type    == 'Global' then
                
                local type, s    = line:match '^%s*(%S-)%s+(.+)$'
                if not type then
                    goto continue
                end

                local var    = {}
                table.insert(chunk.vars, var)

                var.type    = type
                var.name, s    = s:match '^([%w_]+)(.*)$'
                if s then
                    var.array_size    = s:match '%[%s*(%d+)%s*%]'
                    var.value    = s:match '%=(.-)$'
                end

                --message(var.type, var.name, var.array_size, var.value)
                goto continue
            end

            --触发器类型(文件夹)
            if chunk_type    == 'Category' then
                
                local name, id, comment    = line:match '^%s*%[(.-)%]%(%s*(%d+)%s*%)%s*([%*]*)%s*$'
                if not name then
                    goto continue
                end
                
                local category     = {}
                table.insert(chunk.categories, category)

                category.name, category.id, category.comment    = name, id, comment == '*' and 1 or 0
                --message(name, id)

                goto continue
            end

            --触发器
            if chunk_type    == 'Trigger' then
                --读取ECA(最优先解读)
                local readEca, readArg

                function readEca(is_arg, is_child)
                    local eca_args, value    = line:match '^[\t]*(.-)%:(.-)$'

                    --message('line:' .. line)
                    if value then
                        local eca    = {}

                        --eca名字
                        eca.name    = value

                        --eca类型
                        eca.type    = tonumber(eca_args:match '%[%s*(%d+)%s*%]')
                        
                        --是否包含复合结构
                        eca.child_eca_count    = eca_args:match '%<%s*(%d+)%s*%>'

                        if is_arg then
                            --是否是参数
                            is_arg.eca    = eca
                        elseif is_child then
                            --是否是子项
                            table.insert(is_child, eca)

                            --子项ID
                            eca.child_id    = eca_args:match '%(%s*(%d+)%s*%)'
                        else
                            table.insert(trigger.ecas, eca)
                        end

                        --是否允许
                        eca.enable    = eca_args:match '[%*%#]'

                        --读取这个ECA下有多少参数
                        --message(eca.type, eca.name)
                        local state_args    = self.function_state
                        [eca.type]
                        [eca.name]
                        .args
                        local arg_count    = #state_args
                        --message(arg_count)
                        eca.args    = {}

                        for i = 1, arg_count do
                            readArg(eca.args)
                        end

                        --读取这个ECA下有多少子项
                        if eca.child_eca_count then
                            eca.child_ecas    = {}
                            --message(eca.name, eca.child_eca_count)
                            for i = 1, eca.child_eca_count do
                                read()
                                readEca(false, eca.child_ecas)
                            end
                        end

                        return true
                    end
                end

                function readArg(args)
                    local arg_args, value     = read():match '^[\t]*(.-)%:(.-)$'
                    if value then
                        local arg    = {}
                        table.insert(args, arg)

                        --类型
                        arg.type    = tonumber(arg_args:match '%[%s*([%-%d]+)%s*%]')
                        arg.value    = value
                        arg.has_child    = arg_args:match '[%*%#]'

                        --有子数据
                        if arg.has_child == '*' then
                            --数组索引
                            arg.insert_index    = 1
                            --message(has_child .. ':child_index:' .. arg.value)
                            arg.args    = {}
                            readArg(arg.args)
                        elseif arg.has_child == '#' then
                            --函数调用
                            arg.insert_call        = 1
                            --message(has_child .. ':child_call:' .. arg.value)

                            --只有在函数调用时,参数中才会保存函数的名字
                            if arg.type ~= 3 then
                                arg.value = ''
                            end
                            
                            --函数调用的实际type为2
                            arg.type    = 2
                            readEca(arg)
                        end
                    end
                end

                if readEca() then
                    goto continue
                end
                
                --尝试读取触发器名字
                local name    = line:match '^%s*%<(.-)%>%s*$'
                if name then
                    trigger = {}
                    table.insert(chunk.triggers, trigger)

                    trigger.name    = name
                    trigger.ecas    = {}
                    
                    goto continue
                end

                --读取触发器参数
                local name, s    = line:match '^(.-)%=(.-)$'
                if name then
                    trigger[name]    = s

                    goto continue
                end
            end

            --全局数据
            local name, s    = line:match '^(.-)%=(.-)$'
            if name then
                chunk[name]    = s
            end

            :: continue ::
        end
    end

    --转换2进制
    local pack    = {}
    
    do
        --文件头
        table.insert(pack, ('c4l'):pack('WTG!', chunk.file_ver))

        --触发器类别
            --文件夹计数
            table.insert(pack ,('l'):pack(#chunk.categories))
            
            --遍历文件夹
            for _, category in ipairs(chunk.categories) do
                table.insert(pack, ('lzl'):pack(category.id, category.name, category.comment))
            end

        --全局变量
            --计数
            table.insert(pack, ('ll'):pack(chunk['未知1'], #chunk.vars))

            --遍历全局变量
            for _, var in ipairs(chunk.vars) do
                table.insert(pack, ('zzllllz'):pack(
                    var.name,                    --名字
                    var.type,                    --类型
                    1,                            --永远是1
                    var.array_size and 1 or 0,    --是否是数组
                    var.array_size or 1,        --数组大小(非数组是1)
                    var.value and 1 or 0,        --是否有自定义初始值
                    var.value or ''                --自定义初始值
                ))
            end

        --触发器
            --计数
            table.insert(pack, ('l'):pack(#chunk.triggers))

            --遍历触发器
            for _, trigger in ipairs(chunk.triggers) do
                
                --触发器参数
                table.insert(pack, ('zzllllll'):pack(
                    trigger.name,
                    trigger['描述'],
                    trigger['类型'],
                    trigger['允许'],
                    trigger['自定义代码'],
                    trigger['初始化'],
                    trigger['初始化运行'],
                    trigger['类别']
                ))

                --ECA
                    --计数
                    table.insert(pack, ('l'):pack(#trigger.ecas))

                    --遍历ECA
                    local push_eca, push_arg
                    
                    function push_eca(eca)
                        --类型
                        table.insert(pack, ('l'):pack(eca.type))

                        --如果是复合结构,插入一个整数
                        if eca.child_id then
                            table.insert(pack, ('l'):pack(eca.child_id))
                        end

                        --名字,是否允许
                        table.insert(pack, ('zl'):pack(eca.name, eca.enable == '*' and 0 or 1))

                        --读取参数
                        for _, arg in ipairs(eca.args) do
                            push_arg(arg)
                        end

                        --复合结构
                        table.insert(pack, ('l'):pack(eca.child_eca_count or 0))

                        if eca.child_eca_count then
                            for _, eca in ipairs(eca.child_ecas) do
                                push_eca(eca)
                            end
                        end
                    end

                    function push_arg(arg)
                        table.insert(pack, ('lzl'):pack(
                            arg.type,                --类型
                            arg.value,                --值
                            arg.insert_call or 0    --是否插入函数调用
                        ))

                        --是否要插入函数调用
                        if arg.insert_call then
                            push_eca(arg.eca)

                            table.insert(pack, ('l'):pack(0)) --永远是0
                            return
                        end

                        --是否要插入数组索引
                        table.insert(pack, ('l'):pack(arg.insert_index or 0))

                        if arg.insert_index then
                            for _, arg in ipairs(arg.args) do
                                push_arg(arg)
                            end
                        end
                        
                    end
                    
                    for _, eca in ipairs(trigger.ecas) do
                        push_eca(eca)
                    end
            end

        --打包
        io.save(file_name_out, table.concat(pack))
        
    end
end

return txt2wtg
