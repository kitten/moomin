type reprocessingLoopT('state) = {
  setup: Reprocessing.glEnvT => 'state,
  draw: ('state, Reprocessing.glEnvT) => 'state
};

/*
type valueT =
  | Nullary(ref(float))
  | Unary(valueT => float)
  | Binary((valueT, valueT) => float)
  | Ternary((valueT, valueT, valueT) => float);

module type ReprocessingLoop = {
  type state;
  let setup: Reprocessing.glEnvT => state;
  let draw: (state, Reprocessing.glEnvT) => state;
};

module StateMap = Belt.MutableMap.String;
type stateMapT('state) = Belt.MutableMap.String.t('state);

type internalT('state) = {
  name: string,
  states: stateMapT('state),
};

type selfT('state) = {
  glEnv: Reprocessing.glEnvT,
  state: 'state
};

type elementT = {
  name: string,
  initialState: (string, Reprocessing.glEnvT) => unit,
  render: (string, Reprocessing.glEnvT) => option(elementT),
  unmount: string => unit
};

type componentT('state) = {
  internal: internalT('state),
  initialState: Reprocessing.glEnvT => 'state,
  render: selfT('state) => elementT
};
*/
