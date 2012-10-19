function b = magn (a, varargin)
%MAGN   Magnitude (or Euclidian norm) of vectors.
%   If A is a vector (e.g. a P×1, 1×P, or 1×1×P array):
%
%      B = MAGN(A) is a scalar (1×1), equal to the magnitude of vector A.
%
%      B = MAGN(A, DIM) is eqivalent to MAGN(A) if DIM is the non-singleton
%      dimension of A; it is equal to A if DIM is a singleton dimension.
%
%   If A is an N-D array containing more than one vector:
%
%      B = MAGN(A) is an N-D array containing the magnitudes of the
%      vectors constructed along the first non-singleton dimension of A.
%
%      B = MAGN(A, DIM) is an N-D array containing the magnitudes of the
%      vectors constructed along the dimension DIM of A.
%
%   Examples:
%   If A is a 3×10 matrix, then B = MAGN (A) is a 1×10 row vector listing
%   the magnitudes of the ten column vectors contained in A, and 
%   B = MAGN(A, 2) is a column vector (3×1) listing the magnitudes of the
%   three row vectors contained in A.
%
%   If A is a 4×5×25 3-D array, then B = MAGN (A, 2) is a 4×1×25 3-D array
%   containing the magnitudes of the 100 row vectors constructed along the
%   second dimension of A.
%   
%   See also UNIT, DOT2, CROSS2, CROSSDIV, OUTER, PROJECTION, REJECTION.

% $ Version: 2.0 $
% CODE      by:                 Paolo de Leva (IUSM, Rome, IT) 2009 Feb 1
% COMMENTS  by:                 Code author                    2009 Feb 4
% OUTPUT    tested by:          Code author                    2009 Feb 4
% -------------------------------------------------------------------------

b = sum(conj(a).*a, varargin{:}); % Almost equiv. to DOT(A, A, VARARGIN{:})
b = sqrt (b);