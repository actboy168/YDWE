return function (w2l, w3i, w3f, input_ar, output_ar)
    input_ar:close()
    if w2l.setting.mode == 'lni' then
        output_ar:flush()
    end
    local suc, res = output_ar:save(w3i, w3f, w2l)
    if not suc then
        w2l:failed(res or lang.script.CREATE_FAILED)
    end
    output_ar:close()
end
