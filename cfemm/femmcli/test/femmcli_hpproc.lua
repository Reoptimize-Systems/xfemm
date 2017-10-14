-- femmcli_hpproc.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file femmcli_femfile.feh is the same as cfemm/hsolver/test/Temp0.feh
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

open("femmcli_hpproc.feh")
hi_analyze()
hi_loadsolution()

T,Fx,Fy,Gx,Gy,kx,ky= ho_getpointvalues(1.1,1.1)

-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
print("Checks against femm42 output:")
check("T", T, 304.8641290114103, 2)
check("Fx", Fx, 0.2199070927061962, 2)
check("Fy", Fy, 0.1428113935654898, 2)
check("Gx", Gx, 8.313999477015031, 2)
check("Gy", Gy, 5.399252187839117, 2)
check("kx", kx, 0.02645021728882154, 2)
check("ky", ky, 0.02645021728882154, 2)

-- xfemm output before lua integration:
print("Checks against known pre-lua xfemm output:")
check("T", T, 304.864129, 0.1)
check("Fx", Fx, 0.219907, 0.1)
check("Fy", Fy, 0.142811, 0.1)
check("Gx", Gx, 8.313999, 0.1)
check("Gy", Gy, 5.399252, 0.1)
check("kx", kx, 0.026450, 0.1)
check("ky", ky, 0.026450, 0.1)

assert(SUCCESS)
write("SUCCESS\n")
