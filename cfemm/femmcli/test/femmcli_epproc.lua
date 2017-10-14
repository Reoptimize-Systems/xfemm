-- femmcli_epproc.lua
-- The file femmcli_epproc.fee is the same as cfemm/esolver/test/test.fee
-- SUCCESS
showconsole()

-- check variable <name>,
-- compare <value> against <expected> value
-- if the relative difference is greater than the margin (in percent), complain and return 1
function check(name, value, expected, margin)
	diff=100*(value - expected) / expected
	if abs(diff) > margin then
		fail=1
		result="[FAILED] "
	else
		fail=0
		result="[  ok  ] "
	end
	print(result .. name .. ": " .. value .. " (expected: " .. expected .. ", diff: " .. diff .. "%, margin: " .. margin .. "%)")
	--print("check(\""..name.."\", "..name..", "..value..", "..margin..")")
	return fail
end

-- enable for additional output:
-- XFEMM_VERBOSE = 1

open("femmcli_epproc.fee")
ei_analyze(0)
ei_loadsolution()

V,Dx,Dy,Ex,Ey,ex,ey,nrg  = eo_getpointvalues(0.250, 0)

-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
failed=0
failed= failed +check("V", V, 48.37056814422403, 1)
failed= failed +check("Dx", Dx, 1.157764975200258e-009, 1.5)
failed= failed +check("Dy", Dy, 7.559208128960357e-011, 15)
failed= failed +check("Ex", Ex, 32.68975650415626, 2)
failed= failed +check("Ey", Ey, 2.134359549590023, 15)
failed= failed +check("ex", ex, 4, 0.1)
failed= failed +check("ey", ey, 4, 0.1)
failed= failed +check("nrg", nrg, 1.900419790445539e-008, 3)

assert(failed==0)
write("SUCCESS\n")
