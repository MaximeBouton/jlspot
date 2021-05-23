module Spot2

using CxxWrap 
@wrapmodule joinpath(@__DIR__, "../../build/lib/libjlspot")

function __init__()
    @initcxx
end

export Formula, parse_formula

end
