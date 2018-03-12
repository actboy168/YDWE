local process = require 'process'

local backend = {}
backend.message = ''
backend.progress = nil
backend.report = {}
backend.lastreport = nil

local mt = {}
mt.__index = mt

function mt:update_out()
    if not self.out_rd then
        return
    end
    local n = self.process:peek(self.out_rd)
    if n == 0 then
        return
    end
    local r = self.out_rd:read(n)
    if r then
        self.output = self.output .. r
        return
    end
    self.out_rd:close()
    self.out_rd = nil
end

function mt:update_err()
    if not self.err_rd then
        return
    end
    local n = self.process:peek(self.err_rd)
    if n == 0 then
        return
    end
    local r = self.err_rd:read(n)
    if r then
        self.error = self.error .. r
        return
    end
    self.err_rd:close()
    self.err_rd = nil
end

function mt:update_pipe()
    self:update_out()
    self:update_err()
    if not self.process:is_running() then
        self.output = self.output .. self.out_rd:read 'a'
        self.error = self.error .. self.err_rd:read 'a'
        self.process:close()
        return true
    end
    return false
end

local function push_report(type, value)
    if not backend.report[type] then
        backend.report[type] = {}
    end
    backend.lastreport = {value}
    table.insert(backend.report[type], backend.lastreport)
end

function mt:update_message(pos)
    local msg = self.output:sub(1, pos):gsub("^%s*(.-)%s*$", "%1"):gsub('\t', ' ')
    if msg:sub(1, 1) == '-' then
        local key, value = msg:match('%-(%S+)%s(.+)')
        if key then
            if key == 'progress' then
                backend.progress = tonumber(value) * 100
            elseif key == 'report' then
                push_report('', value)
            elseif key:sub(1, 7) == 'report|' then
                push_report(key:sub(8), value)
            elseif key == 'tip' then
                backend.lastreport[2] = value
            end
            msg = ''
        end
    end
    if #msg > 0 then
        backend.message = msg
    end
    self.output = self.output:sub(pos+1)
end

function mt:update()
    if self.exited then
        return
    end
    if not self.closed then
        self.closed = self:update_pipe()
    end
    if #self.output > 0 then
        local pos = self.output:find('\n')
        if pos then
            self:update_message(pos)
        end
    end
    if #self.error > 0 then
        while true do
            local pos = self.output:find('\n')
            if not pos then
                break
            end
            self:update_message(pos)
        end
        self:update_message(-1)
        self.output = ''
        self.out_rd:close()
        self.out_rd = nil
        backend.message = '转换失败'
    end
    if self.closed then
        while true do
            local pos = self.output:find('\n')
            if not pos then
                break
            end
            self:update_message(pos)
        end
        self:update_message(-1)
        self.exited = true
        return true
    end
    return false
end

function backend:init(application, currentdir)
    self.application = application
    self.currentdir = currentdir
end

function backend:clean()
    self.message = ''
    self.progress = nil
    self.report = {}
    self.lastreport = nil
end

function backend:open(entry, commandline)
    local p = process()
    local stdout = p:std_output()
    local stderr = p:std_error()
    p:set_console('disable')
    if not p:create(self.application, ('"%s" -e "package.cpath=[[%s]]" "%s" %s'):format(self.application:string(), package.cpath, entry, commandline), self.currentdir) then
        return
    end
    self:clean()
    return setmetatable({
        process = p,
        out_rd = stdout, 
        err_rd = stderr,
        output = '',
        error = '',
    }, mt)
end

return backend
