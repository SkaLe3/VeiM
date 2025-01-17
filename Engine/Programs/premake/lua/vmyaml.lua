
local VMYAML = {}
VMYAML.__index = VMYAML

-- Constructor: Create a new VMYAML object
function VMYAML:new()
    return setmetatable({ yaml = "", indent = 0 }, self)
end

-- Internal method to emit a table or scalar value
function VMYAML:_emit_value(key, value, is_array)
    local prefix = string.rep("  ", self.indent)

    if type(value) == "table" then
        if is_array then
            self.yaml = self.yaml .. prefix .. "-\n"
        else
            self.yaml = self.yaml .. prefix .. tostring(key) .. ":\n"
        end

        -- Recurse into the nested table
        self.indent = self.indent + 1
        for k, v in pairs(value) do
            local is_nested_array = #value > 0
            self:_emit_value(k, v, is_nested_array)
        end
        self.indent = self.indent - 1
    else
        if is_array then
            self.yaml = self.yaml .. prefix .. "- " .. tostring(value) .. "\n"
        else
            self.yaml = self.yaml .. prefix .. tostring(key) .. ": " .. tostring(value) .. "\n"
        end
    end
end

-- Method to add a single key-value pair
function VMYAML:add(key, value)
    self:_emit_value(key, value, false)
end

-- Method to emit an entire table at once
function VMYAML:add_table(tbl)
    local is_array = #tbl > 0
    for k, v in pairs(tbl) do
        self:_emit_value(k, v, is_array)
    end
end

-- Method to get the current YAML state
function VMYAML:get_yaml()
    return self.yaml
end

-- Method to clear the current YAML state
function VMYAML:clear()
    self.yaml = ""
    self.indent = 0
end

-- Standalone function to convert a table to YAML
function VMYAML.table_to_yaml(tbl, indent)
    indent = indent or 0
    local yaml = ""
    local prefix = string.rep("  ", indent)

    if type(tbl) == "table" then
        -- Handle lists (arrays) and maps (key-value pairs)
        local is_array = #tbl > 0
        for k, v in pairs(tbl) do
            if type(v) == "table" then
                if is_array then
                    yaml = yaml .. prefix .. "-\n" .. VMYAML.table_to_yaml(v, indent + 1)
                else
                    yaml = yaml .. prefix .. tostring(k) .. ":\n" .. VMYAML.table_to_yaml(v, indent + 1)
                end
            else
                if is_array then
                    yaml = yaml .. prefix .. "- " .. tostring(v) .. "\n"
                else
                    yaml = yaml .. prefix .. tostring(k) .. ": " .. tostring(v) .. "\n"
                end
            end
        end
    else
        -- Handle scalar values (unlikely in this context)
        yaml = yaml .. prefix .. tostring(tbl) .. "\n"
    end

    return yaml
end

return VMYAML
