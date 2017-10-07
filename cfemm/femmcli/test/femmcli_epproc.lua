-- femmcli_epproc.lua
-- The file femmcli_epproc.fee is the same as cfemm/esolver/test/test.fee
-- OUTPUT:
-- SUCCESS

SUCCESS=1

function check(name, value, expected, margin)
	diff=value - expected
	if abs(diff) > margin then
		SUCCESS=0
		result="[FAILED] "
	else
		result="[  ok  ] "
	end
	print(result .. name .. ": " .. value .. " (expected: " .. expected .. ", diff: " .. diff .. ", margin: " .. margin .. ")")
end


-- enable for additional output:
-- XFEMM_VERBOSE = 1

open("femmcli_epproc.fee")
ei_analyze(0)
ei_loadsolution()

V,Dx,Dy,Ex,Ey,ex,ey,nrg  = eo_getpointvalues(0.250, 0)

-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
check("V", V, 48.37056814422403, 0.02)
check("Dx", Dx, 1.157764975200258e-009, 0.0001)
check("Dy", Dy, 7.559208128960357e-011, 0.0001)
check("Ex", Ex, 32.68975650415626, 0.5)
check("Ey", Ey, 2.134359549590023, 0.5)
check("ex", ex, 4, 0.0001)
check("ey", ey, 4, 0.0001)
check("nrg", nrg, 1.900419790445539e-008, 0.01)

assert(SUCCESS)
write("SUCCESS\n")
