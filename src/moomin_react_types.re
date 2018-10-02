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

type childrenT =
  | C_NULL
  | C_SINGLE(elementT)
  | C_MANY(array(elementT))
and elementT = {
  name: string,
  key: option(string),
  initialState: (~path: string, ~glEnv: Reprocessing.glEnvT) => unit,
  render: (~path: string, ~glEnv: Reprocessing.glEnvT) => childrenT,
  unmount: (~path: string) => unit
};

type componentT('state) = {
  internal: internalT('state),
  initialState: Reprocessing.glEnvT => 'state,
  render: selfT('state) => elementT
};

type recNodeT =
  | R_NULL
  | R_CHILD(string, elementT, recNodeT)
  | R_CHILDREN(stateMapT(recNodeT));
