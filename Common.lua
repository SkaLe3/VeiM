-- Common.lua

-- Function to construct string with defines for resource files
function constructResourceDefineFlags(defines_list)
    local define_command = " /d"
    local options_list = {}  
        
    for _, str in ipairs(defines_list) do
        table.insert(options_list, define_command .. str)
    end
    local separator = " "
    return table.concat(options_list, separator)
end
