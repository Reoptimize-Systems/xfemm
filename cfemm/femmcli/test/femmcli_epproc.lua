-- femmcli_epproc.lua
-- The file femmcli_epproc.fee is the same as cfemm/esolver/test/test.fee
-- SUCCESS
showconsole()

SUCCESS=1

-- check variable <name>,
-- compare <value> against <expected> value
-- if the relative difference is greater than the margin (in percent), complain and set global variable SUCCESS to false
function check(name, value, expected, margin)
	diff=100*(value - expected) / expected
	if abs(diff) > margin then
		SUCCESS=0
		result="[FAILED] "
	else
		result="[  ok  ] "
	end
	print(result .. name .. ": " .. value .. " (expected: " .. expected .. ", diff: " .. diff .. "%, margin: " .. margin .. "%)")
	--print("check(\""..name.."\", "..name..", "..value..", "..margin..")")
end


-- enable for additional output:
-- XFEMM_VERBOSE = 1

open("femmcli_epproc.fee")
ei_analyze(0)
ei_loadsolution()

V,Dx,Dy,Ex,Ey,ex,ey,nrg  = eo_getpointvalues(0.250, 0)

-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
check("V", V, 48.37056814422403, 2)
check("Dx", Dx, 1.157764975200258e-009, 1)
check("Dy", Dy, 7.559208128960357e-011, 1)
check("Ex", Ex, 32.68975650415626, 5)
check("Ey", Ey, 2.134359549590023, 5)
check("ex", ex, 4, 5)
check("ey", ey, 4, 5)
check("nrg", nrg, 1.900419790445539e-008, 5)

assert(SUCCESS)
write("SUCCESS\n")
