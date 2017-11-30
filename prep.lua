#!/bin/lua
local ENV = setmetatable({}, {__index=_ENV})

function ENV.lit(s)
    return s:gsub('%W','%%%1')
end

function ENV.cat(s)
    return assert(io.open(s)):read('a')
end

function ENV.unind(s,c)
    local lines, i = {}, 1
    local m
    c = c or s:match'%s' or ' '
    for line in s:gmatch'[^\n]*' do
        local n = line:match('^'..ENV.lit(c)..'+')
        --io.stderr:write('n=',n or 'nil','\n')
        if n and #n > 0 and not m then
            m = #n
        end
        lines[i], i = line, i + 1
    end
    for j = 1, i - 1 do
        lines[j] = lines[j]:gsub('^'..ENV.lit(c):rep(m),'')
    end
    return table.concat(lines,'\n')
end

local chunki = 0

io.write((
    assert( io.open((...)) )
        :read('*a')
        :gsub(
            '%%LUA%%(.-)%%END%%',
            function(src)
                chunki = chunki + 1
                local ret = assert(assert(load(src, 'chunk#'..chunki, 't', ENV))(), 'chunk returned nil')
                io.stderr:write(ret,'\n')
                return ret
            end
             )
))
