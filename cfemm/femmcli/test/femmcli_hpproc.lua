-- femmcli_hpproc.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file femmcli_femfile.feh is the same as cfemm/hsolver/test/Temp0.feh
-- SUCCESS
showconsole()

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
check("T", T, 304.8641290114103, 0.01)
check("Fx", Fx, 0.2199070927061962, 0.01)
check("Fy", Fy, 0.1428113935654898, 0.01)
check("Gx", Gx, 8.313999477015031, 0.01)
check("Gy", Gy, 5.399252187839117, 0.01)
check("kx", kx, 0.02645021728882154, 0.01)
check("ky", ky, 0.02645021728882154, 0.01)

-- xfemm output before lua integration:
print("Checks against known pre-lua xfemm output:")
check("T", T, 304.864129, 0.0001)
check("Fx", Fx, 0.219907, 0.0001)
check("Fy", Fy, 0.142811, 0.0001)
check("Gx", Gx, 8.313999, 0.0001)
check("Gy", Gy, 5.399252, 0.0001)
check("kx", kx, 0.026450, 0.0001)
check("ky", ky, 0.026450, 0.0001)

assert(SUCCESS)
write("SUCCESS\n")
