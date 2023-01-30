% check variable <name>,
% compare <value> against <expected> value
% if the absolute or relative difference is greater than the margin, complain and return 1
% if the expected value is 0, the relative margin is ignored
% relative margin is in percent
function [fail, MAX_DIFF, MAX_DIFF_REL] = test_mfemm_check_B_abs_ref_val(name, value, expected, marginAbs, marginRel, MAX_DIFF, MAX_DIFF_REL)

    diff = value - expected;

    diffRel = 0;

    if (expected ~= 0)
        diffRel = 100.*diff ./ expected;
    end
    
    if abs(diff) > marginAbs || abs(diffRel) > marginRel
        fail = 1;
        result = "[FAILED] ";
    else
        fail = 0;
        result = "[  ok  ] ";
    end
    
    if abs(diff) > MAX_DIFF
        MAX_DIFF = abs(diff);
    end

    if abs(diffRel) > MAX_DIFF_REL
        MAX_DIFF_REL = abs(diffRel);
    end

    fprintf(1, '%s%s: %e (expected: %e, diff: %e [%e %%], margin: %e [%e %%})\n', ...
        result, name, value, expected, diff, diffRel,  marginAbs, marginRel);

end