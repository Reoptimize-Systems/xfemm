-- femmcli_antiperiodicBC_flux.lua
-- This checks the correct flux density calculation for a motor with antiperiodic BC.
-- (The calculation contained errors in xfemm 2.0)
-- Output:
-- SUCCE

MAX_DIFF = 0
MAX_DIFF_REL = 0
-- check variable <name>,
-- compare <value> against <expected> value
-- if the absolute or relative difference is greater than the margin, complain and return 1
-- if the expected value is 0, the relative margin is ignored
-- relative margin is in percent
function check(name, value, expected, marginAbs, marginRel)
   diff=value - expected
   diffRel=0
   if (expected~=0) then
      diffRel=100*diff/expected
   end
   if abs(diff) > marginAbs or abs(diffRel) > marginRel then
      fail=1
      result="[FAILED] "
   else
      fail=0
      result="[  ok  ] "
   end
	if abs(diff) > MAX_DIFF then
		MAX_DIFF = abs(diff)
	end
	if abs(diffRel) > MAX_DIFF_REL then
		MAX_DIFF_REL = abs(diffRel)
	end
   print(result .. name .. ": " .. value .. " (expected: " .. expected
   .. ", diff: " .. diff .. " [" .. diffRel .. "%]"
      .. ", margin: " .. marginAbs .. " [" .. marginRel .. "%])")
   return fail
end


show_console()
open("femmcli_antiperiodicBC_AGE_TorqueBenchmark.fem")

mi_saveas("femmcli_antiperiodicBC_AGE_TorqueBenchmark.result.fem")

tolerance = 0.02
tolerance_rel = 70

-- reference values from analytical solution:
analytical_torque = {}
analytical_torque[0] = 0.0
analytical_torque[1] = 0.173648
analytical_torque[2] = 0.342020
analytical_torque[3] = 0.5
analytical_torque[4] = 0.642788
analytical_torque[5] = 0.766044
analytical_torque[6] = 0.866025
analytical_torque[7] = 0.939693
analytical_torque[8] = 0.984808
analytical_torque[9] = 1.0

failed=0
idx = 0
xfemm_torque = {};
angles = {}
for angle = 0, 90, 10 do

    angles[idx] = angle

    mi_modifyboundprop("AGE", 10, angle);

    mi_modifyboundprop("AGE", 11, 0);

    mi_analyze(1)

    mi_loadsolution()

    xfemm_torque[idx] = mo_gapintegral("AGE", 0)
		
    mo_close()

	idx = idx + 1

end

for print_idx = 0, idx-1, 1 do

    failed = failed +check("|T| @ " .. angles[print_idx] .. " degrees", xfemm_torque[print_idx], analytical_torque[print_idx], tolerance, tolerance_rel)

end
print("Maximum difference (abs,rel) = " .. MAX_DIFF ..", " ..MAX_DIFF_REL)
print("Maximum tolerated difference (abs,rel) = " .. tolerance .. ", " .. tolerance_rel)

assert(failed==0)
write("SUCCESS\n")
quit()
