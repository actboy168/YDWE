return function(path)
    local f = io.popen('git log -n 1', 'r')
    local lines = {}
    for l in f:lines() do
        lines[#lines+1] = l
    end
    local commit = lines[1]:match 'commit[ ]+([0-9|a-f]*)'
    local date
    for i = 2, 4 do
        if lines[i] and lines[i]:sub(1,5) == 'Date:' then
            date = lines[i]:match 'Date:[ ]+([0-9|a-z|A-Z|%+|%:| ]*)'
            break
        end
    end

    local f = io.open(path, 'w')
    f:write(([[
return {
    commit = '%s',
    date = '%s',
}]]):format(commit, date))
    f:close()
end
