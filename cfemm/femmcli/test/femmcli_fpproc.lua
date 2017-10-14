-- femmcli_fpproc.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file femmcli_femfile.fem is the same as cfemm/fsolver/test/Temp.fem
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

open("femmcli_fpproc.fem")
mi_analyze()
mi_loadsolution()

A,B1,B2,Sig,E,H1,H2,Je,Js,Mu1,Mu2,Pe,Ph = mo_getpointvalues(0.250, 0)

-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
check("A", A, 1.245741227364988e-014, 2)
check("B1", B1, -9.855007421888915e-014, 2)
check("B2", B2, 3.052725906923963e-014, 2)
check("Sig", Sig, 0, 2)
check("E", E, 4.235125240802008e-021, 3)
check("H1", H1, -7.842365727004682e-008, 2)
check("H2", H2, 2.429282089958189e-008, 2)
check("Je", Je, 0, 2)
check("Js", Js, 0, 2)
check("Mu1", Mu1, 1, 2)
check("Mu2", Mu2, 1, 2)
check("Pe", Pe, 0, 2)
check("Ph ", Ph , 0, 2)

assert(SUCCESS)
write("SUCCESS\n")
