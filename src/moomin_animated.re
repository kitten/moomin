/* These Easing functions are mostly adapted from Popmotion
   See: https://github.com/Popmotion/popmotion/blob/master/packages/easing/src/index.ts
*/
module Easing = {
  let defaultOvershootStrength = 1.525;

  let createBackIn = (power, p) =>
    p ** 2. *. ((power +. 1.) *. p -. power);

  let reverse = (easing, p) =>
    1. -. easing(1. -. p);

  let mirror = (easing, p) => p <= 0.5
    ? easing(2. *. p) /. 2.
    : (2. -. easing(2. *. (1. -. p))) /. 2.;

  let linear = p => p;

  let easeIn = p => p ** 2.;
  let easeOut = reverse(easeIn);
  let easeInOut = mirror(easeIn);

  let circIn = p => 1. -. sin(acos(p));
  let circOut = reverse(circIn);
  let circInOut = mirror(circIn);

  let backIn = createBackIn(defaultOvershootStrength);
  let backOut = reverse(backIn);
  let backInOut = mirror(backIn);
};
