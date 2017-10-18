-- femmcli_hpproc.lua
-- This test is basically the same as running fmesher and fsolver on the .fem file.
-- The file femmcli_femfile.feh is the same as cfemm/hsolver/test/Temp0.feh
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

open("femmcli_hpproc.feh")
hi_analyze()
hi_loadsolution()

T,Fx,Fy,Gx,Gy,kx,ky= ho_getpointvalues(1.1,1.1)

failed=0
-- check result against FEMM42 output:
-- FIXME: error margin needs sane values
print("Checks against femm42 output:")
failed = failed + check("T", T, 304.8641290114103, 2)
failed = failed + check("Fx", Fx, 0.2199070927061962, 4)
failed = failed + check("Fy", Fy, 0.1428113935654898, 4)
failed = failed + check("Gx", Gx, 8.313999477015031, 4)
failed = failed + check("Gy", Gy, 5.399252187839117, 4)
failed = failed + check("kx", kx, 0.02645021728882154, 2)
failed = failed + check("ky", ky, 0.02645021728882154, 2)

assert(failed==0)
write("SUCCESS\n")
