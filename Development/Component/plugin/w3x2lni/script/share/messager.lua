local proto = require 'share.protocol'

local messager = {}
function messager.text(text)
    proto.send('text', ('%q'):format(text))
end
function messager.raw(text)
    proto.send('raw', ('%q'):format(text))
end
function messager.title(title)
    proto.send('title', ('%q'):format(title))
end
function messager.progress(value)
    proto.send('progress', ('%.3f'):format(value))
end
function messager.report(type, level, content, tip)
    proto.send('report', ('{type=%q,level=%d,content=%q,tip=%q}'):format(type, level, content, tip))
end
function messager.exit(type, text)
    proto.send('exit', ('{type=%q,content=%q}'):format(type, text))
end
function messager.wait()
    proto.send('wait', '""')
end

local ext = require 'process.ext'
ext.set_filemode(io.stdout, 'b')
io.stdout:setvbuf 'no'

return messager
